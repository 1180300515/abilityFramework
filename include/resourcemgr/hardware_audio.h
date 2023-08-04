#ifndef _HARDWARE_AUDIO_H
#define _HARDWARE_AUDIO_H

#include <string>
#include <unistd.h>

#include "json/json.h"

class AudioHardware
{
public:
    std::string name;
    std::string description;
    uint32_t volume;
    bool mute;
    uint32_t sampleRate;
    uint8_t channels;
    int cardid;
    int deviceid;

    AudioHardware() = default;
    AudioHardware(std::string name, std::string description, uint32_t volume, bool mute, uint32_t sampleRate, uint8_t channels)
        : name(name), description(description), volume(volume), mute(mute), sampleRate(sampleRate), channels(channels) {}
    void fromJson(const Json::Value &root);
    Json::Value toJson() const;
    std::string ToKeyValue(const std::string &hostname) const;
    void print();
};

#endif // _HARDWARE_AUDIO_H