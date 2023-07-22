#ifndef _ABILITY_INSTANCE_INFO_H
#define _ABILITY_INSTANCE_INFO_H

#include <string>
#include <map>
#include <vector>
#include <mutex>

#include "json/json.h"

#include "common_struct_defination.h"

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

/**
 * ability info defination
 */
class AbilityInstanceInfo
{
public:
    std::string apiVersion;
    std::string kind;
    Metadata metadata;
    std::string level;
    std::string abilityname;
    std::string description;
    std::string followed;
    std::string version;
    Abilitydepends depends;
    std::map<std::string, std::string> spec;
    std::vector<Aapi> ApiList;
    std::vector<Aabilityinstance> abilityinstancelist;

    bool UnMarshal(const std::string &data);
    bool FromJson(const Json::Value &jnode);
    std::string Marshal();
    bool updateAbility(const Json::Value &jnode);
};

#endif // _ABILITY_INSTANCE_INFO_H