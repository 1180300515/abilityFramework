#include "controllermgr/controller_manager.h"
#include "utils/color.h"
void ControllerManager::updateControllerInfo(ControllerHeartbeatInfo info)
{
    httpPortMap[info.name] = info;
}

std::string ControllerManager::getControllerInfo()
{
    std::shared_lock<std::shared_mutex> r_lock(port_mutex);
    Json::Value data;
    for (auto iter : httpPortMap) {
        if (iter.second.isOffline()) {
            continue;
        }
        data.append(iter.second.toJson());
    }
    return data.toStyledString();
}

bool ControllerManager::start_process(const std::string &abilityName)
{
    pid_t pid = fork();

    if (pid < 0) {
        return false;
    } else if (pid > 0) {
    } else {
        std::string program_path = abilityName + "Controller";
        DLOG(INFO) << "start process : " << program_path;
        execl(("bin/" + program_path).c_str(), program_path.c_str(), reinterpret_cast<char *>(NULL));
        perror("execl fail : ");
        exit(0);
    }
    return true;
}

void ControllerManager::init(std::function<std::vector<AbilityInfoExtract>()> callback)
{
    DLOG(INFO) << L_GREEN << "init lifecycle manager" << NONE;
    this->abilityInfo_callback = callback;
    
}

bool ControllerManager::fileExists(const std::string &filename)
{
    struct stat buffer;
    return (stat(("bin/" + filename).c_str(), &buffer) == 0);
}

void ControllerManager::run()
{
    this->updateControllerThread = std::thread([this]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(4));
            auto list = this->abilityInfo_callback();
            for (const auto &iter : list) {
                auto status = httpPortMap.find(iter.name+"Controller");
                if (status == httpPortMap.end() || status->second.isOffline())
                {
                    DLOG(INFO) << iter.name << " start ability controller";
                    if (fileExists(iter.name + "Controller"))
                    {
                        if(!start_process(iter.name))
                        {
                            DLOG(ERROR) << iter.name << " failed to start ability controller";
                        }
                    } else {
                        DLOG(ERROR) << iter.name << "Controller" << " do not exist";
                    }
                }
            }
        }
    });
    
}