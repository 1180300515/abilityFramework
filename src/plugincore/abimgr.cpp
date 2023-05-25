#include "plugincore/abimgr_interface.h"
#include "plugincore/lifecycle_interface.h"
#include "controller/controller.h"
#include "utils/color.h"

HeartbeatMap heartbeat_map;
std::mutex heartbeat_map_mutex;
extern std::shared_ptr<Controller> controller;
ProcessController processController;



void print_heartbeat_info()
{
    // std::lock_guard<std::mutex> lock(heartbeat_map_mutex);
    for (const auto &pair : heartbeat_map)
    {
        std::cout << "AbilityName: " << pair.second.abilityName << ", Port: " << pair.first << ", Status: " << pair.second.status
                  << ", Last update: " << std::chrono::duration_cast<std::chrono::seconds>(pair.second.last_update.time_since_epoch()).count() << " seconds ago\n";
    }
}

void check_process(){
    while(true){
        std::cout << RED << "Enter Check Process thread" << NONE << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::lock_guard<std::mutex> lock(heartbeat_map_mutex);
        std::cout << RED << "Got Lock" << NONE << std::endl;
        processController.handleHeartbeat(heartbeat_map, std::nullopt);
    }
}

void check_timeout()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::lock_guard<std::mutex> lock(heartbeat_map_mutex);
        auto now = std::chrono::steady_clock::now();
        for (auto it = heartbeat_map.begin(); it != heartbeat_map.end();)
        {
            if (now - it->second.last_update > std::chrono::seconds(20))
            {
                it = heartbeat_map.erase(it); // 超时，删除记录
                std::cout << "Erased one record" << std::endl;
            }
            else
            {
                ++it;
            }
        }
    }
}

void run_http_server()
{
    auto svr = std::make_shared<httplib::Server>();

    svr->Post("/heartbeat", [](const httplib::Request &req, httplib::Response &res)
              {
        int port;
        try {
            port = std::stoi(req.get_param_value("port"));
            std::cout << BLUE <<"Got heartbeat from port " << port << NONE << std::endl;
        } catch (const std::exception&) {
            res.status = 400;  // Bad Request
            return;
        }

        std::lock_guard<std::mutex> lock(heartbeat_map_mutex);
        std::cout << BLUE <<"store to heartbeat_map, got status: " << req.get_param_value("status") << NONE << std::endl;
        heartbeat_map[port] = { req.get_param_value("abilityName"), std::stoi(req.get_param_value("abilityPort")), req.get_param_value("status"),  std::chrono::steady_clock::now()};
        
        std::cout << BLUE << "store to heartbeat_map finished" << NONE << std::endl;
        
        res.set_content("OK", "text/plain");

        print_heartbeat_info(); });

    svr->Get("/api/Devices", [](const httplib::Request &req, httplib::Response &res){ 
        res.set_content(deviceProfile.toJson().toStyledString(), "application/json"); });

    svr->Get("/api/AbilityRunning", [](const httplib::Request &req, httplib::Response &res){ 
        res.set_content(mapToJson(heartbeat_map).toStyledString(), "application/json"); });

    svr->Get("/api/AbilitySupport", [](const httplib::Request &req, httplib::Response &res){
        std::cout << RED << "controller's ability print: "<< NONE << std::endl;
        for (auto kv : controller->ability_instances){
            std::cout << RED << kv.first << NONE << std::endl;
        }
        AbilityInfoExtractList extractList = buildAbilityInfoExtractList(*controller);
        extractList.print();
        DependTreeArray dependTreeArray = GenerateDependTreeArray(extractList);

        for (const auto& tree : dependTreeArray.trees) {
            PrintTreeIterative(tree);
            std::cout << "\n";
        }

        DependTreeArray newTreeArray = GenerateDependTreeArrayWithDevices(devicePoolExtended, dependTreeArray);
        PrintDependTreeArray(newTreeArray);
        res.set_content(serializeDependTreeArray(newTreeArray).toStyledString(), "application/json"); });
    
    svr->Post("/api/AbilityRequest", [](const httplib::Request &req, httplib::Response &res){
        std::cout << RED << "Receive Ability Request "<< NONE << std::endl;
        auto cmd_g = CommandInfo{std::stoi(req.get_param_value("port")),
                                req.get_param_value("abilityName"),
                                req.get_param_value("cmd"),
                                std::stoi(req.get_param_value("connectPort")),
                                req.get_param_value("connectIP")};
        cmd_g.print();
        std::lock_guard<std::mutex> lock(heartbeat_map_mutex);
        std::cout << RED << "Got Lock and handle the cmd" << NONE << std::endl;
        processController.handleHeartbeat(heartbeat_map, cmd_g);
        std::cout << RED << "Exit handle the cmd" << NONE << std::endl;
    });

    svr->listen("0.0.0.0", 8080);
}

