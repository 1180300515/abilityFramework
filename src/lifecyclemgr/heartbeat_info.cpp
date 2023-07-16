#include "heartbeat_info.h"

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