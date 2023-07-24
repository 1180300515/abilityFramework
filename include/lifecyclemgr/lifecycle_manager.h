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
    void HandleCommandInfo(const CommandInfo &cmd_info);
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
     * @brief run lifecycle
     */
    void Run();
    /**
     * @brief get the heart beat map
     * @return
     */
    std::string GetHeartbeatMap();

private:
    std::unordered_map<std::string, std::thread> threads;                    // store ability lifecycle deal thread
    std::unordered_map<std::string, std::unique_ptr<AbilityClient>> clients; // store ability client (by grpc)

    std::unordered_map<std::string, HeartbeatInfo> heartbeat_map; // abilityName and the heart beat info
    std::mutex heartbeat_map_lock;

    std::thread checkTimeOutThread;
    std::thread checkProcessThread;

    std::function<bool(std::string)> resourcemgr_checkexist; // resource manager check resource exist function

    void lifeCycleDeal(AbilityClient &client, HeartbeatInfo &hbinfo, const CommandInfo &cmdinfo);

    bool start_process(const std::string &abilityName);
};

#endif // _LIFECYCLE_MANAGER_H
