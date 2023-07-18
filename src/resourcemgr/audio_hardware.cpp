#include "audio_hardware.h"

#include "iostream"

Json::Value AudioDevice::toJson() const
{

    Json::Value root;
    root["name"] = name;
    root["description"] = description;
    root["volume"] = volume;
    root["mute"] = mute;
    root["sampleRate"] = sampleRate;
    root["channels"] = channels;
    return root;
}

std::string AudioDevice::ToKeyValue(const std::string &hostname) const
{
    std::stringstream ss;
    ss << "hostname:" << hostname << ",";
    ss << "name:" << name << ",";
    ss << "description:" << description << ",";
    ss << "volume:" << volume << ",";
    ss << "mute:" << (mute ? "true" : "false") << ",";
    ss << "sampleRate:" << sampleRate << ",";
    ss << "channels:" << static_cast<int>(channels);
    return ss.str();
}

AudioDevice AudioDevice::fromJson(const Json::Value &root)
{

    return AudioDevice(
        root["name"].asString(),
        root["description"].asString(),
        root["volume"].asUInt(),
        root["mute"].asBool(),
        root["sampleRate"].asUInt(),
        root["channels"].asUInt());
}

void AudioDevice::print()
{
    std::cout << "name: " << name << std::endl;
    std::cout << "description: " << description << std::endl;
    std::cout << "volume: " << volume << std::endl;
    std::cout << "mute: " << mute << std::endl;
    std::cout << "sampleRate: " << sampleRate << std::endl;
    std::cout << "channels: " << static_cast<int>(channels) << std::endl;
}
