#include "discoverymanager/devpool_interface.h"

// 在字符串键值对中查找指定键的值
std::string findValueByKey(const std::string& keyValueString, const std::string& key) {
    std::unordered_map<std::string, std::string> keyValueMap;

    std::stringstream ss(keyValueString);
    std::string pair;
    while (std::getline(ss, pair, ',')) {
        std::stringstream pair_ss(pair);
        std::string key, value;
        std::getline(pair_ss, key, ':');
        std::getline(pair_ss, value, ':');
        std::cout << "key: " << key << ", value: " << value << std::endl;
        keyValueMap[key] = value;
    }

    if (keyValueMap.find(key) != keyValueMap.end()) {
        return keyValueMap[key];
    } else {
        return "Key not found";
    }
}

std::string cameraHardwareToKeyValue(std::string hostname, const CameraHardware& cameraHardware) {
    std::stringstream ss;
    ss << "hostname:" << hostname << ",";
    ss << "device_path:" << cameraHardware.device_path << ",";
    ss << "driver:" << cameraHardware.driver << ",";
    ss << "card:" << cameraHardware.card << ",";
    ss << "bus_info:" << cameraHardware.bus_info << ",";
    ss << "formats:[";
    for (const auto& format : cameraHardware.formats) {
        ss << format << " ";
    }
    ss << "]";
    return ss.str();
}

std::string audioDeviceToKeyValue(std::string hostname, const AudioDevice& audioDevice) {
    std::stringstream ss;
    ss << "hostname:" << hostname << ",";
    ss << "name:" << audioDevice.name << ",";
    ss << "description:" << audioDevice.description << ",";
    ss << "volume:" << audioDevice.volume << ",";
    ss << "mute:" << (audioDevice.mute ? "true" : "false") << ",";
    ss << "sampleRate:" << audioDevice.sampleRate << ",";
    ss << "channels:" << static_cast<int>(audioDevice.channels);
    return ss.str();
}

std::string displayInfoToKeyValue(std::string hostname, const DisplayInfo& displayInfo) {
    std::stringstream ss;
    ss << "hostname:" << hostname << ",";
    ss << "screen:" << displayInfo.screen << ",";
    ss << "x:" << displayInfo.x << ",";
    ss << "y:" << displayInfo.y << ",";
    ss << "width:" << displayInfo.width << ",";
    ss << "height:" << displayInfo.height << ",";
    ss << "mmWidth:" << displayInfo.mmWidth << ",";
    ss << "mmHeight:" << displayInfo.mmHeight;
    return ss.str();
}