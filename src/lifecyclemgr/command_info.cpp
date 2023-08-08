#include "lifecyclemgr/command_info.h"

Json::Value CommandInfo::toJson() const {
  Json::Value root;
  root["IPCPort"] = IPCPort;
  root["abilityName"] = abilityName;
  root["cmd"] = cmd;
  root["connectPort"] = connectPort;
  root["connectIP"] = connectIP;
  return root;
}

void CommandInfo::FromJson(const Json::Value &root) {
  IPCPort = root["IPCPort"].asInt();
  abilityName = root["abilityName"].asString();
  cmd = root["cmd"].asString();
  connectPort = root["connectPort"].asInt();
  connectIP = root["connectIP"].asString();
}

void CommandInfo::print() {
  std::cout << "IPCPort: " << IPCPort << std::endl;
  std::cout << "abilityName: " << abilityName << std::endl;
  std::cout << "cmd: " << cmd << std::endl;
  std::cout << "connectPort: " << connectPort << std::endl;
  std::cout << "connectIP: " << connectIP << std::endl;
}
