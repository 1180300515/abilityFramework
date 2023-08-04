#ifndef _HARDWARE_CAMERA_H
#define _HARDWARE_CAMERA_H

#include <string>
#include <vector>

#include "json/json.h"

class CameraHardware
{
public:
    std::string identifier;
    std::string device_path;
    std::string driver;
    std::string card;
    std::string bus_info;
    std::vector<std::string> formats;
    //...更多属性和方法...

    CameraHardware &operator=(const CameraHardware &other);
    Json::Value toJson() const;
    void fromJson(const Json::Value &root);
    std::string ToKeyValue(std::string hostname) const;
    void print();
};

#endif // _HARDWARE_CAMERA_H