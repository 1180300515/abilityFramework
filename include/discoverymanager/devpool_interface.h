#ifndef _DEVPOOL_INTERFACE_H
#define _DEVPOOL_INTERFACE_H

#include "localhw_interface.h"
#include "discmgr_interface.h"

class DevicePool {
private:
    std::map<std::string, DeviceProfile> deviceList;

public:
    // 构造函数，初始化本地设备
    DevicePool(const DeviceProfile& localDeviceProfile) {
        deviceList["localhost"] = localDeviceProfile;
    }

    // 添加设备
    void addDevice(const std::string& hostname, const DeviceProfile& deviceProfile) {
        deviceList[hostname] = deviceProfile;
    }

    // 删除设备
    void removeDevice(const std::string& hostname) {
        auto it = deviceList.find(hostname);
        if (it != deviceList.end()) {
            deviceList.erase(it);
        } else {
            throw std::runtime_error("Device not found: " + hostname);
        }
    }

    // 更新设备
    void updateDevice(const std::string& hostname, const DeviceProfile& deviceProfile) {
        auto it = deviceList.find(hostname);
        if (it != deviceList.end()) {
            it->second = deviceProfile;
        } else {
            throw std::runtime_error("Device not found: " + hostname);
        }
    }
    
    // 获取设备列表
    const std::map<std::string, DeviceProfile>& getDeviceList() const {
        return deviceList;
    }
};




extern DevicePool devicePool;

#endif // _DEVPOOL_INTERFACE_H
