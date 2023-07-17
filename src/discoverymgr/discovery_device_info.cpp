#include "discovery_device_info.h"

bool DiscoveryDeviceInfo::IsOffline(std::time_t current_time) const
{
    return current_time - last_online_time > 30; // 30 seconds
}

void DiscoveryDeviceInfo::Update(const std::string &new_address,const std::string &new_status, std::time_t new_timestamp)
{
    address = new_address;
    status = new_status;

    if (last_online_time != 0)
    {
        online_duration += new_timestamp - last_online_time;
    }

    last_online_time = new_timestamp;
}
