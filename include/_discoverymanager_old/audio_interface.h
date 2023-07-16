#ifndef _AUDIO_INTERFACE_H_
#define _AUDIO_INTERFACE_H_

#include <iostream>
#include <vector>
#include <unistd.h>
#include <cstring>
#include <pulse/pulseaudio.h>
#include <json/json.h>

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

static void sink_info_callback(pa_context *c, const pa_sink_info *info, int eol, void *userdata);
static void source_info_callback(pa_context *c, const pa_source_info *info, int eol, void *userdata);
static void context_state_callback(pa_context *c, void *userdata);
std::pair<std::vector<AudioDevice>, std::vector<AudioDevice>> getAudioInfo();





#endif // _AUDIO_INTERFACE_H_