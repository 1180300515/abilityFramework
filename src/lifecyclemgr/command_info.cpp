#include "command_info.h"

CommandInfo::CommandInfo(int port, std::string abilityName, std::string cmd, int connectPort, std::string connectIP)
{
    this->port = port;
    this->abilityName = abilityName;
    this->cmd = cmd;
    this->connectPort = connectPort;
    this->connectIP = connectIP;
}

Json::Value CommandInfo::toJson()
{
    Json::Value root;
    root["port"] = port;
    root["abilityName"] = abilityName;
    root["cmd"] = cmd;
    root["connectPort"] = connectPort;
    root["connectIP"] = connectIP;
    return root;
}

void CommandInfo::print()
{
    std::cout << "port: " << port << std::endl;
    std::cout << "abilityName: " << abilityName << std::endl;
    std::cout << "cmd: " << cmd << std::endl;
    std::cout << "connectPort: " << connectPort << std::endl;
    std::cout << "connectIP: " << connectIP << std::endl;
}