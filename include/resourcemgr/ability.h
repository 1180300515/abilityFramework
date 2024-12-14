

#ifndef ABILITYFRAMEWORK_ABILITY_H
#define ABILITYFRAMEWORK_ABILITY_H

#include <string>
#include <vector>
#include <json/json.h>

// 用于/api/AbilityList的数据
struct Ability {
  std::string abilityName;
  std::string type;

  struct locationDef {
    float x;
    float y;
    float z;
    std::string scene;
  };

  // QoS信息
  float response;
  float reliability;
  float availability;

  locationDef location;

  std::vector<std::string> abilityDepend;

  std::string frameworkIP;
  std::string state; // 生命状态

  void parseJson(const Json::Value& root);
  [[nodiscard]] Json::Value toJson() const;
};

#endif // ABILITYFRAMEWORK_ABILITY_H
