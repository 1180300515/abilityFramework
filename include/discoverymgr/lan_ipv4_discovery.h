#ifndef DISCOVERYMGR_LAN_IPV4_DISCOVERY_H_
#define DISCOVERYMGR_LAN_IPV4_DISCOVERY_H_

#include <ctime>
#include <functional>
#include <map>
#include <string>
#include <thread>  // NOLINT [build/c++11]

#include "discoverymgr/discovery_device_info.h"

class LANIPV4Discovery {
 private:
  std::function<void(DiscoveryDeviceInfo)>
      callback;  // return the discovery device info to the discovery manager

  std::string getHostName();

  void udp_broadcast_receiver();

  std::string discoverySource = "LAN-IPV4-Discovery";

  std::thread receiver_thread;

 public:
  /**
   * register the discovery manager callback function to store the discovery
   * result
   */
  void RegisterCallback(std::function<void(DiscoveryDeviceInfo)> callback);
  /**
   * start a broadcast
   */
  void BroadcastSender();
  /**
   * start broadcast receiver thread
   */
  void RunBroadcastReceiver();
};

#endif  // DISCOVERYMGR_LAN_IPV4_DISCOVERY_H_
