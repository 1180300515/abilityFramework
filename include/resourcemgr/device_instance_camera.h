/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: device_instance_camera.h
 * Description: device_instance_camera.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef RESOURCEMGR_DEVICE_INSTANCE_CAMERA_H_
#define RESOURCEMGR_DEVICE_INSTANCE_CAMERA_H_

#include <json/json.h>

#include <map>
#include <vector>
#include <string>

#include "resourcemgr/device_instance_info.h"

// a cameradevice struct
struct CameraProperties {
  // get from the yaml
  std::string vendor;
  std::string resolution;
  std::string location;
  int wideAngle;
  std::string focusMethod;
  bool telephoto;
  std::string interface;
  // get from the hardware
  std::string devicePath;
  std::string driverName;
  std::string card;
  std::string busInfo;
  std::vector<std::string> supportFormat;
};
struct CameraSpec {
  std::string version;
  std::string hostname;
  std::string kind;
  std::string
      hardwareidentifier;  // a hardware indentify in order to combine the
                           // hardware info with the instance,camera use
                           // "card",audio use "name",dispaly use "screen"
  std::vector<Acapability> capability1;
  std::vector<Acapability> capability2;
  CameraProperties properties;
  // std::map<std::string, std::string> customprops;
};

class CameraInstance : public DeviceInstanceInfo {
 public:
  CameraSpec spec;

  std::string GetHardwareIdentifier() const;
  bool UpdateHardwareInfo(const Json::Value &info);
  void EraseHardwareInfo();
  std::string Marshal() const;
  Json::Value ToJson() const;

  bool FromJson(const Json::Value &jnode);
  bool UnMarshal(const std::string &data);
  bool updateInstance(const Json::Value &jnode);
  std::string getInstanceVersion() const;
};

#endif  // RESOURCEMGR_DEVICE_INSTANCE_CAMERA_H_
