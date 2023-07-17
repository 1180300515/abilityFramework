#include "camera_hardware.h"

#include <iostream>

CameraHardware &CameraHardware::operator=(const CameraHardware &other)
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

Json::Value CameraHardware::toJson() const
{

    Json::Value root;
    root["device_path"] = device_path;
    root["driver"] = driver;
    root["card"] = card;
    root["bus_info"] = bus_info;
    for (const auto &format : formats)
        root["formats"].append(format);
    return root;
}

CameraHardware CameraHardware::fromJson(const Json::Value &root)
{

    CameraHardware cam;
    cam.device_path = root["device_path"].asString();
    cam.driver = root["driver"].asString();
    cam.card = root["card"].asString();
    cam.bus_info = root["bus_info"].asString();
    for (const auto &format : root["formats"])
        cam.formats.push_back(format.asString());
    return cam;
}

void CameraHardware::print()
{
    std::cout << "device_path: " << device_path << std::endl;
    std::cout << "driver: " << driver << std::endl;
    std::cout << "card: " << card << std::endl;
    std::cout << "bus_info: " << bus_info << std::endl;
    std::cout << "formats: ";
    for (const auto &format : formats)
        std::cout << format << " ";
    std::cout << std::endl
              << std::endl;
}
