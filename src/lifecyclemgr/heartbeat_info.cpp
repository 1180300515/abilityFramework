#include "heartbeat_info.h"

Json::Value HeartbeatInfo::toJson() const
{
    Json::Value j;
    j["abilityName"] = abilityName;
    j["state"] = state;
    j["last_update"] = std::chrono::duration_cast<std::chrono::milliseconds>(last_update.time_since_epoch()).count();
    j["abilityPort"] = abilityPort;
    j["IPCPort"] = IPCPort;
    return j;
}

void HeartbeatInfo::parseJson(const Json::Value &root)
{
    abilityName = root["abilityName"].asString();
    IPCPort = root["IPCPort"].asInt();
    state = root["state"].asString();
    abilityPort = root["abilityPort"].asInt();
    last_update = std::chrono::steady_clock::now();
}

bool HeartbeatInfo::IsOffline() const
{
    auto now = std::chrono::steady_clock::now();
    if (now - last_update > std::chrono::seconds(20)) {
        return true;
    }
    return false;
}
