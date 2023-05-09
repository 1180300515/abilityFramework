#include "discoverymanager/localhw_interface.h"
#include "discoverymanager/audio_interface.h"
#include "discoverymanager/disp_interface.h"

DeviceProfile getLocalHWInfo(){
    DeviceProfile deviceProfile;
    deviceProfile.cameraDevices = getCameraInfo();
    deviceProfile.displayDevices = getDisplayInfo();
    auto audioDevices = getAudioInfo();
    deviceProfile.inputDevices = audioDevices.first;
    deviceProfile.outputDevices = audioDevices.second;
    return deviceProfile;
}