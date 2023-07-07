#include "discoverymanager/localhw_interface.h"
#include "discoverymanager/audio_interface.h"
#include "discoverymanager/disp_interface.h"

#include <httplib.h>

extern DeviceProfile deviceProfile;

DeviceProfile getLocalHWInfo(){
    DeviceProfile dp;
    // std::cout << "start to get local hw info" << std::endl;
    dp.cameraDevices = getCameraInfo();
    dp.displayDevices = getDisplayInfo();
    auto audioDevices = getAudioInfo();
    dp.micDevices = audioDevices.first;
    dp.speakerDevices = audioDevices.second;
    // dp.print();
    // std::cout << "finish to get local hw info" << std::endl;
    return dp;
}

void LocalhwPrint(){
    auto a =  getLocalHWInfo();
    for (const auto &device : a.micDevices)
    {
        std::cout << "  " << device.description << std::endl;
        std::cout << "  " << device.name << std::endl;
        std::cout << "  " << device.volume << std::endl;
        std::cout << "  " << device.mute << std::endl;
        std::cout << "  " << device.sampleRate << std::endl;
        std::cout << "  " << device.channels << std::endl;
    }

    for (const auto &device : a.speakerDevices)
    {
        std::cout << "  " << device.description << std::endl;
        std::cout << "  " << device.name << std::endl;
        std::cout << "  " << device.volume << std::endl;
        std::cout << "  " << device.mute << std::endl;
        std::cout << "  " << device.sampleRate << std::endl;
        std::cout << "  " << device.channels << std::endl;
    }

    for (const auto &device : a.cameraDevices)
    {
        std::cout << "Device path: " << device.device_path << std::endl
                  << "Driver: " << device.driver << std::endl
                  << "Card: " << device.card << std::endl
                  << "Bus info: " << device.bus_info << std::endl
                  << "Supported formats: ";
        for (const auto &format : device.formats)
        {
            std::cout << format << " ";
        }
        std::cout << std::endl
                  << std::endl;
    }
    

    for (const auto &device : a.displayDevices)
    {
        std::cout << "  " << device.screen << std::endl;
        std::cout << "  " << device.x << std::endl;
        std::cout << "  " << device.y << std::endl;
        std::cout << "  " << device.width << std::endl;
        std::cout << "  " << device.height << std::endl;
        std::cout << "  " << device.mmWidth << std::endl;
        std::cout << "  " << device.mmHeight << std::endl;
    }
}

DeviceProfile getDeviceProfileFromHost(const std::string& ip) {
    httplib::Client ccc(ip, 8080); 
    DeviceProfile dp;
    auto res = ccc.Get("/api/Devices");

    if (res && res->status == 200) {
        // 响应成功，解析 JSON
        Json::CharReaderBuilder rbuilder;
        std::unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());
        Json::Value root;
        std::string errs;
        bool parsingSuccessful = reader->parse(res->body.c_str(), res->body.c_str() + res->body.size(), &root, &errs);
        if (!parsingSuccessful) {
            throw std::runtime_error("Failed to parse JSON: " + errs);
        }
        
        // 反序列化 DeviceProfile
        dp = fromJson2Profile(root);   
    } 
    return dp;
}

DeviceProfile fromJson2Profile(const Json::Value &root)
{
    DeviceProfile profile;
    for (const auto &cam : root["cameraDevices"])
        profile.cameraDevices.push_back(CameraHardware::fromJson(cam));
    for (const auto &disp : root["displayDevices"])
        profile.displayDevices.push_back(DisplayInfo::fromJson(disp));
    for (const auto &mic : root["micDevices"])
        profile.micDevices.push_back(AudioDevice::fromJson(mic));
    for (const auto &speaker : root["speakerDevices"])
        profile.speakerDevices.push_back(AudioDevice::fromJson(speaker));
    return profile;
}
