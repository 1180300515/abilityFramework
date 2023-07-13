#ifndef LAN_IPV4_DISCOVERY_H
#define LAN_IPV4_DISCOVERY_H


#include <string>
#include <functional>
#include <string>
#include <map>
#include <ctime>

#include "discovery_device_info.h"

class LANIPV4Discovery
{
private:
    std::function<void(DiscoveryDeviceInfo)> callback; // return the discovery device info to the discovery manager

    std::string getHostName();
    void udp_broadcast_sender();
    void udp_broadcast_receiver();

    std::string discoverySource = "LAN-IPV4-Discovery";

public:
    void RegisterCallback(std::function<void(std::map<std::string, std::string>)> callback);
};


#endif //LAN_IPV4_DISCOVERY_H