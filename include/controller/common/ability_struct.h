#ifndef ABILITY_STRUCT_H
#define ABILITY_STRUCT_H

#include <map>
#include <mutex>

#include "controller/common/common_struct.h"

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
struct Abilitydepends
{
    std::vector<std::string> abilities;
    std::vector<std::string> devices;
};
class Ability
{
public:
    std::string apiVersion;
    std::string kind;
    Metadata metadata;
    std::string level;
    std::string abilityname;
    std::string description;
    std::string followed;
    Abilitydepends depends;
    std::map<std::string, std::string> spec;
    std::vector<Aapi> ApiList;
    std::vector<Aabilityinstance> abilityinstancelist;

    std::mutex abilitylock_;

    bool UnMarshal(const std::string source);
    std::string Marshal();
    bool updateAbility(std::string data);

};

#endif