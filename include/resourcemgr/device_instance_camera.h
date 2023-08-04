#ifndef _DEVICE_INSTANCE_CAMERA_H
#define _DEVICE_INSTANCE_CAMERA_H

#include <map>

#include "json/json.h"

#include "device_instance_info.h"

// a cameradevice struct
struct CameraProperties
{
    //get from the yaml
    std::string vendor;
    std::string resolution;
    std::string location;
    int wideAngle;
    std::string focusMethod;
    bool telephoto;
    std::string interface;
    //get from the hardware
    std::string devicePath;
    std::string driverName;
    std::string card; 
    std::string busInfo;
    std::vector<std::string> supportFormat;
};
struct CameraSpec
{
    std::string version;
    std::string hostname;
    std::string kind;
    std::string hardwareidentifier; // a hardware indentify in order to combine the hardware info with the instance,camera use "card",audio use "name",dispaly use "screen"
    std::vector<Acapability> capability1;
    std::vector<Acapability> capability2;
    CameraProperties properties;
    //std::map<std::string, std::string> customprops;
};

class CameraInstance : public DeviceInstanceInfo
{
public:
    CameraSpec spec;

    std::string GetHardwareIdentifier() const;
    bool UpdateHardwareInfo(const Json::Value &info);
    void EraseHardwareInfo();
    std::string Marshal() const;
    Json::Value ToJson() const;

    bool FromJson(const Json::Value &jnode);
    bool UnMarshal(const std::string &data);
    bool updateInstance(const Json::Value &jnode);
    std::string getInstanceVersion() const;
};

#endif // _DEVICE_INSTANCE_CAMERA_H