

#ifndef ABILITYFRAMEWORK_ABILITY_MANAGER_H
#define ABILITYFRAMEWORK_ABILITY_MANAGER_H

#include <condition_variable>
#include <functional>
#include <json/json.h>
#include <optional>
#include <queue>
#include <thread>

#include "ability.h"
#include "ability_message.h"

class abilityManager {
public:
  void Init(std::function<std::string(const std::string&)> get_state_ ,
            std::function<int(const std::string&)> get_ability_port_,
            std::function<void(const AbilityMessage::AbilityCommand&)> set_state_,
            std::function<std::unordered_map<std::string, Ability>()> get_ability_list_);

  std::optional<std::string> handleAbilityCommand(const Json::Value& root);

  std::string handleAbilityList();

  std::string handleAbilityStatus(const std::string &name);

  void Run();

private:
  void pushTask(const AbilityMessage::AbilityCommand& cmd);

  std::function<std::string(const std::string&)> get_state;
  std::function<int(const std::string&)> get_ability_port;
  std::function<void(const AbilityMessage::AbilityCommand&)> set_state;
  std::function<std::unordered_map<std::string, Ability>()> get_ability_list;
  std::function<Ability::locationDef(const std::string&)> get_location;

  std::queue<AbilityMessage::AbilityCommand> taskQueue;
  std::mutex lock_queue;

  std::condition_variable cv;
  std::thread queueClean;
};

#endif // ABILITYFRAMEWORK_ABILITY_MANAGER_H
