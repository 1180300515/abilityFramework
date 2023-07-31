#ifndef _HARDWARE_DEVICE_RESOURCE_MANAGER_H
#define _HARDWARE_DEVICE_RESOURCE_MANAGER_H

#include <string>
#include <map>
#include <mutex>

#include "hardware_device_profile.h"


class ResourceManager;
/**
 * @brief manager the hardware device resource, include the hardware of the discovered device
 */
class HardwareResourceManager
{
public:
    /**
     * @brief init the hardware resource manager
     * @param hostname
     */
    void Init(std::shared_ptr<ResourceManager> manager_, std::string hostname);
    /**
     * @brief called by discovery manager, base the discovery manager result, get all hostname device profile
     * @param result the discovery manager discovery result
     */
    void EndAddressResult(std::map<std::string, std::string> result);
    /**
     * @brief get hardware device list
     * @param type
     * @return
     */
    std::vector<std::string> GetHardwareResourceList(std::string type);

private:
    std::map<std::string, DeviceProfile> hardware_resources; // store the whole info about each host
    std::chrono::steady_clock::time_point last_update;
    bool change = false; // each discovery result, whether exec get deviceprofile

    std::vector<std::string> micDevices; // store the mic device info as a string
    std::vector<std::string> speakerDevices;
    std::vector<std::string> cameraDevices;
    std::vector<std::string> displayDevices;
    std::mutex locker_;

    std::string hostname_;

    std::shared_ptr<ResourceManager> resource_manager;

    /**
     * @brief get device profile
     * @param ip
     * @return
     */
    DeviceProfile getDeviceProfileFromHost(const std::string &ip);
    /**
     * @brief compare the old and new, delete the non exist host
     * @param new_
     */
    void compareOldAndNew(std::map<std::string, std::string> new_);
};

#endif // _HARDWARE_DEVICE_RESOURCE_MANAGER_H