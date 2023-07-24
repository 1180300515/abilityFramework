#include "ability_relation_manager.h"

#include <sys/stat.h>
#include <iostream>

#include "glog/logging.h"

#include "yaml_json_converter.h"
#include "color.h"

void AbilityRelationManager::Init(std::function<std::vector<AbilityInfoExtract>()> callback_1, std::function<std::vector<std::string>(std::string)> callback_2)
{
    LOG(INFO) << L_GREEN << "init abilityrelation manager" << NONE;
    this->abilityinfoextract_callback_ = callback_1;
    this->hardwaredevicelist_callback_ = callback_2;
}

std::string AbilityRelationManager::GetAbilitySupport()
{
    auto list = this->abilityinfoextract_callback_();
    for (const auto & iter : list)
    {
        LOG(INFO) << iter.name;
    }
    auto treearray = this->generateDependTreeArray(list);
    auto treearraywithdevice = this->generateDependTreeArrayWithDevices(treearray);

    return treearraywithdevice.ToJson().toStyledString();
}

DependTreeArray AbilityRelationManager::generateDependTreeArray(std::vector<AbilityInfoExtract> &list)
{
    std::unordered_map<std::string, TreeNode> map;

    for (const auto &ability : list)
    {
        TreeNode node{ability, {}, 0};
        map[ability.name] = node;
    }

    for (const auto &ability : list)
    {
        for (const auto &depend : ability.depends.abilities)
        {
            if (map.count(depend) > 0 && map[depend].children.at(0).ability.name != "none")
            {
                map[depend].children.push_back(map[ability.name]);
                map[ability.name].level = map[depend].level + 1;
            }
        }
    }

    DependTreeArray treeArray;

    for (const auto &pair : map)
    {
        if (pair.second.level == 0)
        {
            treeArray.trees.push_back(pair.second);
        }
    }

    return treeArray;
}

DependTreeArray AbilityRelationManager::generateDependTreeArrayWithDevices(DependTreeArray &treeArray)
{
    DependTreeArray newTreeArray;

    for (auto &tree : treeArray.trees)
    {
        generateNodes(tree, newTreeArray);
    }

    return newTreeArray;
}

void AbilityRelationManager::generateNodes(TreeNode &node, DependTreeArray &treeArray)
{
    if (!node.ability.depends.devices.empty())
    {
        std::vector<std::vector<std::string>> deviceLists;

        for (const auto &device : node.ability.depends.devices)
        {
            auto deviceList = hardwaredevicelist_callback_(device);

            if (!deviceList.empty())
            {
                deviceLists.push_back(deviceList);
            }
            else
            {
                return; // If any required device list is empty, don't create any instance.
            }
        }

        // Generate all combinations of devices.
        std::vector<std::vector<std::string>> combinations = cartesianProduct(deviceLists);
        for (const auto &combination : combinations)
        {
            if (!fileExists(node.ability.name))
            {
                std::cout << "File " << node.ability.name << " does not exist.\n";
                return;
            }
            TreeNode newNode{node.ability, node.children, node.level};
            newNode.ability.depends.devices = combination; // Store the devices into the ability instance.
            treeArray.trees.push_back(newNode);
        }
    }

    for (auto &child : node.children)
    {
        generateNodes(child, treeArray);
    }
}

std::vector<std::vector<std::string>> AbilityRelationManager::cartesianProduct(const std::vector<std::vector<std::string>> &lists)
{

    std::vector<std::vector<std::string>> result;
    std::vector<std::string> combination(lists.size());

    size_t counter = 0;
    while (true)
    {
        for (size_t i = 0; i < lists.size(); ++i)
        {
            combination[i] = (lists[i])[counter % lists[i].size()];
        }

        result.push_back(combination);

        ++counter;
        for (size_t i = 0; i < lists.size(); ++i)
        {
            if (counter % lists[i].size() != 0)
            {
                break;
            }
            else if (i == lists.size() - 1)
            {
                return result;
            }
        }
    }
}

bool AbilityRelationManager::fileExists(const std::string &filename)
{
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}
