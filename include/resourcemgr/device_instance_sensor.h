/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: device_instance_sensor.h
 * Description: device_instance_sensor.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef RESOURCEMGR_DEVICE_INSTANCE_SENSOR_H_
#define RESOURCEMGR_DEVICE_INSTANCE_SENSOR_H_

#include <json/json.h>

#include <map>
#include <string>
#include <vector>

#include "resourcemgr/device_instance_info.h"

// sensor part
struct Sensorproperties {
  std::string vendor;
  std::string location;
  std::string interface;
};
struct SensorSpec {
  std::string version;
  std::string hostname;
  std::string kind;
  // a hardware indentify in order to combine the hardware info with the
  // instance,camera use "card",audio use "name",dispaly use "screen"
  std::string hardwareidentifier;

  std::vector<Acapability> capability1;
  std::vector<Acapability> capability2;
  Sensorproperties properties;
  // std::map<std::string, std::string> customprops;
};
class SensorInstance : public DeviceInstanceInfo {
 public:
  SensorSpec spec;
  std::string Marshal() const;
  Json::Value ToJson() const;
  std::string GetHardwareIdentifier() const;
  bool UpdateHardwareInfo(const Json::Value &info);
  void EraseHardwareInfo();
  bool FromJson(const Json::Value &jnode);
  bool UnMarshal(const std::string &data);
  bool updateInstance(const Json::Value &jnode);
  std::string getInstanceVersion() const;
};

#endif  // RESOURCEMGR_DEVICE_INSTANCE_SENSOR_H_
