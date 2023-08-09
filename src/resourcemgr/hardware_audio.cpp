#include "resourcemgr/hardware_audio.h"

#include <iostream>

Json::Value AudioHardware::toJson() const {
  Json::Value root;
  root["name"] = name;
  root["description"] = description;
  for (const auto &iter : sampleRate) {
    root["sampleRate"].append(iter);
  }
  for (const auto &iter : channels) {
    root["channels"].append(iter);
  }
  for (const auto &iter : format) {
    root["format"].append(iter);
  }
  root["cardID"] = cardID;
  root["deviceID"] = deviceID;
  return root;
}

std::string AudioHardware::ToKeyValue(const std::string &hostname) const {
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

void AudioHardware::fromJson(const Json::Value &root) {
  name = root["name"].asString();
  description = root["description"].asString();
  for (const auto &iter : root["sampleRate"]) {
    sampleRate.emplace_back(iter.asUInt());
  }
  for (const auto &iter : root["channels"]) {
    channels.emplace_back(iter.asUInt());
  }
  for (const auto &iter : root["format"]) {
    format.emplace_back(iter.asString());
  }
  cardID = root["cardID"].asString();
  deviceID = root["deviceID"].asString();
}

void AudioHardware::print() const {
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

const unsigned int rates[13] = {
    5512,  8000,  11025, 16000, 22050,  32000,  44100,
    48000, 64000, 88200, 96000, 176400, 192000,
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
