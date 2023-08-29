/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: get_audio_hardware.cpp
 * Description: get_audio_hardware.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "include/get_audio_hardware.h"

#include <alsa/asoundlib.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof *(a))

const unsigned int rates[13] = {
    5512, 8000, 11025, 16000, 22050, 32000, 44100, 48000, 64000, 88200, 96000, 176400, 192000,
};

const snd_pcm_format_t formats[38] = {
    SND_PCM_FORMAT_S8,
    SND_PCM_FORMAT_U8,
    SND_PCM_FORMAT_S16_LE,
    SND_PCM_FORMAT_S16_BE,
    SND_PCM_FORMAT_U16_LE,
    SND_PCM_FORMAT_U16_BE,
    SND_PCM_FORMAT_S24_LE,
    SND_PCM_FORMAT_S24_BE,
    SND_PCM_FORMAT_U24_LE,
    SND_PCM_FORMAT_U24_BE,
    SND_PCM_FORMAT_S32_LE,
    SND_PCM_FORMAT_S32_BE,
    SND_PCM_FORMAT_U32_LE,
    SND_PCM_FORMAT_U32_BE,
    SND_PCM_FORMAT_FLOAT_LE,
    SND_PCM_FORMAT_FLOAT_BE,
    SND_PCM_FORMAT_FLOAT64_LE,
    SND_PCM_FORMAT_FLOAT64_BE,
    SND_PCM_FORMAT_IEC958_SUBFRAME_LE,
    SND_PCM_FORMAT_IEC958_SUBFRAME_BE,
    SND_PCM_FORMAT_MU_LAW,
    SND_PCM_FORMAT_A_LAW,
    SND_PCM_FORMAT_IMA_ADPCM,
    SND_PCM_FORMAT_MPEG,
    SND_PCM_FORMAT_GSM,
    SND_PCM_FORMAT_SPECIAL,
    SND_PCM_FORMAT_S24_3LE,
    SND_PCM_FORMAT_S24_3BE,
    SND_PCM_FORMAT_U24_3LE,
    SND_PCM_FORMAT_U24_3BE,
    SND_PCM_FORMAT_S20_3LE,
    SND_PCM_FORMAT_S20_3BE,
    SND_PCM_FORMAT_U20_3LE,
    SND_PCM_FORMAT_U20_3BE,
    SND_PCM_FORMAT_S18_3LE,
    SND_PCM_FORMAT_S18_3BE,
    SND_PCM_FORMAT_U18_3LE,
    SND_PCM_FORMAT_U18_3BE,
};

void AudioHardwareForTest::print() const
{
    std::cout << "name: " << name << std::endl;
    std::cout << "description: " << description << std::endl;
    std::cout << "sampleRate: " << std::endl;
    for (auto &i : sampleRate) {
        std::cout << "  " << i << std::endl;
    }
    std::cout << "channels: " << std::endl;
    for (auto &i : channels) {
        std::cout << "  " << std::to_string(i) << std::endl;
    }
    std::cout << "format: " << std::endl;
    for (auto &i : format) {
        std::cout << "  " << i << std::endl;
    }
    std::cout << "cardID: " << cardID << std::endl;
    std::cout << "deviceID: " << deviceID << std::endl;
    std::cout << std::endl << std::endl;
}
std::string AudioHardwareForTest::ToKeyValue(std::string hostname) const
{
    std::stringstream ss;
    ss << "hostname:" << hostname << ",";
    ss << "name:" << name << ",";
    ss << "description:" << description << ",";
    ss << "sampleRate:[";
    for (const auto &iter : sampleRate) {
        ss << iter << " ";
    }
    ss << "],";
    ss << "channels:[";
    for (const auto &iter : channels) {
        ss << std::to_string(iter) << " ";
    }
    ss << "],";
    ss << "format:[";
    for (const auto &iter : format) {
        ss << iter << " ";
    }
    ss << "],";
    ss << "cardID:" << cardID << ",";
    ss << "deviceID:" << deviceID;
    return ss.str();
}

