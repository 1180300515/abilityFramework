#ifndef _LAN_IPV4_DISCOVERY_H
#define _LAN_IPV4_DISCOVERY_H

#include <string>
#include <functional>
#include <string>
#include <map>
#include <ctime>
#include <thread>

#include "discovery_device_info.h"

class LANIPV4Discovery
{
private:
    std::function<void(DiscoveryDeviceInfo)> callback; // return the discovery device info to the discovery manager

    std::string getHostName();

    void udp_broadcast_receiver();

    std::string discoverySource = "LAN-IPV4-Discovery";

    std::thread receiver_thread;

public:
    /**
     * register the discovery manager callback function to store the discovery result
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

#endif // _LAN_IPV4_DISCOVERY_H