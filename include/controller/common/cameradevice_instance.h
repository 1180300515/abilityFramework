#ifndef CAMERADEVICE_INSTANCE_H
#define CAMERADEVICE_INSTANCE_H

#include "controller/common/common_struct.h"
#include "controller/common/Instance.h"

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

class CameraInstance : public Instance
{
public:
    CameraSpec spec;
    std::string Marshal();
    bool UnMarshal(std::string source);
    bool updateInstance(std::string data);
    std::string getInstanceVersion();
};

#endif