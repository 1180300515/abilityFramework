#ifndef LOUDSPEAKERDEVICE_INSTANCE_H
#define LOUDSPEAKERDEVICE_INSTANCE_H

#include "controller/common/common_struct.h"
#include "controller/common/Instance.h"

// loudspeaker part
struct LoudspeakerProperties
{
    std::string sampleRates;
    int channelNumber;
    int bitWidth;
    std::string interface;
};
struct LoudspeakerSpec
{
    std::string version;
    std::string hostname;
    std::string kind;
    std::vector<Acapability> capability1;
    std::vector<Acapability> capability2;
    LoudspeakerProperties properties;
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