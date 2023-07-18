#ifndef ABILITY_INSTANCE_INFO_EXTRACT_H
#define ABILITY_INSTANCE_INFO_EXTRACT_H

#include <string>
#include <vector>

struct Depends
{
    std::vector<std::string> abilities;
    std::vector<std::string> devices;
};

struct AbilityInfoExtract
{
    std::string name; // only include name , not include namespace
    Depends depends;
};

#endif //ABILITY_INSTANCE_INFO_EXTRACT_H