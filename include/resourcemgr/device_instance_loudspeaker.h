#ifndef _DEVICE_INSTANCE_LOUDSPEAKER_H
#define _DEVICE_INSTANCE_LOUDSPEAKER_H

#include <map>

#include "json/json.h"

#include "device_instance_info.h"

// loudspeaker part
struct LoudspeakerProperties
{
    //get from the yaml
    std::string sampleRates;
    int channelNumber;
    int bitWidth;
    std::string interface;
    // get from the hardware
    std::string hardwareName; // as the tag to find the matching hardware
    std::string description;
    std::vector<uint32_t> sampleRate;
    std::vector<uint8_t> channels;
    std::vector<std::string> format;
    std::string cardID;
    std::string deviceID;
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
    //std::map<std::string, std::string> customprops;
};
class LoudspeakerInstance : public DeviceInstanceInfo
{
public:
    LoudspeakerSpec spec;

    std::string GetHardwareIdentifier() const;
    bool UpdateHardwareInfo(const Json::Value &info);
    void EraseHardwareInfo();
    std::string Marshal() const;
    bool FromJson(const Json::Value &jnode);
    Json::Value ToJson() const;
    bool UnMarshal(const std::string &data);
    bool updateInstance(const Json::Value &jnode);
    std::string getInstanceVersion() const;
};

#endif // _DEVICE_INSTANCE_LOUDSPEAKER_H