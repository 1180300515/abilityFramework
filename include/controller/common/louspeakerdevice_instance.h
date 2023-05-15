#ifndef LOUDSPEAKERDEVICE_INSTANCE_H
#define LOUDSPEAKERDEVICE_INSTANCE_H

#include "controller/common/common_struct.h"
#include "controller/common/Instance.h"

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
class LoudspeakerInstance : public Instance
{
public:
    LoudspeakerSpec spec;
    std::string Marshal();
    bool UnMarshal(std::string source);
    bool updateInstance(std::string data);
    std::string getInstanceVersion();
};
#endif