#ifndef DISCOVERYMGR_DISCOVERY_MANAGER_H_
#define DISCOVERYMGR_DISCOVERY_MANAGER_H_

#include <functional>
#include <map>
#include <memory>
#include <mutex>  // NOLINT [build/c++11]
#include <string>
#include <vector>

#include "discoverymgr/discovery_device_info.h"
#include "discoverymgr/lan_ipv4_discovery.h"

class DiscoveryManager {
 public:
  void Init(std::function<void(std::map<std::string, std::string>)> resource_);
  void Run();

 private:
  /**
   * as the lanipv4discovery... callback
   */
  void ReceiveDeviceInfo(DiscoveryDeviceInfo info);

  std::map<std::string, std::vector<DiscoveryDeviceInfo>> devices;
  std::mutex lock_;

  std::shared_ptr<LANIPV4Discovery> lanipv4discovery_;

  std::function<void(std::map<std::string, std::string>)>
      resource_callback;  // resource manager callback function to remind the
                          // resource manager about discovery result

  std::string hostname_;

  std::thread discovery_thread;
};

#endif  // DISCOVERYMGR_DISCOVERY_MANAGER_H_
