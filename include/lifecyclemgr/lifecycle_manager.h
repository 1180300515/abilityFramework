#ifndef LIFECYCLE_MANAGER_H
#define LIFECYCLE_MANAGER_H

#include <unordered_map>
#include <thread>
#include <memory>

#include "json/json.h"

#include "abilityclient.h"
#include "command_info.h"

struct HeartbeatInfo
{
    std::string abilityName;
    int abilityPort;
    std::string status;
    std::chrono::steady_clock::time_point last_update;

    Json::Value toJson(int port) const;
};

class LifeCycleManager
{
public:
    void handleHeartbeat(std::optional<CommandInfo> cmd_info);
    void AddHeartbeatInfo(HeartbeatInfo info);
    void check_timeout();
    void check_process();

private:
    std::unordered_map<int, std::thread> threads;
    std::unordered_map<int, std::unique_ptr<AbilityClient>> clients;
    std::unordered_map<int, HeartbeatInfo> heartbeat_map;
    std::mutex heartbeat_map_mutex;

    void controlProcess(AbilityClient &client, const int &port, HeartbeatInfo &hbinfo, std::optional<CommandInfo> &cmdinfo);
};

#endif // LIFECYCLE_MANAGER_H
