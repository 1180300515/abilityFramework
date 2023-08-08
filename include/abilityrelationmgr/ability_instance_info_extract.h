#ifndef ABILITYRELATIONMGR_ABILITY_INSTANCE_INFO_EXTRACT_H_
#define ABILITYRELATIONMGR_ABILITY_INSTANCE_INFO_EXTRACT_H_

#include <string>
#include <vector>

struct Depends {
  std::vector<std::string> abilities;
  std::vector<std::string> devices;
};

struct AbilityInfoExtract {
  std::string name;  // only include name , not include namespace
  Depends depends;
};

#endif  // ABILITYRELATIONMGR_ABILITY_INSTANCE_INFO_EXTRACT_H_
