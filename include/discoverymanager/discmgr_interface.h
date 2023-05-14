#ifndef _DISCMGR_INTERFACE_H
#define _DISCMGR_INTERFACE_H

#include "localhw_interface.h"
#include <iostream>
#include <thread>
#include <fstream>
#include <functional>
#include <map>

struct Device
{
  std::string hostname;
  std::string ip;
  std::time_t last_online_time;
  std::time_t online_duration;
  std::string status;

  void Update(const std::string& new_hostname, const std::string& new_status, std::time_t new_timestamp)
  {
    if (hostname != new_hostname)
    {
      hostname = new_hostname;
    }

    status = new_status;

    if (last_online_time != 0)
    {
      online_duration += new_timestamp - last_online_time;
    }

    last_online_time = new_timestamp;
  }
};

void udp_broadcast_sender();
void udp_broadcast_receiver(std::function<void(std::map<std::string, std::string>)> callback);

#endif // _DISCMGR_INTERFACE_H