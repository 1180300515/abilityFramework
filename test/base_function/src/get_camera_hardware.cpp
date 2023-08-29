/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: get_camera_hardware.cpp
 * Description: get_camera_hardware.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "include/get_camera_hardware.h"

#include <fcntl.h>
#include <glob.h>
#include <linux/videodev2.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

CameraHardwareForTest &CameraHardwareForTest::operator=(const CameraHardwareForTest &other)
{
    if (this != &other) {
        device_path = other.device_path;
        driver = other.driver;
        card = other.card;
        bus_info = other.bus_info;
        formats = other.formats;
    }
    return *this;
}

void CameraHardwareForTest::print() const
{
    std::cout << "device_path: " << device_path << std::endl;
    std::cout << "driver: " << driver << std::endl;
    std::cout << "card: " << card << std::endl;
    std::cout << "bus_info: " << bus_info << std::endl;
    std::cout << "formats: ";
    for (const auto &format : formats) {
        std::cout << "  " << format << " ";
    }
    std::cout << std::endl << std::endl;
}

std::string CameraHardwareForTest::ToKeyValue(std::string hostname) const
{
    std::stringstream ss;
    ss << "hostname:" << hostname << ",";
    ss << "device_path:" << device_path << ",";
    ss << "driver:" << driver << ",";
    ss << "card:" << card << ",";
    ss << "bus_info:" << bus_info << ",";
    ss << "formats:[";
    for (const auto &format : formats) {
        ss << format << " ";
    }
    ss << "]";
    return ss.str();
}

std::vector<CameraHardwareForTest> getCameraInfo()
{
    std::vector<CameraHardwareForTest> camera;
    glob_t glob_result;
    glob("/dev/video*", GLOB_TILDE, nullptr, &glob_result);
    for (unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
        const char *path = glob_result.gl_pathv[i];
        int fd = open(path, O_RDONLY);

        if (fd == -1) {
            std::cerr << "Failed to open " << path << std::endl;
            continue;
        }

        v4l2_capability cap;
        if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
            std::cerr << "Failed to get device capabilities for " << path << std::endl;
            close(fd);
            continue;
        }

        v4l2_fmtdesc fmt = {0};
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        char fourcc[5] = {0};
        std::vector<std::string> formats;
        while (ioctl(fd, VIDIOC_ENUM_FMT, &fmt) == 0) {
            strncpy(fourcc, reinterpret_cast<const char *>(&fmt.pixelformat), 4);
            formats.push_back(fourcc);
            fmt.index++;
        }

        CameraHardwareForTest device;
        device.device_path = path;
        device.driver = reinterpret_cast<const char *>(cap.driver);
        device.card = reinterpret_cast<const char *>(cap.card);
        device.bus_info = reinterpret_cast<const char *>(cap.bus_info);
        device.formats = formats;
        // camera_devices.push_back(device);

        bool device_exists = false;
        for (auto &existing_device : camera) {
            if (existing_device.bus_info == device.bus_info) {
                device_exists = true;
                if (existing_device.device_path > device.device_path) {
                    existing_device = device;  // Note: this requires CameraDevice to be assignable
                }
                break;
            }
        }

        if (!device_exists) {
            camera.push_back(device);
        }

        close(fd);
    }

    globfree(&glob_result);
    return camera;
}

void print(const std::vector<CameraHardwareForTest> &camera)
{
    for (const auto &iter : camera) {
        iter.print();
    }
}
