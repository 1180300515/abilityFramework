#ifndef SENSORDEVICE_INSTANCE_H
#define SENSORDEVICE_INSTANCE_H

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
    std::vector<Acapability> capability1;
    std::vector<Acapability> capability2;
    Sensorproperties properties;
    std::map<std::string,std::string> customprops;
};
class SensorInstance : public DeviceInstanceInfo
{
public:
    SensorSpec spec;
    std::string Marshal();
    bool UnMarshal(const Json::Value &jnode);
    bool updateInstance(const Json::Value &jnode);
    std::string getInstanceVersion();
};
#endif