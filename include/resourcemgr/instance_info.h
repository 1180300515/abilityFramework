#ifndef INSTANCE_INFO_H
#define INSTANCE_INFO_H

#include <string>
#include <mutex>
#include <vector>

// common part struct define
struct InstanceMetadata
{
    std::string name;
    std::string namespace_name;
};
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
struct DeviceDescribe
{
    std::string devicename;
    std::string deviceid;
    std::string deviceip;
    std::string deviceport;
    std::string status;
};

/**
 * the base class of all the device
*/
class InstanceInfo {
public:
    std::string apiVersion;
    std::string kind;
    InstanceMetadata metadata;
    DStatus status;
    Api api;
    std::vector<DeviceDescribe> devicelist;

    std::mutex resourcelock_ ;
    
    virtual std::string Marshal() = 0;
    virtual bool UnMarshal(std::string source);
    virtual bool updateInstance(std::string data) = 0;
    virtual std::string getInstanceVersion() = 0;
};





#endif //INSTANCE_INFO_H