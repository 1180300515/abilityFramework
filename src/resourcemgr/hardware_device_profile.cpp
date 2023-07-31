#include "hardware_device_profile.h"

Json::Value DeviceProfile::toJson() const
{

    Json::Value root;
    for (const auto &cam : cameraDevices)
        root["cameraDevices"].append(cam.toJson());
    for (const auto &disp : displayDevices)
        root["displayDevices"].append(disp.toJson());
    for (const auto &mic : micDevices)
        root["micDevices"].append(mic.toJson());
    for (const auto &speaker : speakerDevices)
        root["speakerDevices"].append(speaker.toJson());
    return root;
}

void DeviceProfile::print()
{
    std::cout << "cameraDevices: " << std::endl;
    for (auto &cam : cameraDevices)
        cam.print();
    std::cout << "displayDevices: " << std::endl;
    for (auto &disp : displayDevices)
        disp.print();
    std::cout << "micDevices: " << std::endl;
    for (auto &mic : micDevices)
        mic.print();
    std::cout << "speakerDevices: " << std::endl;
    for (auto &speaker : speakerDevices)
        speaker.print();
}

void DeviceProfile::FromJson2Profile(const Json::Value &root)
{
    for (const auto &cam : root["cameraDevices"])
    {
        CameraHardware hw;
        hw.fromJson(cam);
        cameraDevices.emplace_back(hw);
    }
    for (const auto &disp : root["displayDevices"])
    {
        DisplayHardware hw;
        hw.fromJson(disp);
        displayDevices.emplace_back(hw);
    }
    for (const auto &mic : root["micDevices"])
    {
        AudioHardware hw;
        hw.fromJson(mic);
        micDevices.emplace_back(hw);
    }
    for (const auto &speaker : root["speakerDevices"])
    {
        AudioHardware hw;
        hw.fromJson(speaker);
        speakerDevices.emplace_back(hw);
    }
}
