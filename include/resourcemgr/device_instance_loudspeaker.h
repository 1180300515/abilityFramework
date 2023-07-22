#ifndef _DEVICE_INSTANCE_LOUDSPEAKER_H
#define _DEVICE_INSTANCE_LOUDSPEAKER_H

#include <map>

#include "json/json.h"

#include "device_instance_info.h"

// loudspeaker part
struct LoudspeakerProperties
{
    int bitWidth;
    std::string interface;
    // get info from the device
    int channelNumber;
    std::string hardwareName; // as the tag to find the matching hardware
    std::string sampleRates;
    int volume; // 音量
    bool mute;  // 是否静音
    std::string description;
};
struct LoudspeakerSpec
{
    std::string version;
    std::string hostname;
    std::string kind;
    std::string hardwareidentifier; // a hardware indentify in order to combine the hardware info with the instance,camera use "card",audio use "name",dispaly use "screen"

    std::vector<Acapability> capability1;
    std::vector<Acapability> capability2;
    LoudspeakerProperties properties;
    std::map<std::string, std::string> customprops;
};
class LoudspeakerInstance : public DeviceInstanceInfo
{
public:
    LoudspeakerSpec spec;

    std::string GetHardwareIdentifier();
    bool UpdateHardwareInfo(const Json::Value &info);
    std::string Marshal();
    bool FromJson(const Json::Value &jnode);
    bool UnMarshal(const std::string &data);
    bool updateInstance(const Json::Value &jnode);
    std::string getInstanceVersion();
};

#endif // _DEVICE_INSTANCE_LOUDSPEAKER_H