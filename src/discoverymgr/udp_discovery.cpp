#include "udp_discovery.h"

bool Device::IsOffline(std::time_t current_time) const
{
    return false;
}

void Device::Update(const std::string &new_hostname, const std::string &new_status, std::time_t new_timestamp)
{
}

std::string UDPDiscovery::getHostName()
{
    return std::string();
}

void UDPDiscovery::udp_broadcast_sender()
{
}

void UDPDiscovery::udp_broadcast_receiver()
{
}

void UDPDiscovery::RegisterCallback(std::function<void(std::map<std::string, std::string>)> callback)
{
}
