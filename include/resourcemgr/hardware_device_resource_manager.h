#ifndef _HARDWARE_DEVICE_RESOURCE_MANAGER_H
#define _HARDWARE_DEVICE_RESOURCE_MANAGER_H

#include <string>
#include <map>
#include <mutex>

#include "hardware_device_profile.h"

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
    void Init(std::string hostname);
    /**
     * @brief base the discovery manager result, get all hostname device profile
     * @param result
     */
    void EndAddressResult(std::map<std::string, std::string> result);
    /**
     * @brief get hardware device list
     * @param type
     * @return
     */
    std::vector<std::string> GetHardwareResourceList(std::string type);

private:
    /**
     * @brief get device profile
     * @param ip
     * @return
     */
    DeviceProfile getDeviceProfileFromHost(const std::string &ip);
    /**
     * @brief compare the new discovery host, delete the non-existing host
     */
    void compareOldAndNew(std::map<std::string, std::string> new_);

    /**
     * @brief store the whole info about each host
     */
    std::map<std::string, DeviceProfile> hardware_resources;

    /**
     * @brief store the mic device info as a string
     */
    std::vector<std::string> micDevices;
    std::vector<std::string> speakerDevices;
    std::vector<std::string> cameraDevices;
    std::vector<std::string> displayDevices;
    std::mutex locker_;
    bool change = false;

    std::string hostname_;
};

#endif // _HARDWARE_DEVICE_RESOURCE_MANAGER_H