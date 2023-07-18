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

    Json::Value toJson() const;
    void FromJson2Profile(const Json::Value &root);
    void print();
};

#endif // DEVICE_PROFILE_H