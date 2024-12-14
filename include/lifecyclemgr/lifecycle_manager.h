#ifndef LIFECYCLEMGR_LIFECYCLE_MANAGER_H_
#define LIFECYCLEMGR_LIFECYCLE_MANAGER_H_

#include <future> // NOLINT [build/c++11]
#include <memory>
#include <shared_mutex>
#include <string>
#include <thread> // NOLINT [build/c++11]
#include <unordered_map>

#include "ability_message.h"
#include "heartbeat_info.h"

// 声明式控制生命周期
class lifeCycleManager {
public:
  // 处理心跳信息更新
  bool handleAbilityHeartbeat(const Json::Value &info);
  // 处理生命周期
  void lifeCycleCommand(const AbilityMessage::AbilityCommand &cmd);

  void Init();

  void Run();

  std::string getState(const std::string& abilityName);

  int getAbilityPort(const std::string& abilityName);

private:
  int getIPCPort(const std::string& abilityName);

  static bool start_process(const std::string &abilityName);

  std::unordered_map<std::string, HeartbeatInfo> heartbeat_map;
  std::shared_mutex heartbeat_map_lock;
};

#endif // LIFECYCLEMGR_LIFECYCLE_MANAGER_H_
