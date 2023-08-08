#ifndef RESOURCEMGR_HARDWARE_DEVICE_PROFILE_H_
#define RESOURCEMGR_HARDWARE_DEVICE_PROFILE_H_

#include <iostream>
#include <vector>

#include "resourcemgr/hardware_audio.h"
#include "resourcemgr/hardware_camera.h"
#include "resourcemgr/hardware_display.h"

class DeviceProfile {
 public:
  std::vector<CameraHardware> cameraDevices;
  std::vector<DisplayHardware> displayDevices;
  std::vector<AudioHardware> micDevices;
  std::vector<AudioHardware> speakerDevices;

  Json::Value toJson() const;
  void FromJson2Profile(const Json::Value &root);
  void print();
};

#endif  // RESOURCEMGR_HARDWARE_DEVICE_PROFILE_H_
