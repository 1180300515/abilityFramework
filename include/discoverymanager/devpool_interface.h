#ifndef _DEVPOOL_INTERFACE_H
#define _DEVPOOL_INTERFACE_H

#include "localhw_interface.h"
#include "discmgr_interface.h"
#include "utils/color.h"
#include <unordered_map>

// 在字符串键值对中查找指定键的值
std::string findValueByKey(const std::string& keyValueString, const std::string& key);

std::string cameraHardwareToKeyValue(std::string hostname, const CameraHardware& cameraHardware);
std::string audioDeviceToKeyValue(std::string hostname, const AudioDevice& audioDevice);
std::string displayInfoToKeyValue(std::string hostname, const DisplayInfo& displayInfo);

class DevicePool {
public:
    std::map<std::string, DeviceProfile> deviceList;
    // 构造函数，初始化本地设备
    DevicePool(std::string hostname, const DeviceProfile& localDeviceProfile) {
        std::cout << RED << "DevicePool constructor" << NONE << std::endl;
        deviceList[hostname] = localDeviceProfile;
    }

    // 添加设备
    void addDevice(const std::string& hostname, const DeviceProfile& deviceProfile) {
        std::cout << L_RED << "DevicePool addDevice" << NONE << std::endl;
        deviceList[hostname] = deviceProfile;
    }

    // 删除设备
    void removeDevice(const std::string& hostname) {

        std::cout << L_RED << "DevicePool removeDevice: " << hostname << NONE << std::endl;
        auto it = deviceList.find(hostname);
        std::cout << "find result: " << (it == deviceList.end() ? "not found" : "found") << std::endl;
        if (it != deviceList.end()) {
            std::cout << "Erased :" << it->first << std::endl;
            deviceList.erase(it);
            std::cout << "Erased finished" << std::endl;
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

class DevicePoolExtended{
public:
    std::vector<std::string> micDevices;
    std::vector<std::string> speakerDevices;
    std::vector<std::string> cameraDevices;
    std::vector<std::string> displayDevices;

    DevicePoolExtended(DevicePool &devicePool){
        for (const auto& item : devicePool.deviceList){
            for(const auto& micDevice : item.second.micDevices){
                micDevices.push_back(audioDeviceToKeyValue(item.first, micDevice));
            }
            for(const auto& speakerDevice : item.second.speakerDevices){
                speakerDevices.push_back(audioDeviceToKeyValue(item.first, speakerDevice));
            }
            for(const auto& cameraDevice : item.second.cameraDevices){
                cameraDevices.push_back(cameraHardwareToKeyValue(item.first, cameraDevice));
            }
            for(const auto& displayDevice : item.second.displayDevices){
                displayDevices.push_back(displayInfoToKeyValue(item.first, displayDevice));
            }
        }
    }

    void addDevice(std::string hostname, DeviceProfile &deviceProfile){
        std::cout << L_RED << "DevicePoolExtended addDevice: " << hostname << NONE << std::endl;
        for(const auto& micDevice : deviceProfile.micDevices){
            micDevices.push_back(audioDeviceToKeyValue(hostname, micDevice));
        }
        for(const auto& speakerDevice : deviceProfile.speakerDevices){
            speakerDevices.push_back(audioDeviceToKeyValue(hostname, speakerDevice));
        }
        for(const auto& cameraDevice : deviceProfile.cameraDevices){
            cameraDevices.push_back(cameraHardwareToKeyValue(hostname, cameraDevice));
        }
        for(const auto& displayDevice : deviceProfile.displayDevices){
            displayDevices.push_back(displayInfoToKeyValue(hostname, displayDevice));
        }
    }

    void deleteDevice(std::string hostname){
        std::cout << L_RED << "DevicePoolExtended deleteDevice: " << hostname << NONE << std::endl;
        for (auto it = micDevices.begin(); it != micDevices.end();){
            std::cout << "iteration: " << *it << std::endl;
            if (findValueByKey(*it, "hostname") == hostname){
                std::cout << "find to delete: " << *it << std::endl;
                it = micDevices.erase(it);
            } else {
                ++it;
            }
        }
        for (auto it = speakerDevices.begin(); it != speakerDevices.end();){
            if (findValueByKey(*it, "hostname") == hostname){
                it = speakerDevices.erase(it);
            } else {
                ++it;
            }
        }
        for (auto it = cameraDevices.begin(); it != cameraDevices.end();){
            if (findValueByKey(*it, "hostname") == hostname){
                it = cameraDevices.erase(it);
            } else {
                ++it;
            }
        }
        for (auto it = displayDevices.begin(); it != displayDevices.end();){
            if (findValueByKey(*it, "hostname") == hostname){
                it = displayDevices.erase(it);
            } else {
                ++it;
            }
        }
    }

    void updateDevice(std::string hostname, DeviceProfile &deviceProfile){
        deleteDevice(hostname);
        addDevice(hostname, deviceProfile);
    }

    void print(){
        std::cout << "Mic devices: " << std::endl;
        for (const auto& micDevice : micDevices){
            std::cout << micDevice << std::endl;
        }
        std::cout << "Speaker devices: " << std::endl;
        for (const auto& speakerDevice : speakerDevices){
            std::cout << speakerDevice << std::endl;
        }
        std::cout << "Camera devices: " << std::endl;
        for (const auto& cameraDevice : cameraDevices){
            std::cout << cameraDevice << std::endl;
        }
        std::cout << "Display devices: " << std::endl;
        for (const auto& displayDevice : displayDevices){
            std::cout << displayDevice << std::endl;
        }
    }
};

extern DevicePool devicePool;
extern DevicePoolExtended devicePoolExtended;

#endif // _DEVPOOL_INTERFACE_H
