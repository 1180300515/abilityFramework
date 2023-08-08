#ifndef RESOURCEMGR_DEVICE_INSTANCE_MICROPHONE_H_
#define RESOURCEMGR_DEVICE_INSTANCE_MICROPHONE_H_

#include <json/json.h>

#include <map>
#include <vector>
#include <string>

#include "resourcemgr/device_instance_info.h"

struct MicrophoneProperties {
  // get from the yaml
  std::string sampleRates;
  std::string interface;
  // get from the hardware
  std::string hardwareName;  // as the tag to find the matching hardware
  std::string description;
  std::vector<uint32_t> sampleRate;
  std::vector<uint8_t> channels;
  std::vector<std::string> format;
  std::string cardID;
  std::string deviceID;
};
struct MicrophoneSpec {
  std::string version;
  std::string hostname;
  std::string kind;
  // a hardware indentify in order to combine the  hardware info with the
  // instance,camera use "card",audio use "name",dispaly use "screen"
  std::string hardwareidentifier;

  std::vector<Acapability> capability1;
  std::vector<Acapability> capability2;
  MicrophoneProperties properties;
  // std::map<std::string, std::string> customprops;
};
class MicrophoneInstance : public DeviceInstanceInfo {
 public:
  MicrophoneSpec spec;

  std::string GetHardwareIdentifier() const;
  bool UpdateHardwareInfo(const Json::Value &info);
  void EraseHardwareInfo();
  std::string Marshal() const;
  Json::Value ToJson() const;
  bool FromJson(const Json::Value &jnode);
  bool UnMarshal(const std::string &data);
  bool updateInstance(const Json::Value &jnode);
  std::string getInstanceVersion() const;
};

#endif  // RESOURCEMGR_DEVICE_INSTANCE_MICROPHONE_H_
