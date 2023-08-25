/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: device_instance_info.h
 * Description: device_instance_info.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef RESOURCEMGR_DEVICE_INSTANCE_INFO_H_
#define RESOURCEMGR_DEVICE_INSTANCE_INFO_H_

#include <json/json.h>

#include <mutex>  // NOLINT [build/c++11]
#include <string>
#include <vector>

#include "resourcemgr/common_struct_defination.h"

// common part struct define

struct DStatus {
  bool occupancy;
};
struct Adeviceparam {
  std::string name;
  std::string type;
  std::string index;
};
struct Afunction {
  std::string name;
  std::vector<Adeviceparam> param;
  std::vector<Adeviceparam> returnparam;
};
struct Api {
  std::vector<Afunction> function;
};
struct Acapability {
  std::string name;
  std::vector<std::string> api;
};

/**
 * the base class of all the device
 */
class DeviceInstanceInfo {
 public:
  std::string apiVersion;
  std::string kind;
  Metadata metadata;
  DStatus status;
  Api api;
  std::vector<DeviceDescribe> devicelist;

  virtual std::string Marshal() const = 0;
  virtual std::string GetHardwareIdentifier() const = 0;
  virtual bool UpdateHardwareInfo(const Json::Value &info) = 0;
  virtual void EraseHardwareInfo() = 0;
  virtual Json::Value ToJson() const = 0;

  virtual bool FromJson(const Json::Value &jnode);
  virtual bool UnMarshal(const std::string &data);
  virtual bool updateInstance(const Json::Value &jnode) = 0;
  virtual std::string getInstanceVersion() const = 0;
};

#endif  // RESOURCEMGR_DEVICE_INSTANCE_INFO_H_