int retriveOtherAudioParameters(snd_pcm_stream_t stream_type, AudioHardwareForTest *audioDevice)
{
    snd_pcm_t *pcm;
    snd_pcm_hw_params_t *hw_params;
    unsigned int i;
    unsigned int min, max;
    int any_rate;
    int err;
    std::string deviceName = "hw:" + audioDevice->cardID + "," + audioDevice->deviceID;
    const char *device_name = deviceName.c_str();

    err = snd_pcm_open(&pcm, device_name, stream_type, SND_PCM_NONBLOCK);
    if (err < 0) {
        fprintf(stderr, "cannot open device '%s': %s\n", device_name, snd_strerror(err));
        return 1;
    }

    snd_pcm_hw_params_alloca(&hw_params);
    err = snd_pcm_hw_params_any(pcm, hw_params);
    if (err < 0) {
        fprintf(stderr, "cannot get hardware parameters: %s\n", snd_strerror(err));
        snd_pcm_close(pcm);
        return 1;
    }
    for (i = 0; i < ARRAY_SIZE(formats); ++i) {
        if (!snd_pcm_hw_params_test_format(pcm, hw_params, formats[i])) audioDevice->format.push_back(snd_pcm_format_name(formats[i]));
        // printf(" %s", snd_pcm_format_name(formats[i]));
    }

    err = snd_pcm_hw_params_get_channels_min(hw_params, &min);
    if (err < 0) {
        fprintf(stderr, "cannot get minimum channels count: %s\n", snd_strerror(err));
        snd_pcm_close(pcm);
        return 1;
    }
    err = snd_pcm_hw_params_get_channels_max(hw_params, &max);
    if (err < 0) {
        fprintf(stderr, "cannot get maximum channels count: %s\n", snd_strerror(err));
        snd_pcm_close(pcm);
        return 1;
    }
    // printf("Channels:");
    for (i = min; i <= max; ++i) {
        if (!snd_pcm_hw_params_test_channels(pcm, hw_params, i)) {
            audioDevice->channels.push_back(i);
            // std::cout << "*********** CHANNEL: " << i << std::endl;
        }
        // printf(" %u", i);
    }

    err = snd_pcm_hw_params_get_rate_min(hw_params, &min, NULL);
    if (err < 0) {
        fprintf(stderr, "cannot get minimum rate: %s\n", snd_strerror(err));
        snd_pcm_close(pcm);
        return 1;
    }
    err = snd_pcm_hw_params_get_rate_max(hw_params, &max, NULL);
    if (err < 0) {
        fprintf(stderr, "cannot get maximum rate: %s\n", snd_strerror(err));
        snd_pcm_close(pcm);
        return 1;
    }
    // printf("Sample rates:");
    if (min == max) {
        // printf(" %u", min);
        audioDevice->sampleRate.push_back(min);
        // std::cout << "*********** RATE: " << min << std::endl;
    } else if (!snd_pcm_hw_params_test_rate(pcm, hw_params, min + 1, 0)) {
        printf(" %u-%u", min, max);
    } else {
        any_rate = 0;
        for (i = 0; i < ARRAY_SIZE(rates); ++i) {
            if (!snd_pcm_hw_params_test_rate(pcm, hw_params, rates[i], 0)) {
                any_rate = 1;
                audioDevice->sampleRate.push_back(rates[i]);
                // std::cout << "*********** RATE: " << rates[i] << std::endl;
                // printf(" %u", rates[i]);
            }
        }
        if (!any_rate) printf(" (empty)");
    }

    snd_pcm_close(pcm);
    return 0;
}

std::vector<AudioHardwareForTest> getAudioList(const std::string &type)
{
    std::vector<AudioHardwareForTest> audioDevices;
    const char *command;
    snd_pcm_stream_t stream;
    if (type == "record") {
        command = "arecord";
        stream = SND_PCM_STREAM_CAPTURE;
    } else if (type == "playback") {
        command = "aplay";
        stream = SND_PCM_STREAM_PLAYBACK;
    } else {
        std::cout << "Wrong type." << std::endl;
        return audioDevices;
    }

    snd_ctl_t *handle;
    int card, err, dev, idx;
    snd_ctl_card_info_t *info;
    snd_pcm_info_t *pcminfo;
    snd_ctl_card_info_alloca(&info);
    snd_pcm_info_alloca(&pcminfo);

    card = -1;
    if (snd_card_next(&card) < 0 || card < 0) {
        std::cout << "No soundcards found..." << std::endl;
        return audioDevices;
    }
    snd_pcm_stream_name(stream);
    while (card >= 0) {
        char name[32];
        snprintf(name, sizeof(name), "hw:%d", card);
        if ((err = snd_ctl_open(&handle, name, 0)) < 0) {
            std::cout << "control open " << card << " error: " << snd_strerror(err) << std::endl;
            goto next_card;
        }
        if ((err = snd_ctl_card_info(handle, info)) < 0) {
            std::cout << "control hardware info " << card << " error: " << snd_strerror(err) << std::endl;
            snd_ctl_close(handle);
            goto next_card;
        }
        dev = -1;
        while (1) {
            unsigned int count;
            if (snd_ctl_pcm_next_device(handle, &dev) < 0) std::cout << "snd_ctl_pcm_next_device" << std::endl;
            if (dev < 0) break;
            snd_pcm_info_set_device(pcminfo, dev);
            snd_pcm_info_set_subdevice(pcminfo, 0);
            snd_pcm_info_set_stream(pcminfo, stream);
            if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0) {
                if (err != -ENOENT) std::cout << "control digital audio info (" << card << "): " << snd_strerror(err) << std::endl;
                continue;
            }
            std::string name;
            if (type == "record") {
                name = "mic.card-" + std::to_string(card) + ".device-" + std::to_string(dev) + "." + std::string(snd_ctl_card_info_get_name(info));
            } else if (type == "playback") {
                name =
                    "speaker.card-" + std::to_string(card) + ".device-" + std::to_string(dev) + "." + std::string(snd_ctl_card_info_get_name(info));
            }

            AudioHardwareForTest audioDevice(name, snd_pcm_info_get_name(pcminfo), {}, {}, {}, std::to_string(card), std::to_string(dev));
            if (retriveOtherAudioParameters(stream, &audioDevice)) continue;
            audioDevices.push_back(audioDevice);
        }

        snd_ctl_close(handle);
    next_card:
        if (snd_card_next(&card) < 0) {
            std::cout << "snd_card_next" << std::endl;
            break;
        }
    }
    return audioDevices;
}

void print(const std::vector<AudioHardwareForTest> &audio)
{
    for (const auto &iter : audio) {
        iter.print();
    }
}