bool start_program(const std::string &program_path)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        // 错误处理：fork失败
        return false;
    }
    else if (pid > 0)
    {
        // 我们在父进程中，pid是子进程的PID
    }
    else
    {
        // 我们在子进程中，启动新的程序
        execl(program_path.c_str(), program_path.c_str(), (char *)NULL);

        // 如果execl返回，那么说明出错了
        return false;
    }

    return true;
}

Json::Value mapToJson(const std::unordered_map<int, HeartbeatInfo> &map)
{
    Json::Value j;
    for (const auto &kv : map)
    {
        j.append(kv.second.toJson(kv.first));
    }
    return j;
}

bool fileExists(const std::string& filename) {
    struct stat buffer;
    return (stat(("bin/" + filename).c_str(), &buffer) == 0);
}

std::string stripSlashPrefix(const std::string& str) {
    size_t pos = str.rfind('/');
    if (pos != std::string::npos) {
        return str.substr(pos + 1);
    }
    return str;
}



DependTreeArray GenerateDependTreeArray(AbilityInfoExtractList& list) {
    std::unordered_map<std::string, TreeNode> map;

    for (const auto& ability : list.abilities) {
        TreeNode node{ability, {}, 0};
        map[ability.name] = node;
    }

    for (const auto& ability : list.abilities) {
        for (const auto& depend : ability.depends.abilities) {
            if (map.count(depend) > 0 && map[depend].children.at(0).ability.name != "none") {
                map[depend].children.push_back(map[ability.name]);
                map[ability.name].level = map[depend].level + 1;
            }
        }
    }

    DependTreeArray treeArray;

    for (const auto& pair : map) {
        if (pair.second.level == 0) {
            treeArray.trees.push_back(pair.second);
        }
    }

    return treeArray;
}

AbilityInfoExtractList buildAbilityInfoExtractList(Controller& controller) {
    AbilityInfoExtractList extractList;

    for (const auto& abilityPair : controller.ability_instances) {
        const std::shared_ptr<Ability>& ability = abilityPair.second;

        AbilityInfoExtract abilityInfo;
        abilityInfo.name = stripSlashPrefix(abilityPair.first);
        abilityInfo.depends = ability->depends;

        extractList.abilities.push_back(abilityInfo);
    }

    return extractList;
}

void PrintTreeIterative(const TreeNode& root) {
    std::stack<std::pair<const TreeNode*, std::string>> stk;
    stk.push({&root, ""});

    while (!stk.empty()) {
        const TreeNode* node = stk.top().first;
        std::string indent = stk.top().second;
        stk.pop();

        std::cout << indent << "Name: " << node->ability.name << ", Level: " << node->level << "\n";

        for (auto it = node->children.rbegin(); it != node->children.rend(); ++it) {
            stk.push({&(*it), indent + "  "});
        }
    }
}

