#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <mutex>
#include <memory>
#include <map>
#include <thread>

#include "device_instance_info.h"
#include "ability_instance_info.h"
#include "nonlocal_resource_info.h"
#include "hardware_scan.h"
#include "message_package_struct.h"
#include "connection_manager.h"

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager() = default;
    /**
     * @brief regist crd into database
     * @param data the crd data ot file path
     * @param from_file from file or not
     * @return
     */
    bool RegistCrd(const std::string &data, bool from_file = false);
    /**
     * @brief unregist crd
     * @param name
     * @return
     */
    bool UnregistCrd(const std::string &name);
    /**
     * @brief add a ability instance
     * @param data the instance data or the file path
     * @param from_file from file or not
     * @return success or not
     */
    bool AddAbilityInstance(const std::string &data, bool from_file = false);
    bool UpdateAbilityInstance(const std::string &data, bool from_file = false);
    bool DeleteAbilityInstance(const std::string &key);

    /**
     * @brief add a device instance (also called by the hardwarescan the add a device)
     * @param data the file path or the device data
     * @param from_file from file or not
     * @return success or not
     */
    bool AddDeviceInstance(const std::string &data, bool from_file = false);
    bool UpdateDeviceInstance(const std::string &data, bool from_file = false);
    bool DeleteDeviceInstance(const std::string &key);

    /**
     * @brief judge the key resource type
     * @param key 
     * @return 
     */
    std::string isLocalResource(std::string key);
    /**
     * @brief called by lifecycle manager to judge ability exist or not
     * @param key the ability key
     * @return exist or not
     */
    bool AbilityExistJudge(const std::string &key);
    /**
     * @brief
     */
    void LoadLocalResource();
    /**
     * @brief init the resource manager
     */
    void Init(std::shared_ptr<ConnectionManager> connect);

    void Run();
    void RefreshKVRecord();
    /**
     * @brief get the local hardware device json string
     * @param format format string or not
     * @return the json string
     */
    std::string GetHardwareDeviceInfo(bool format = false);

private:
    /**
     * @brief add nonlocal resource into the map
     * @param data the nonlocal resource
     */
    void addNonLocalResource(const std::string &data);
    /**
     * @brief  handle the end message
     * @param message  the cloud message
     */
    void cloudMessageHandle(const KeyAndDataPackages &data);
    /**
     * @brief handle the end message
     * @param message data
     */
    void endMessageHandle(const KeyAndDataPackages &data);
    /**
     * @brief called by the connection manager to handle the recv message received by the server
     * @param message data
     */
    void recvMessageHandle(const std::string &message);
    /**
     * @brief generate nonlocal format data
     * @return the result
     */
    std::string generateNonLocalFormat();
    /**
     * @brief get the host name
     */
    void getHostName();
    /**
     * @brief sync with cloud
     */
    void cloudSyncThread();
    /**
     * @brief sync with end
     */
    void endSyncThread();

    std::shared_ptr<HardwareScan> hardware_;
    std::shared_ptr<ConnectionManager> connection_;

    std::map<std::string, std::string> versionRecord; // record the key and the version
    std::map<std::string, std::shared_ptr<DeviceInstanceInfo>> devices_;
    std::map<std::string, std::shared_ptr<AbilityInstanceInfo>> abilities_;
    std::map<std::string, std::shared_ptr<NonLocalResource>> nonlocal_instances_;

    std::mutex nonlocal_lock_;
    std::mutex abilities_lock_;
    std::mutex devices_lock_;

    std::thread cloud_sync_;
    std::thread end_sync_;

    std::string hostname_;
};

#endif // RESOURCE_MANAGER_H