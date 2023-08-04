#ifndef _HARDWARE_AUDIO_H
#define _HARDWARE_AUDIO_H

#include <string>
#include <unistd.h>
#include <alsa/asoundlib.h>

#include "json/json.h"

class AudioHardware
{
public:
    std::string name;
    std::string description;
    std::vector<uint32_t> sampleRate;
    std::vector<uint8_t> channels;
    std::vector<std::string> format;
    std::string cardID;
    std::string deviceID;

    AudioHardware() = default;
    AudioHardware(const std::string &name, const std::string &description,
                  const std::vector<uint32_t> &sampleRate, const std::vector<uint8_t> &channels,
                  const std::vector<std::string> &format, const std::string &cardID, const std::string &deviceID)
        : name(name), description(description), sampleRate(sampleRate), channels(channels), format(format), cardID(cardID), deviceID(deviceID) {}
        
    void fromJson(const Json::Value &root);
    Json::Value toJson() const;
    std::string ToKeyValue(const std::string &hostname) const;
    void print();
};

//global var define
extern const unsigned int rates[13];
extern const snd_pcm_format_t formats[38];

#endif // _HARDWARE_AUDIO_H