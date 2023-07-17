#ifndef INSTANCE_INFO_H
#define INSTANCE_INFO_H

#include <string>
#include <mutex>
#include <vector>

#include "json/json.h"

#include "common_struct_defination.h"

// common part struct define

struct DStatus
{
    bool occupancy;
};
struct Adeviceparam
{
    std::string name;
    std::string type;
    std::string index;
};
struct Afunction
{
    std::string name;
    std::vector<Adeviceparam> param;
    std::vector<Adeviceparam> returnparam;
};
struct Api
{
    std::vector<Afunction> function;
};
struct Acapability
{
    std::string name;
    std::vector<std::string> api;
};

/**
 * the base class of all the device
*/
class DeviceInstanceInfo {
public:
    std::string apiVersion;
    std::string kind;
    Metadata metadata;
    DStatus status;
    Api api;
    std::vector<DeviceDescribe> devicelist;

    std::mutex resourcelock_ ;
    
    virtual std::string Marshal() = 0;
    virtual bool UnMarshal(const Json::Value &jnode);
    virtual bool updateInstance(const Json::Value &jnode) = 0;
    virtual std::string getInstanceVersion() = 0;
};





#endif //INSTANCE_INFO_H