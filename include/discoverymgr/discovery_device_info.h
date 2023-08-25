/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: discovery_device_info.h
 * Description: discovery_device_info.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef DISCOVERYMGR_DISCOVERY_DEVICE_INFO_H_
#define DISCOVERYMGR_DISCOVERY_DEVICE_INFO_H_

#include <ctime>
#include <string>

struct DiscoveryDeviceInfo {
  std::string hostname;
  std::string address;
  std::string discovery_source;  // Describe the source of this information(like
                                 // LAN-IPV4-Discovery)
  std::time_t last_online_time;
  std::time_t online_duration;
  std::string status;

  bool IsOffline(std::time_t current_time) const;
  void Update(const std::string &new_address, const std::string &new_status,
              std::time_t new_timestamp);
};

#endif  // DISCOVERYMGR_DISCOVERY_DEVICE_INFO_H_
