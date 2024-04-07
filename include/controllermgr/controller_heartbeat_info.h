/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: heartbeat_info.h
 * Description: heartbeat_info.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef CONTROLLER_HEARTBEAT_INFO_H_
#define CONTROLLER_HEARTBEAT_INFO_H_

#include <json/json.h>

#include <chrono>  // NOLINT [build/c++11]
#include <string>

class ControllerHeartbeatInfo
{
private:
public:
    int port;
    std::string name;
    std::chrono::steady_clock::time_point last_update;
    int fromJson(const Json::Value root);
    Json::Value toJson();
    bool isOffline();
    ControllerHeartbeatInfo();
    ~ControllerHeartbeatInfo();
};


#endif  // CONTROLLER_HEARTBEAT_INFO_H_
