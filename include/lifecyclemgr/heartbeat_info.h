#ifndef HEARTBEAT_INFO_H
#define HEARTBEAT_INFO_H

#include <string>
#include <chrono>

#include "json/json.h"

struct HeartbeatInfo
{
    std::string abilityName;
    int abilityPort;
    std::string status;
    std::chrono::steady_clock::time_point last_update;

    Json::Value toJson(int port) const;
};

#endif //HEARTBEAT_INFO_H