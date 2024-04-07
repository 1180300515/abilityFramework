/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: heartbeat_info.cpp
 * Description: heartbeat_info.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "lifecyclemgr/heartbeat_info.h"

Json::Value HeartbeatInfo::toJson() const
{
    Json::Value j;
    j["abilityName"] = abilityName;
    j["status"] = status;
    j["last_update"] = std::chrono::duration_cast<std::chrono::milliseconds>(last_update.time_since_epoch()).count();
    j["abilityPort"] = abilityPort;
    j["IPCPort"] = IPCPort;
    j["abilityId"] = abilityId;
    j["abilityInstanceId"] = abilityInstanceId;
    return j;
}

void HeartbeatInfo::FromJson(const Json::Value &root)
{
    abilityName = root["abilityName"].asString();
    IPCPort = root["IPCPort"].asInt();
    status = root["status"].asString();
    abilityPort = root["abilityPort"].asInt();
    abilityId = root["abilityId"].asUInt64();
    abilityInstanceId = root["abilityInstanceId"].asUInt64();
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
