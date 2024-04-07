/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: hardware_device_resource_manager.h
 * Description: hardware_device_resource_manager.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef RESOURCEMGR_HARDWARE_DEVICE_RESOURCE_MANAGER_H_
#define RESOURCEMGR_HARDWARE_DEVICE_RESOURCE_MANAGER_H_

#include <map>
#include <memory>
#include <mutex>  // NOLINT [build/c++11]
#include <string>
#include <vector>

#include "resourcemgr/hardware_device_profile.h"

class ResourceManager;
/**
 * @brief manager the hardware device resource, include the hardware of the
 * discovered device
 */
class HardwareResourceManager {
   public:
    /**
     * @brief init the hardware resource manager
     * @param hostname
     */
    void Init(std::shared_ptr<ResourceManager> manager_, std::string hostname);
    /**
     * @brief called by discovery manager, base the discovery manager result, get
     * all hostname device profile
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

    std::map<std::string, DeviceProfile> hardware_resources;  // store the whole info about each host
    std::chrono::steady_clock::time_point last_update;
    bool change = false;  // each discovery result, whether exec get deviceprofile

    std::vector<std::string> micDevices;  // store the mic device info as a string
    std::vector<std::string> speakerDevices;
    std::vector<std::string> cameraDevices;
    std::vector<std::string> displayDevices;
    std::map<std::string,std::vector<std::string>> generalDevices;
    std::mutex locker_;

    std::string hostname_;

    std::shared_ptr<ResourceManager> resource_manager;
};

#endif  // RESOURCEMGR_HARDWARE_DEVICE_RESOURCE_MANAGER_H_
