/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: get_camera_hardware.h
 * Description: get_camera_hardware.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef TEST_BASE_FUNCTION_INCLUDE_GET_CAMERA_HARDWARE_H_
#define TEST_BASE_FUNCTION_INCLUDE_GET_CAMERA_HARDWARE_H_

#include <string>
#include <vector>

class CameraHardwareForTest {
   public:
    std::string identifier;
    std::string device_path;
    std::string driver;
    std::string card;
    std::string bus_info;
    std::vector<std::string> formats;

    CameraHardwareForTest &operator=(const CameraHardwareForTest &other);
    std::string ToKeyValue(std::string hostname) const;
    void print() const;
};

std::vector<CameraHardwareForTest> getCameraInfo();
void print(const std::vector<CameraHardwareForTest> &camera);

#endif  // TEST_BASE_FUNCTION_INCLUDE_GET_CAMERA_HARDWARE_H_
