#ifndef _HEARTBEAT_INFO_H
#define _HEARTBEAT_INFO_H

#include <string>
#include <chrono>

#include "json/json.h"

struct HeartbeatInfo
{
    std::string abilityName; // ability name
    int IPCPort;             // ipc port
    int abilityPort;         // the ability port
    std::string status;      // the state of the current ability
    std::chrono::steady_clock::time_point last_update;

    Json::Value toJson() const;
    bool IsOffline() const;
};

#endif // _HEARTBEAT_INFO_H