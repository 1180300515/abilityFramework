#ifndef _AUDIO_INTERFACE_H_
#define _AUDIO_INTERFACE_H_

#include <iostream>
#include <vector>
#include <cstring>
#include <pulse/pulseaudio.h>

class AudioDevice
{
public:
    std::string name;
    std::string description;
    uint32_t volume;
    bool mute;
    uint32_t sampleRate;
    uint8_t channels;

    AudioDevice(std::string name, std::string description, uint32_t volume, bool mute, uint32_t sampleRate, uint8_t channels)
        : name(name), description(description), volume(volume), mute(mute), sampleRate(sampleRate), channels(channels) {}
};

std::vector<AudioDevice> inputDevices;
std::vector<AudioDevice> outputDevices;

static void sink_info_callback(pa_context *c, const pa_sink_info *info, int eol, void *userdata);
static void source_info_callback(pa_context *c, const pa_source_info *info, int eol, void *userdata);
static void context_state_callback(pa_context *c, void *userdata);
std::pair<std::vector<AudioDevice>, std::vector<AudioDevice>> getAudioInfo();





#endif // _AUDIO_INTERFACE_H_