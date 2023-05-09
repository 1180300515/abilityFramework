#ifndef ABILITY_STRUCT_H
#define ABILITY_STRUCT_H

#include "controller/common/common_struct.h"
#include <map>

// ability struct defination
struct Aabilityparam
{
    std::string paramname;
    std::string paramtype;
    std::string index;
};
struct Aapi
{
    std::string ApiType;
    std::string ApiName;
    std::vector<Aabilityparam> inputparam;
    std::vector<Aabilityparam> returnparam;
};
struct Aabilityinstance
{
    std::string devicename;
    std::string deviceid;
    std::string deviceip;
    std::string port;
    std::string status;
};
struct Ability
{
    std::string apiVersion;
    std::string kind;
    Metadata metadata;
    std::string level;
    std::string abilityname;
    std::string description;
    std::string followed;
    std::vector<std::string> required;
    std::map<std::string, std::string> spec;
    std::vector<Aapi> ApiList;
    std::vector<Aabilityinstance> abilityinstancelist;
};


bool UnMarshal(const std::string source, Ability &des);
std::string Marshal(Ability &source);
#endif