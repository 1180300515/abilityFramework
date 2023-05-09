#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>
#include <mutex>

#include <controller/common/common_struct.h>

/**
 * 所有实例的基类
*/
class Instance {
public:
    std::string apiVersion;
    std::string kind;
    Metadata metadata;
    DStatus status;
    Api api;
    std::mutex resourcelock_ ;
    std::vector<DeviceDescribe> devicelist;

    virtual std::string Marshal() = 0;
    virtual bool UnMarshal(std::string source);
    virtual bool updateInstance(std::string data) = 0;
    virtual std::string getInstanceVersion() = 0;
};

#endif