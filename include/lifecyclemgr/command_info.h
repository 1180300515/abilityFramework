#ifndef _COMMAND_INFO_H
#define _COMMAND_INFO_H

#include <string>
#include <iostream>

#include "json/json.h"

class CommandInfo
{
public:
    int port;
    std::string abilityName;
    std::string cmd;
    int connectPort;
    std::string connectIP;

    CommandInfo(int port, std::string abilityName, std::string cmd, int connectPort, std::string connectIP);

    Json::Value toJson();

    void print();
};

#endif // _COMMAND_INFO_H