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

#ifndef RESOURCEMGR_DEVICE_INSTANCE_GENERAL_H_
#define RESOURCEMGR_DEVICE_INSTANCE_GENERAL_H_

#include <json/json.h>

#include <map>
#include <string>
#include <vector>

#include "resourcemgr/device_instance_info.h"

class GeneralDeviceInstance : public DeviceInstanceInfo {
   public:
    Json::Value info;
    Json::Value spec;

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