#ifndef _RESOURCE_MANAGER_H
#define _RESOURCE_MANAGER_H

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
#include "ability_instance_info_extract.h"
#include "hardware_device_resource_manager.h"

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
     * @brief load data from the database
     */
    void LoadLocalResourceInDB();
    /**
     * @brief judge the key resource type
     * @param key
     * @return
     */
    std::string isLocalResource(std::string key);

    /**
     * @brief init the resource manager
     */
    void Init(std::shared_ptr<ConnectionManager> connect, bool cleandb);
    /**
     * @brief start the resource manager module
     * @param startcloudsync
     * @param startendsync
     */
    void Run(bool startcloudsync, bool startendsync);
    /**
     * @brief refresh the key and version record
     */
    void RefreshKVRecord();
    /**
     * @brief get the local hardware device json string
     * @param format format string or not
     * @return the json string
     */
    std::string GetHardwareDeviceInfo(bool format = false);

    // callback function defination
    /**
     * @brief called by lifecycle manager, judge ability exist or not
     * @param key the ability name,don't include namespace
     * @return exist or not
     */
    bool AbilityExistJudge(const std::string &key);
    /**
     * @brief called by discovery manager, tell the discovery result
     * @param result
     */
    void EndAddressDiscoveryResult(const std::map<std::string, std::string> &result);
    /**
     * @brief called by ability relation manager, get the abilityinfo extract , the namespace/name will only keep name
     * @return
     */
    std::vector<AbilityInfoExtract> GetAbilityInfoExtractList();
    /**
     * @brief called by ability relation manager, get the hardware
     * @param type the hardware type
     * @return
     */
    std::vector<std::string> GetHardWareResourceList(std::string type);
    /**
     * @brief called by the connection manager, to handle the recv message received by the server
     * @param message data
     */
    void RecvMessageHandle(const std::string &message);
    /**
     * @brief insert the hardware info into the matching instance
     */
    void InsertHardwareInfo(std::map<std::string, CameraHardware> &camera,
                            std::map<std::string, AudioHardware> &mic,
                            std::map<std::string, AudioHardware> &speaker);

private:
    /**
     * @brief add nonlocal resource into the map
     * @param data the nonlocal resource
     */
    void addNonLocalResource(const std::string &data);
    /**
     * @brief  handle the cloud message
     * @param message  the cloud message
     */
    void cloudMessageHandle(const KeyAndDataPackages &data);
    /**
     * @brief handle the end message
     * @param message data
     */
    void endMessageHandle(const KeyAndDataPackages &data);
    /**
     * @brief generate nonlocal format data
     * @return the result
     */
    std::string generateNonLocalFormat();
    /**
     * @brief generate key-value format data
     * @return
     */
    std::string generateKVFormat();
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
    std::shared_ptr<HardwareResourceManager> hardware_manager_;

    std::map<std::string, std::string> versionRecord; // record the key and the version
    std::map<std::string, std::shared_ptr<DeviceInstanceInfo>> devices_;
    std::map<std::string, std::shared_ptr<AbilityInstanceInfo>> abilities_;
    std::map<std::string, std::shared_ptr<NonLocalResource>> nonlocal_instances_;

    std::mutex nonlocal_lock_;
    std::mutex abilities_lock_;
    std::mutex devices_lock_;

    std::string hostname_;

    std::thread endsyncThread_;
    bool endsync_running = false;
    std::thread cloudsyncThread_;
    bool cloudsync_running = false;
};

#endif // _RESOURCE_MANAGER_H