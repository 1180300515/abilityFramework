#ifndef CAMERADEVICE_INSTANCE_H
#define CAMERADEVICE_INSTANCE_H

#include "controller/common/common_struct.h"
#include "controller/common/Instance.h"

// a cameradevice struct
struct CameraProperties
{
    std::string vendor;
    std::string resolution;
    std::string location;
    int wideAngle;
    std::string focusMethod;
    bool telephoto;
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