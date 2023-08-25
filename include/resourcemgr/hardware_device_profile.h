/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: hardware_device_profile.h
 * Description: hardware_device_profile.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef RESOURCEMGR_HARDWARE_DEVICE_PROFILE_H_
#define RESOURCEMGR_HARDWARE_DEVICE_PROFILE_H_

#include <iostream>
#include <vector>

#include "resourcemgr/hardware_audio.h"
#include "resourcemgr/hardware_camera.h"
#include "resourcemgr/hardware_display.h"

class DeviceProfile {
 public:
  std::vector<CameraHardware> cameraDevices;
  std::vector<DisplayHardware> displayDevices;
  std::vector<AudioHardware> micDevices;
  std::vector<AudioHardware> speakerDevices;

  Json::Value toJson() const;
  void FromJson2Profile(const Json::Value &root);
  void print();
};

#endif  // RESOURCEMGR_HARDWARE_DEVICE_PROFILE_H_
