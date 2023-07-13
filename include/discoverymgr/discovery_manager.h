#ifndef DISCOVERY_MANAGER_H
#define DISCOVERY_MANAGER_H

#include <functional>
#include <map>
#include <string>
#include <memory>
#include <mutex>

#include "discovery_device_info.h"
#include "lan_ipv4_discovery.h"
#include "ble_discovery.h"

class DiscoveryManager
{
private:
    std::map<std::string, std::vector<DiscoveryDeviceInfo>> devices;
    std::mutex lock_;

    std::shared_ptr<LANIPV4Discovery> lanipv4discovery_;
    std::shared_ptr<BLEDiscovery> blediscovery_;

    std::function<void(std::map<std::string, std::string>)> callback;

    /**
     * as the lanipv4discovery, blediscovery.. callback
    */
    void ReceiveDeviceInfo(DiscoveryDeviceInfo info);
public:
    
};

#endif // DISCOVERY_MANAGER_H