#ifndef LOUDSPEAKERDEVICE_INSTANCE_H
#define LOUDSPEAKERDEVICE_INSTANCE_H

#include <map>

#include "json/json.h"

#include "device_instance_info.h"

// loudspeaker part
struct LoudspeakerProperties
{
    
    int channelNumber;
    int bitWidth;
    std::string interface;
    // get info from the device
    std::string hardwareName;
    std::string sampleRates;
    int volume; // 音量
    bool mute; // 是否静音
    std::string description;
};
struct LoudspeakerSpec
{
    std::string version;
    std::string hostname;
    std::string kind;
    std::vector<Acapability> capability1;
    std::vector<Acapability> capability2;
    LoudspeakerProperties properties;
    std::map<std::string,std::string> customprops;
};
class LoudspeakerInstance : public DeviceInstanceInfo
{
public:
    LoudspeakerSpec spec;
    std::string Marshal();
    bool UnMarshal(const Json::Value &jnode);
    bool updateInstance(const Json::Value &jnode);
    std::string getInstanceVersion();
};
#endif