

#ifndef ABILITYFRAMEWORK_ABILITY_MESSAGE_H
#define ABILITYFRAMEWORK_ABILITY_MESSAGE_H

#include <json/json.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace AbilityMessage {
struct desireProp {
  std::string controlName;
  std::string controlIntent;
};

//
struct AbilityCommand {
  std::string abilityName;
  std::string type;
  std::string ip;
  std::string desireState;
  Json::Value stateParam; // 关于的附加参数

  std::vector<desireProp> desire;

  bool parseJson(const Json::Value &root);
  [[nodiscard]] Json::Value toJson() const;

  int priority = 0;

  bool operator<(const AbilityCommand& other) const;
};

struct AbilityStatus {
  std::string abilityName;
  struct Location {
    float x;
    float y;
    float z;
    std::string scene;
  };
  Location location;
  std::string state;
  std::unordered_map<std::string, std::string> control;
  std::unordered_map<std::string, std::string> obs;
  Json::Value depend;

  bool parseJson(const Json::Value &root);
  [[nodiscard]] Json::Value toJson() const;
};
} // namespace AbilityMessage

#endif // ABILITYFRAMEWORK_ABILITY_MESSAGE_H