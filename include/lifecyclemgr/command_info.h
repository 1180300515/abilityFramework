/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: command_info.h
 * Description: command_info.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef LIFECYCLEMGR_COMMAND_INFO_H_
#define LIFECYCLEMGR_COMMAND_INFO_H_

#include <json/json.h>

#include <iostream>
#include <string>

struct CommandInfo {
    std::string abilityName;
    std::string cmd;
    int connectPort;
    std::string connectIP;
    unsigned long abilityId;
    unsigned long abilityInstanceId;

    Json::Value toJson() const;
    void FromJson(const Json::Value &root);

    void print();
};

#endif  // LIFECYCLEMGR_COMMAND_INFO_H_
