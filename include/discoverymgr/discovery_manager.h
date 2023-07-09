#ifndef DISCOVERY_MANAGER_H
#define DISCOVERY_MANAGER_H

#include <functional>
#include <map>
#include <string>
#include <memory>

#include "device_pool.h"
#include "udp_discovery.h"
#include "ble_discovery.h"

class DiscoveryManager
{
private:
    DevicePool devicepool;
    DevicePoolExtended devicepoolextended;

    std::shared_ptr<UDPDiscovery> udpdiscovery_;
    std::shared_ptr<BLEDiscovery> blediscovery_;

    std::function<void(std::map<std::string, std::string>)> callback;

public:
    void Getdevicepool(DevicePool &devicepool);
    void Getdevicepoolextended(DevicePoolExtended &devicepoolextended);
};

#endif // DISCOVERY_MANAGER_H