/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: hardware_camera.h
 * Description: hardware_camera.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef RESOURCEMGR_HARDWARE_CAMERA_H_
#define RESOURCEMGR_HARDWARE_CAMERA_H_

#include <json/json.h>

#include <string>
#include <vector>

class CameraHardware {
   public:
    std::string identifier;
    std::string device_path;
    std::string driver;
    std::string card;
    std::string bus_info;
    std::vector<std::string> formats;

    CameraHardware &operator=(const CameraHardware &other);
    Json::Value toJson() const;
    void fromJson(const Json::Value &root);
    std::string ToKeyValue(std::string hostname) const;
    void print() const;
};

#endif  // RESOURCEMGR_HARDWARE_CAMERA_H_
