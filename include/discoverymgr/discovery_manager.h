#ifndef _DISCOVERY_MANAGER_H
#define _DISCOVERY_MANAGER_H

#include <functional>
#include <map>
#include <string>
#include <memory>
#include <mutex>

#include "discovery_device_info.h"
#include "lan_ipv4_discovery.h"
#include "ble_discovery.h"
#include "connect_info.h"

class DiscoveryManager
{
public:
    void Init(std::function<void(std::map<std::string, ConnectInfo>)> connection_, std::function<void(std::map<std::string, std::string>)> resource_);
    void Run();

private:
    /**
     * as the lanipv4discovery, blediscovery.. callback
     */
    void ReceiveDeviceInfo(DiscoveryDeviceInfo info);

    std::map<std::string, std::vector<DiscoveryDeviceInfo>> devices;
    std::mutex lock_;

    std::shared_ptr<LANIPV4Discovery> lanipv4discovery_;
    std::shared_ptr<BLEDiscovery> blediscovery_;

    std::function<void(std::map<std::string, ConnectInfo>)> connection_callback; // connection manager callback function
    std::function<void(std::map<std::string, std::string>)> resource_callback;   // resource manager callback function

    std::string hostname_;

    std::thread discovery_thread;
};

#endif // _DISCOVERY_MANAGER_H