#ifndef _LIFECYCLE_MANAGER_H
#define _LIFECYCLE_MANAGER_H

#include <unordered_map>
#include <thread>
#include <memory>

#include "heartbeat_info.h"
#include "abilityclient.h"
#include "command_info.h"

class LifeCycleManager
{
public:
    /**
     * @brief handle the command info
     * @param cmd_info
     */
    void HandleCommandInfo(std::optional<CommandInfo> cmd_info);
    /**
     * @brief add a heart beart info
     * @param port
     * @param info
     * @return
     */
    bool AddHeartbeatInfo(int port, HeartbeatInfo info);
    /**
     * @brief
     * @param callback
     */
    void Init(std::function<bool(std::string)> callback);
    std::string GetHeartbeatMap();

private:
    std::unordered_map<int, std::thread> threads;
    std::unordered_map<int, std::unique_ptr<AbilityClient>> clients;

    std::unordered_map<int, HeartbeatInfo> heartbeat_map;
    std::mutex heartbeat_map_lock;

    std::function<bool(std::string)> resourcemgr_checkexist; // resource manager check resource exist function

    void lifeCycleDeal(AbilityClient &client, const int &port, HeartbeatInfo &hbinfo, std::optional<CommandInfo> &cmdinfo);

    bool start_process(const std::string &abilityName);
};

#endif // _LIFECYCLE_MANAGER_H
