#ifndef _LOCALHW_INTERFACE_H
#define _LOCALHW_INTERFACE_H

#include "audio_interface.h"
#include "cam_interface.h"
#include "disp_interface.h"


class DeviceProfile{
public:
    std::vector<CameraHardware> cameraDevices;
    std::vector<DisplayInfo> displayDevices;
    std::vector<AudioDevice> micDevices;
    std::vector<AudioDevice> speakerDevices;

    Json::Value toJson() const {
        Json::Value root;
        for (const auto& cam : cameraDevices)
            root["cameraDevices"].append(cam.toJson());
        for (const auto& disp : displayDevices)
            root["displayDevices"].append(disp.toJson());
        for (const auto& mic : micDevices)
            root["micDevices"].append(mic.toJson());
        for (const auto& speaker : speakerDevices)
            root["speakerDevices"].append(speaker.toJson());
        return root;
    }

    DeviceProfile fromJson(const Json::Value& root) {
        DeviceProfile profile;
        for (const auto& cam : root["cameraDevices"])
            profile.cameraDevices.push_back(CameraHardware::fromJson(cam));
        for (const auto& disp : root["displayDevices"])
            profile.displayDevices.push_back(DisplayInfo::fromJson(disp));
        for (const auto& mic : root["micDevices"])
            profile.micDevices.push_back(AudioDevice::fromJson(mic));
        for (const auto& speaker : root["speakerDevices"])
            profile.speakerDevices.push_back(AudioDevice::fromJson(speaker));
        return profile;
    }
};

DeviceProfile getLocalHWInfo();
void LocalhwPrint();
extern DeviceProfile deviceProfile;


#endif // _LOCALHW_INTERFACE_H_