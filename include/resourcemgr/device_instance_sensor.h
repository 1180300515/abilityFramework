#ifndef _DEVICE_INSTANCE_SENSOR_H
#define _DEVICE_INSTANCE_SENSOR_H

#include <map>

#include "json/json.h"

#include "device_instance_info.h"

// sensor part
struct Sensorproperties
{
    std::string vendor;
    std::string location;
    std::string interface;
};
struct SensorSpec
{
    std::string version;
    std::string hostname;
    std::string kind;
    std::string hardwareidentifier; // a hardware indentify in order to combine the hardware info with the instance,camera use "card",audio use "name",dispaly use "screen"

    std::vector<Acapability> capability1;
    std::vector<Acapability> capability2;
    Sensorproperties properties;
    std::map<std::string, std::string> customprops;
};
class SensorInstance : public DeviceInstanceInfo
{
public:
    SensorSpec spec;
    std::string Marshal();
    std::string GetHardwareIdentifier();
    bool UpdateHardwareInfo(const Json::Value &info);
    bool FromJson(const Json::Value &jnode);
    bool UnMarshal(const std::string &data);
    bool updateInstance(const Json::Value &jnode);
    std::string getInstanceVersion();
};

#endif // _DEVICE_INSTANCE_SENSOR_H