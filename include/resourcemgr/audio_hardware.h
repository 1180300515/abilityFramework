#ifndef AUDIO_HARDWARE_H
#define AUDIO_HARDWARE_H

#include <string>
#include <unistd.h>

#include "json/json.h"

class AudioDevice
{
public:
    std::string name;
    std::string description;
    uint32_t volume;
    bool mute;
    uint32_t sampleRate;
    uint8_t channels;

    Json::Value toJson() const ;

    AudioDevice(std::string name, std::string description, uint32_t volume, bool mute, uint32_t sampleRate, uint8_t channels)
        : name(name), description(description), volume(volume), mute(mute), sampleRate(sampleRate), channels(channels) {}

    static AudioDevice fromJson(const Json::Value& root);

    void print();
};

#endif //AUDIO_HARDWARE_H