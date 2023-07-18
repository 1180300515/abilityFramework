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

    CameraHardware &operator=(const CameraHardware &other);
    Json::Value toJson() const;
    static CameraHardware fromJson(const Json::Value &root);
    std::string ToKeyValue(std::string hostname) const;
    void print();
};

#endif // CAMERA_HARDWARE_H