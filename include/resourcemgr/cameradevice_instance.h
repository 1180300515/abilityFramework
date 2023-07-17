#ifndef CAMERADEVICE_INSTANCE_H
#define CAMERADEVICE_INSTANCE_H

#include <map>

#include "json/json.h"

#include "device_instance_info.h"

// a cameradevice struct
struct CameraProperties
{
    std::string vendor;
    std::string location;
    int wideAngle;
    std::string focusMethod;
    bool telephoto;
    std::string resolution;
    // following information can get from the device
    std::string deviceNode;
    std::string driverName;
    std::string cardType;
    std::string busInfo;
    std::string description;
    std::vector<std::string> supportFormat;
    std::string interface;
};
struct CameraSpec
{
    std::string version;
    std::string hostname;
    std::string kind;
    std::vector<Acapability> capability1;
    std::vector<Acapability> capability2;
    CameraProperties properties;
    std::map<std::string,std::string> customprops;
};

class CameraInstance : public DeviceInstanceInfo
{
public:
    CameraSpec spec;
    std::string Marshal();
    bool UnMarshal(const Json::Value &jnode);
    bool updateInstance(const Json::Value &jnode);
    std::string getInstanceVersion();
};

#endif