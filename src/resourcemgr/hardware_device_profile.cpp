/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: hardware_device_profile.cpp
 * Description: hardware_device_profile.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "resourcemgr/hardware_device_profile.h"

Json::Value DeviceProfile::toJson() const {
  Json::Value root;
  for (const auto &cam : cameraDevices) {
    root["cameraDevices"].append(cam.toJson());
  }
  for (const auto &disp : displayDevices) {
    root["displayDevices"].append(disp.toJson());
  }
  for (const auto &mic : micDevices) {
    root["micDevices"].append(mic.toJson());
  }
  for (const auto &speaker : speakerDevices) {
    root["speakerDevices"].append(speaker.toJson());
  }
  return root;
}

void DeviceProfile::print() {
  std::cout << "cameraDevices: " << std::endl;
  for (auto &cam : cameraDevices) {
    cam.print();
  }
  std::cout << "displayDevices: " << std::endl;
  for (auto &disp : displayDevices) {
    disp.print();
  }
  std::cout << "micDevices: " << std::endl;
  for (auto &mic : micDevices) {
    mic.print();
  }
  std::cout << "speakerDevices: " << std::endl;
  for (auto &speaker : speakerDevices) {
    speaker.print();
  }
}

void DeviceProfile::FromJson2Profile(const Json::Value &root) {
  for (const auto &cam : root["cameraDevices"]) {
    CameraHardware hw;
    hw.fromJson(cam);
    cameraDevices.emplace_back(hw);
  }
  for (const auto &disp : root["displayDevices"]) {
    DisplayHardware hw;
    hw.fromJson(disp);
    displayDevices.emplace_back(hw);
  }
  for (const auto &mic : root["micDevices"]) {
    AudioHardware hw;
    hw.fromJson(mic);
    micDevices.emplace_back(hw);
  }
  for (const auto &speaker : root["speakerDevices"]) {
    AudioHardware hw;
    hw.fromJson(speaker);
    speakerDevices.emplace_back(hw);
  }
}
