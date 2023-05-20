#ifndef _LOCALHW_INTERFACE_H
#define _LOCALHW_INTERFACE_H

#include "audio_interface.h"
#include "cam_interface.h"
#include "disp_interface.h"

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
};

DeviceProfile getDeviceProfileFromHost(const std::string& ip);
DeviceProfile fromJson2Profile(const Json::Value &root);

DeviceProfile getLocalHWInfo();
void LocalhwPrint();
extern DeviceProfile deviceProfile;

#endif // _LOCALHW_INTERFACE_H_