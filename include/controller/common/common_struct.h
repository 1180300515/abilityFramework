#ifndef COMMON_STRUCT_H
#define COMMON_STRUCT_H

#include <string>
#include <vector>
#include <memory>

// common part struct define
struct Metadata
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

// to describe a resource data that will keep in nonlocal host
struct NonLocalResource
{
    std::string name;
    std::string namespace_name;
    std::string hostname;
    std::string kind;
    std::vector<DeviceDescribe> devicelist;
};

/**
 * deserialization the json string into the NonLocalResource struct
 * 
 * @param source the json string 
 * @param des the target struct
 * 
 * @return success or not
*/
bool UnMarshal(const std::string source, std::shared_ptr<NonLocalResource> des);
/**
 * deserialization the NonLocalResource into the json string
 * 
 * @param source the source struct
 * 
 * @return the json string
*/
std::string Marshal(std::shared_ptr<NonLocalResource> source);
#endif