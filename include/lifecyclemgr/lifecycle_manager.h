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
    void HandleCommandInfo(const std::string &cmd);
    /**
     * @brief add a heart beart info
     * @param info
     * @return
     */
    bool AddHeartbeatInfo(HeartbeatInfo info);
    /**
     * @brief
     * @param callback
     */
    void Init(std::function<bool(std::string)> callback);
    /**
     * @brief start lifecycle manager
     */
    void Run();
    /**
     * @brief get the heart beat map
     * @return
     */
    std::string GetHeartbeatMap();

private: 
    std::unordered_map<int, std::thread> threads;                    // store IPCPort and ability lifecycle deal thread
    std::unordered_map<int, std::unique_ptr<AbilityClient>> clients; // store IPCPort and ability client (by grpc)
    std::mutex clients_lock_;

    std::unordered_map<int, HeartbeatInfo> heartbeat_map; // IPCPort and the heart beat info
    std::mutex heartbeat_map_lock;

    std::thread checkClientThread;

    std::function<bool(std::string)> resourcemgr_checkexist; // resource manager check resource exist function

    void lifeCycleDeal(AbilityClient &client, HeartbeatInfo &hbinfo, const CommandInfo &cmdinfo);

    bool start_process(const std::string &abilityName);

    void checkAbilityClientAndTimeout();
};

#endif // _LIFECYCLE_MANAGER_H
