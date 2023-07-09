#include "lifecycle_manager.h"

void LifeCycleManager::controlProcess(AbilityClient &client, const int &port, HeartbeatInfo &hbinfo, std::optional<CommandInfo> &cmdinfo)
{
}

void LifeCycleManager::handleHeartbeat(std::optional<CommandInfo> cmd_info)
{
}

void LifeCycleManager::AddHeartbeatInfo(HeartbeatInfo info)
{
}

void LifeCycleManager::check_timeout()
{
}

void LifeCycleManager::check_process()
{
}

Json::Value HeartbeatInfo::toJson(int port) const
{

    Json::Value j;
    j["abilityName"] = abilityName;
    j["status"] = status;
    j["last_update"] = std::chrono::duration_cast<std::chrono::milliseconds>(last_update.time_since_epoch()).count();
    j["port"] = port;
    j["abilityPort"] = abilityPort;
    return j;
}