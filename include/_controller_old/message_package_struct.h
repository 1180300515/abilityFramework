#ifndef MESSAGE_PACKAGE_STRUCT_H
#define MESSAGE_PACKAGE_STRUCT_H

#include <vector>
#include <string>

struct KeyDatapack
{
    std::string key; // represent the key in json , format: namespace/name
    std::string version; // represent the version in json 
    std::string eventType; // represent the eventtype in json
    std::string resourceType; // 资源类型
    std::string data; // represent the data in json储存全部信息
};

// as the message between server and client 
struct KeyAndDataPackages
{
    std::vector<KeyDatapack> data;
    std::string hostname;
    std::string packageType; // the type of this message: kv , s2c , c2s , notify
};



bool UnMarshalMessageStruct(std::string data , KeyAndDataPackages &kdpak);
std::string MarshalMessageStruct(KeyAndDataPackages &kdpak);

#endif