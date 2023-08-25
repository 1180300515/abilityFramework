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
