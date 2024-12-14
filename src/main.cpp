
#include <iostream>

#include "ability_manager.h"
#include "http_server.h"
#include "lifecycle_manager.h"
#include "resource_manager.h"
#include "broadcast_manager.h"

int main() {
  resourceManager resource_mgr;
  abilityManager ability_mgr;
  lifeCycleManager lifeCycle_mgr;
  broadcastManager broadcast_mgr;
  httpServer http_svr;

  resource_mgr.Init();
  ability_mgr.Init(
      [&lifeCycle_mgr](const std::string &name) -> std::string {
        return lifeCycle_mgr.getState(name);
      },
      [&lifeCycle_mgr](const std::string &name) -> int {
        return lifeCycle_mgr.getAbilityPort(name);
      },
      [&lifeCycle_mgr](const AbilityMessage::AbilityCommand &cmd) {
        lifeCycle_mgr.lifeCycleCommand(cmd);
      },
      [&resource_mgr]() -> std::unordered_map<std::string, Ability> {
        return resource_mgr.getAbilityList();
      });
  lifeCycle_mgr.Init();
  http_svr.Init(
      [&lifeCycle_mgr](const Json::Value &value) -> bool {
        return lifeCycle_mgr.handleAbilityHeartbeat(
            value); // Return true on success, false on failure
      },
      [&ability_mgr](const Json::Value &value) -> auto {
        // Implement ability command processing logic here
        // ...
        return ability_mgr.handleAbilityCommand(value);
      },
      [&ability_mgr]() -> auto { return ability_mgr.handleAbilityList(); },
      [&ability_mgr](const std::string &name) {
        return ability_mgr.handleAbilityStatus(name);
      });

  resource_mgr.Run();
  lifeCycle_mgr.Run();
  ability_mgr.Run();
  broadcast_mgr.Run();
  http_svr.Run();
  return 0;
}
