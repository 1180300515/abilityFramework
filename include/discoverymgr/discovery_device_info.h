#ifndef _DISCOVERY_DEVICE_INFO_H
#define _DISCOVERY_DEVICE_INFO_H

#include <ctime>
#include <string>

struct DiscoveryDeviceInfo
{
    std::string hostname;
    std::string address;
    std::string discovery_source; // Describe the source of this information(like LAN-IPV4-Discovery)
    std::time_t last_online_time;
    std::time_t online_duration;
    std::string status;

    bool IsOffline(std::time_t current_time) const;
    void Update(const std::string &new_address, const std::string &new_status, std::time_t new_timestamp);
};

#endif // _DISCOVERY_DEVICE_INFO_H