#ifndef LIFECYCLEMGR_HEARTBEAT_INFO_H_
#define LIFECYCLEMGR_HEARTBEAT_INFO_H_

#include <json/json.h>

#include <chrono> // NOLINT [build/c++11]
#include <string>

struct HeartbeatInfo {
  std::string abilityName;  // ability name
  int IPCPort;              // ipc port
  int abilityPort;          // the ability port
  std::string status;       // the state of the current ability
  std::chrono::steady_clock::time_point last_update;

  Json::Value toJson() const;
  void FromJson(const Json::Value &root);
  bool IsOffline() const;
};

#endif  // LIFECYCLEMGR_HEARTBEAT_INFO_H_
