#include "controllermgr/controller_heartbeat_info.h"

ControllerHeartbeatInfo::ControllerHeartbeatInfo()
{

}

int ControllerHeartbeatInfo::fromJson(Json::Value root)
{
    name = root["name"].asString();
    port = root["port"].asInt();
    last_update = std::chrono::steady_clock::now();
    return 0;
}

bool ControllerHeartbeatInfo::isOffline()
{
    auto now = std::chrono::steady_clock::now();
    if (now - last_update > std::chrono::seconds(20)) {
        return true;
    }
    return false;
}

Json::Value ControllerHeartbeatInfo::toJson()
{
    Json::Value root;
    root["name"] = name;
    root["port"] = port;
    return root;
}

ControllerHeartbeatInfo::~ControllerHeartbeatInfo()
{
}