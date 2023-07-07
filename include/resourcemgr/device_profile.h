#ifndef DEVICE_PROFILE_H
#define DEVICE_PROFILE_H

#include <iostream>

#include "audio_hardware.h"
#include "camera_hardware.h"
#include "display_hardware.h"

class DeviceProfile
{
public:
    std::vector<CameraHardware> cameraDevices;
    std::vector<DisplayInfo> displayDevices;
    std::vector<AudioDevice> micDevices;
    std::vector<AudioDevice> speakerDevices;

    Json::Value toJson() const
    {
        Json::Value root;
        for (const auto &cam : cameraDevices)
            root["cameraDevices"].append(cam.toJson());
        for (const auto &disp : displayDevices)
            root["displayDevices"].append(disp.toJson());
        for (const auto &mic : micDevices)
            root["micDevices"].append(mic.toJson());
        for (const auto &speaker : speakerDevices)
            root["speakerDevices"].append(speaker.toJson());
        return root;
    }

    void print(){
        std::cout << "cameraDevices: " << std::endl;
        for (auto &cam : cameraDevices)
            cam.print();
        std::cout << "displayDevices: " << std::endl;
        for (auto &disp : displayDevices)
            disp.print();
        std::cout << "micDevices: " << std::endl;
        for (auto &mic : micDevices)
            mic.print();
        std::cout << "speakerDevices: " << std::endl;
        for (auto &speaker : speakerDevices)
            speaker.print();
    }
};

#endif //DEVICE_PROFILE_H