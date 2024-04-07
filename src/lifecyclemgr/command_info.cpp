/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: command_info.cpp
 * Description: command_info.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "lifecyclemgr/command_info.h"

Json::Value CommandInfo::toJson() const
{
    Json::Value root;
    root["IPCPort"] = IPCPort;
    root["abilityName"] = abilityName;
    root["cmd"] = cmd;
    root["connectPort"] = connectPort;
    root["connectIP"] = connectIP;
    root["abilityId"] = abilityId;
    root["abilityInstanceId"] = abilityInstanceId;
    return root;
}

void CommandInfo::FromJson(const Json::Value &root)
{
    IPCPort = root["IPCPort"].asInt();
    abilityName = root["abilityName"].asString();
    cmd = root["cmd"].asString();
    connectPort = root["connectPort"].asInt();
    connectIP = root["connectIP"].asString();
    abilityId = root["abilityId"].asUInt64();
    abilityInstanceId = root["abilityInstanceId"].asUInt64();
}

void CommandInfo::print()
{
    std::cout << "IPCPort: " << IPCPort << std::endl;
    std::cout << "abilityName: " << abilityName << std::endl;
    std::cout << "cmd: " << cmd << std::endl;
    std::cout << "connectPort: " << connectPort << std::endl;
    std::cout << "connectIP: " << connectIP << std::endl;
    std::cout << "abilityId:" << abilityId << std::endl;
}
