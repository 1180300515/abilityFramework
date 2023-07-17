#ifndef COMMON_STRUCT_DEFINATION_H
#define COMMON_STRUCT_DEFINATION_H

#include <string>

struct DeviceDescribe
{
    std::string devicename;
    std::string deviceid;
    std::string deviceip;
    std::string deviceport;
    std::string status;
};

struct Metadata
{
    std::string name;
    std::string namespace_name;
};

#endif //COMMON_STRUCT_DEFINATION_H