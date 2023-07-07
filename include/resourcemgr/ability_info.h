#ifndef ABILITY_INFO_H
#define ABILITY_INFO_H

#include <string>
#include <map>
#include <vector>
#include <mutex>


// ability struct defination
struct AbilityMetadata
{
    std::string name;
    std::string namespace_name;
};
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
class AbilityInfo
{
public:
    std::string apiVersion;
    std::string kind;
    AbilityMetadata metadata;
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


#endif // ABILITY_INFO_H