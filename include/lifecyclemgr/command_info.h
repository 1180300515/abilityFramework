#ifndef LIFECYCLEMGR_COMMAND_INFO_H_
#define LIFECYCLEMGR_COMMAND_INFO_H_

#include <json/json.h>

#include <iostream>
#include <string>

struct CommandInfo {
  int IPCPort;
  std::string abilityName;
  std::string cmd;
  int connectPort;
  std::string connectIP;

  Json::Value toJson() const;
  void FromJson(const Json::Value &root);

  void print();
};

#endif  // LIFECYCLEMGR_COMMAND_INFO_H_
