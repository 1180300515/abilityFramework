#ifndef _COMMAND_INFO_H
#define _COMMAND_INFO_H

#include <string>
#include <iostream>

#include "json/json.h"

struct CommandInfo
{
    int IPCPort;
    std::string abilityName;
    std::string cmd;
    int connectPort;
    std::string connectIP;

    Json::Value toJson() const;
    void FromJson(const Json::Value &root);

    void print();
};

#endif // _COMMAND_INFO_H