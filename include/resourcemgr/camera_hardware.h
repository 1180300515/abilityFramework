#ifndef CAMERA_HARDWARE_H
#define CAMERA_HARDWARE_H

#include <string>
#include <vector>

#include "json/json.h"

class CameraHardware
{
public:
    std::string device_path;
    std::string driver;
    std::string card;
    std::string bus_info;
    std::vector<std::string> formats;
    //...更多属性和方法...

    CameraHardware &operator=(const CameraHardware &other)
    {
        if (this != &other)
        {
            device_path = other.device_path;
            driver = other.driver;
            card = other.card;
            bus_info = other.bus_info;
            formats = other.formats;
            //...复制其它成员...
        }
        return *this;
    }

    Json::Value toJson() const {
        Json::Value root;
        root["device_path"] = device_path;
        root["driver"] = driver;
        root["card"] = card;
        root["bus_info"] = bus_info;
        for (const auto& format : formats)
            root["formats"].append(format);
        return root;
    }

    static CameraHardware fromJson(const Json::Value& root) {
        CameraHardware cam;
        cam.device_path = root["device_path"].asString();
        cam.driver = root["driver"].asString();
        cam.card = root["card"].asString();
        cam.bus_info = root["bus_info"].asString();
        for (const auto& format : root["formats"])
            cam.formats.push_back(format.asString());
        return cam;
    }

    void print(){
        std::cout << "device_path: " << device_path << std::endl;
        std::cout << "driver: " << driver << std::endl;
        std::cout << "card: " << card << std::endl;
        std::cout << "bus_info: " << bus_info << std::endl;
        std::cout << "formats: ";
        for (const auto& format : formats)
            std::cout << format << " ";
        std::cout << std::endl << std::endl;
    }
};

#endif //CAMERA_HARDWARE_H