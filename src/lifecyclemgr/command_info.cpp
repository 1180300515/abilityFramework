#include "command_info.h"

CommandInfo::CommandInfo(int port, std::string abilityName, std::string cmd, int connectPort, std::string connectIP)
{
    this->IPCPort = port;
    this->abilityName = abilityName;
    this->cmd = cmd;
    this->connectPort = connectPort;
    this->connectIP = connectIP;
}

Json::Value CommandInfo::toJson() const
{
    Json::Value root;
    root["IPCPort"] = IPCPort;
    root["abilityName"] = abilityName;
    root["cmd"] = cmd;
    root["connectPort"] = connectPort;
    root["connectIP"] = connectIP;
    return root;
}

void CommandInfo::print()
{
    std::cout << "IPCPort: " << IPCPort << std::endl;
    std::cout << "abilityName: " << abilityName << std::endl;
    std::cout << "cmd: " << cmd << std::endl;
    std::cout << "connectPort: " << connectPort << std::endl;
    std::cout << "connectIP: " << connectIP << std::endl;
}