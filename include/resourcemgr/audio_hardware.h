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

    Json::Value toJson() const {
        Json::Value root;
        root["name"] = name;
        root["description"] = description;
        root["volume"] = volume;
        root["mute"] = mute;
        root["sampleRate"] = sampleRate;
        root["channels"] = channels;
        return root;
    }

    AudioDevice(std::string name, std::string description, uint32_t volume, bool mute, uint32_t sampleRate, uint8_t channels)
        : name(name), description(description), volume(volume), mute(mute), sampleRate(sampleRate), channels(channels) {}

    static AudioDevice fromJson(const Json::Value& root) {
        return AudioDevice(
            root["name"].asString(),
            root["description"].asString(),
            root["volume"].asUInt(),
            root["mute"].asBool(),
            root["sampleRate"].asUInt(),
            root["channels"].asUInt()
        );
    }

    void print(){
        std::cout << "name: " << name << std::endl;
        std::cout << "description: " << description << std::endl;
        std::cout << "volume: " << volume << std::endl;
        std::cout << "mute: " << mute << std::endl;
        std::cout << "sampleRate: " << sampleRate << std::endl;
        std::cout << "channels: " << static_cast<int>(channels) << std::endl;
    }
};

#endif //AUDIO_HARDWARE_H