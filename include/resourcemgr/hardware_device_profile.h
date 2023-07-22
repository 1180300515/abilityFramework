#ifndef _HARDWARE_DEVICE_PROFILE_H
#define _HARDWARE_DEVICE_PROFILE_H

#include <iostream>

#include "hardware_audio.h"
#include "hardware_camera.h"
#include "hardware_display.h"

class DeviceProfile
{
public:
    std::vector<CameraHardware> cameraDevices;
    std::vector<DisplayHardware> displayDevices;
    std::vector<AudioHardware> micDevices;
    std::vector<AudioHardware> speakerDevices;

    Json::Value toJson() const;
    void FromJson2Profile(const Json::Value &root);
    void print();
};

#endif // _HARDWARE_DEVICE_PROFILE_H