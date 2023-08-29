/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: hardware_audio.h
 * Description: hardware_audio.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef RESOURCEMGR_HARDWARE_AUDIO_H_
#define RESOURCEMGR_HARDWARE_AUDIO_H_

#include <alsa/asoundlib.h>
#include <json/json.h>
#include <unistd.h>

#include <string>
#include <vector>

class AudioHardware {
   public:
    std::string name;
    std::string description;
    std::vector<uint32_t> sampleRate;
    std::vector<uint8_t> channels;
    std::vector<std::string> format;
    std::string cardID;
    std::string deviceID;

    AudioHardware() = default;
    AudioHardware(const std::string &name, const std::string &description, const std::vector<uint32_t> &sampleRate,
                  const std::vector<uint8_t> &channels, const std::vector<std::string> &format, const std::string &cardID,
                  const std::string &deviceID)
        : name(name), description(description), sampleRate(sampleRate), channels(channels), format(format), cardID(cardID), deviceID(deviceID)
    {
    }

    void fromJson(const Json::Value &root);
    Json::Value toJson() const;
    std::string ToKeyValue(const std::string &hostname) const;
    void print() const;
};

// global var define
extern const unsigned int rates[13];
extern const snd_pcm_format_t formats[38];

#endif  // RESOURCEMGR_HARDWARE_AUDIO_H_
