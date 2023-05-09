#ifndef _LOCALHW_INTERFACE_H
#define _LOCALHW_INTERFACE_H

#include "audio_interface.h"
#include "cam_interface.h"
#include "disp_interface.h"

class DeviceProfile{
public:
    std::vector<CameraDevice> cameraDevices;
    std::vector<DisplayInfo> displayDevices;
    std::vector<AudioDevice> micDevices;
    std::vector<AudioDevice> speakerDevices;
};

DeviceProfile getLocalHWInfo();


#endif // _LOCALHW_INTERFACE_H_