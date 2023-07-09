#ifndef HARDWARE_SCAN_H
#define HARDWARE_SCAN_H

#include "device_profile.h"

class HardwareScan
{
private:
    DeviceProfile profile;
    void insertCameraInfo(Json::Value &jnode);
    void insertMicInfo(Json::Value &jnode);
    void insertloudspeakerInfo(Json::Value &jnode);

public:
    void LocalHardwareScan();
    void AutoGenerateCR();
    DeviceProfile getHardwareInfo();
};

#endif // HARDWARE_SCAN_H