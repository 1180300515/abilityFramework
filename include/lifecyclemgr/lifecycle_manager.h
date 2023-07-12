#ifndef LIFECYCLE_MANAGER_H
#define LIFECYCLE_MANAGER_H

#include <unordered_map>
#include <thread>
#include <memory>

#include "heartbeat_info.h"
#include "abilityclient.h"
#include "command_info.h"

class LifeCycleManager
{
public:
    void HandleCommandInfo(std::optional<CommandInfo> cmd_info);
    bool AddHeartbeatInfo(int port, HeartbeatInfo info);

private:
    std::unordered_map<int, std::thread> threads;
    std::unordered_map<int, std::unique_ptr<AbilityClient>> clients;

    std::unordered_map<int, HeartbeatInfo> heartbeat_map;
    std::mutex heartbeat_map_lock;

    std::function<bool(std::string)> resourcemgr_checkexist;

    void lifeCycleDeal(AbilityClient &client, const int &port, HeartbeatInfo &hbinfo, std::optional<CommandInfo> &cmdinfo);

    bool start_process(const std::string &abilityName);
};

#endif // LIFECYCLE_MANAGER_H
