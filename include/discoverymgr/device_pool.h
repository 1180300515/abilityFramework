#ifndef DEVICE_POOL_H
#define DEVICE_POOL_H

#include <map>
#include <string>

#include "device_profile.h"

class DevicePool
{
private:
    std::map<std::string, DeviceProfile> deviceList;

};

class DevicePoolExtended
{
};

#endif // DEVICE_POOL_H