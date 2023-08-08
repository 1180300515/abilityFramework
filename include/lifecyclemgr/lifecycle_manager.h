#ifndef LIFECYCLEMGR_LIFECYCLE_MANAGER_H_
#define LIFECYCLEMGR_LIFECYCLE_MANAGER_H_

#include <memory>
#include <string>
#include <thread>  // NOLINT [build/c++11]
#include <unordered_map>

#include "lifecyclemgr/abilityclient.h"
#include "lifecyclemgr/command_info.h"
#include "lifecyclemgr/heartbeat_info.h"

class LifeCycleManager {
 public:
  /**
   * @brief handle the command info
   * @param cmd_info
   */
  void HandleCommandInfo(const CommandInfo &cmd_info);
  /**
   * @brief add a heart beart info
   * @param info
   * @return
   */
  bool AddHeartbeatInfo(HeartbeatInfo info);
  /**
   * @brief
   * @param callback
   */
  void Init(std::function<bool(std::string)> callback);
  /**
   * @brief start lifecycle manager
   */
  void Run();
  /**
   * @brief get the heart beat map
   * @return
   */
  std::string GetHeartbeatMap();

 private:
  // store IPCPort and ability lifecycle deal thread
  std::unordered_map<int, std::thread> threads;
  // store IPCPort and ability client (by grpc)
  std::unordered_map<int, std::shared_ptr<AbilityClient>> clients;
  std::mutex clients_lock_;

  // IPCPort and the heart beat info
  std::unordered_map<int, HeartbeatInfo> heartbeat_map;
  std::mutex heartbeat_map_lock;

  std::thread checkClientThread;

  // resource manager check resource exist function
  std::function<bool(std::string)> resourcemgr_checkexist;

  void lifeCycleDeal(std::shared_ptr<AbilityClient> client,
                     const HeartbeatInfo &hbinfo, const CommandInfo &cmdinfo);

  bool start_process(const std::string &abilityName);

  void checkAbilityClientAndTimeout();
};

#endif  // LIFECYCLEMGR_LIFECYCLE_MANAGER_H_