void GenerateNodes(TreeNode& node, DevicePoolExtended& devicePool, DependTreeArray& treeArray) {
    if (!node.ability.depends.devices.empty()) {
        std::vector<std::vector<std::string>*> deviceLists;

        for (const auto& device : node.ability.depends.devices) {
            std::vector<std::string>* deviceList = nullptr;
            
            if (device == "mic") {
                deviceList = &devicePool.micDevices;
            } else if (device == "speaker") {
                deviceList = &devicePool.speakerDevices;
            } else if (device == "camera") {
                deviceList = &devicePool.cameraDevices;
            } else if (device == "display") {
                deviceList = &devicePool.displayDevices;
            }

            if (deviceList && !deviceList->empty()) {
                deviceLists.push_back(deviceList);
            } else {
                return;  // If any required device list is empty, don't create any instance.
            }
        }

        // Generate all combinations of devices.
        std::vector<std::vector<std::string>> combinations = cartesianProduct(deviceLists);
        for (const auto& combination : combinations) {
            if(!fileExists(node.ability.name)) {
                std::cout << "File " << node.ability.name << " does not exist.\n";
                return;
            }
            TreeNode newNode{node.ability, node.children, node.level};
            newNode.ability.depends.devices = combination;  // Store the devices into the ability instance.
            treeArray.trees.push_back(newNode);
        }
    }

    for (auto& child : node.children) {
        GenerateNodes(child, devicePool, treeArray);
    }
}


DependTreeArray GenerateDependTreeArrayWithDevices(DevicePoolExtended& devicePool, DependTreeArray& treeArray) {
    DependTreeArray newTreeArray;

    for (auto& tree : treeArray.trees) {
        GenerateNodes(tree, devicePool, newTreeArray);
    }

    return newTreeArray;
}


std::vector<std::vector<std::string>> cartesianProduct(const std::vector<std::vector<std::string>*>& lists) {
    std::vector<std::vector<std::string>> result;
    std::vector<std::string> combination(lists.size());

    size_t counter = 0;
    while (true) {
        for (size_t i = 0; i < lists.size(); ++i) {
            combination[i] = (*lists[i])[counter % lists[i]->size()];
        }

        result.push_back(combination);

        ++counter;
        for (size_t i = 0; i < lists.size(); ++i) {
            if (counter % lists[i]->size() != 0) {
                break;
            } else if (i == lists.size() - 1) {
                return result;
            }
        }
    }
}

void PrintDependTreeArray(const DependTreeArray& treeArray) {
    int treeIdx = 1;
    for (const auto& tree : treeArray.trees) {
        std::cout << "Tree " << treeIdx++ << ":\n";
        PrintTreeNode(tree);
    }
}

void PrintTreeNode(const TreeNode& node) {
    std::string indent(node.level * 2, ' ');

    std::cout << indent << "AbilityName: " << node.ability.name << ", Level: " << node.level << "\n";
    std::cout << indent << "Depends on devices: ";
    for (const auto& device : node.ability.depends.devices) {
        std::cout << device << " ";
    }
    std::cout << "\n";

    for (const auto& child : node.children) {
        PrintTreeNode(child);
    }
}

Json::Value serializeNode(const TreeNode& node) {
    Json::Value jsonNode;
    jsonNode["name"] = node.ability.name;
    jsonNode["level"] = node.level;
    for (const auto& ability : node.ability.depends.abilities) {
        jsonNode["depends"]["abilities"].append(ability);
    }
    for (const auto& device : node.ability.depends.devices) {
        jsonNode["depends"]["devices"].append(device);
    }
    for (const auto& child : node.children) {
        jsonNode["children"].append(serializeNode(child));
    }
    return jsonNode;
}

Json::Value serializeDependTreeArray(const DependTreeArray& treeArray) {
    Json::Value jsonTreeArray;
    for (const auto& tree : treeArray.trees) {
        jsonTreeArray.append(serializeNode(tree));
    }
    return jsonTreeArray;
}