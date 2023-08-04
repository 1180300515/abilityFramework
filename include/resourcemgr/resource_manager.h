#ifndef _RESOURCE_MANAGER_H
#define _RESOURCE_MANAGER_H

#include <mutex>
#include <memory>
#include <map>
#include <thread>

#include "device_instance_info.h"
#include "ability_instance_info.h"
#include "hardware_scan.h"
#include "message_package_struct.h"
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
     * @brief add ability instance (database and memory)
     * @param data the instance data or the file path
     * @param from_file from file or not
     * @return success or not
     */
    bool AddAbilityInstance(const std::string &data, bool from_file = false);
    /**
     * @brief update the ability instance (database and memory)
     * @param data the data
     * @return success or not
     */
    bool UpdateAbilityInstance(const std::string &data);
    /**
     * @brief delete the ability instance (database and memory)
     * @param key the ability key
     * @return success or not
     */
    bool DeleteAbilityInstance(const std::string &key);
    /**
     * @brief add a device instance (also called by the hardwarescan the add a device)
     * @param data the file path or the device data
     * @param from_file from file or not
     * @return success or not
     */
    bool AddDeviceInstance(const std::string &data, bool from_file = false);
    /**
     * @brief update device instance (database and memory)
     * @param data the data
     * @return success or not
     */
    bool UpdateDeviceInstance(const std::string &data);
    /**
     * @brief delete the device instance (database and memory)
     * @param key the device key
     * @return success or not
     */
    bool DeleteDeviceInstance(const std::string &key);
    /**
     * @brief load data from the database
     */
    void LoadLocalResourceFromDB();
    /**
     * @brief judge the key resource type
     * @param key
     * @return
     */
    std::string isLocalResource(std::string key);

    /**
     * @brief init the resource manager
     */
    void Init(bool cleandb);
    /**
     * @brief start the resource manager module
     */
    void Run();
    void Wait();
    /**
     * @brief get the local hardware device json string
     * @param format format string or not
     * @return the json string
     */
    std::string GetHardwareDeviceInfo(bool format = false);

    std::shared_ptr<DeviceInstanceInfo> GetDeviceInstance(const std::string &key);
    std::shared_ptr<AbilityInstanceInfo> GetAbilityInstance(const std::string &key);

    void Print();

    // callback function defination
    /**
     * @brief called by lifecycle manager, judge ability exist or not
     * @param key the ability name,don't include namespace
     * @return exist or not
     */
    bool AbilityExistJudge(const std::string &key);
    /**
     * @brief called by discovery manager, tell resource manager about discovery result
     * @param result the discovery result (not include the local hostname)
     */
    void EndAddressDiscoveryResult(const std::map<std::string, std::string> &result);
    /**
     * @brief called by ability relation manager, get the abilityinfo extract , the namespace/name will only keep name
     * @return the list
     */
    std::vector<AbilityInfoExtract> GetAbilityInfoExtractList();
    /**
     * @brief called by ability relation manager, get the corresponding type of hardware information
     * @param type the hardware type
     * @return the info
     */
    std::vector<std::string> GetHardWareResourceList(std::string type);
    /**
     * @brief insert the hardware info into the matching instance
     */
    void InsertHardwareInfo(const std::map<std::string, CameraHardware> &camera,
                            const std::map<std::string, AudioHardware> &mic,
                            const std::map<std::string, AudioHardware> &speaker,
                            std::map<std::string, std::string> &record);

private:
    /**
     * @brief get the host name
     */
    void getHostName();

    bool addDeviceInstance(const Json::Value &instance_json);

    bool deleteDeviceInstance(const std::string &key);

    std::shared_ptr<HardwareScan> hardware_;
    std::shared_ptr<HardwareResourceManager> hardware_manager_;

    std::map<std::string, std::shared_ptr<DeviceInstanceInfo>> devices_;
    std::map<std::string, std::shared_ptr<AbilityInstanceInfo>> abilities_;
    std::mutex abilities_lock_;
    std::mutex devices_lock_;

    std::string hostname_;

    friend class HardwareScan;
};

#endif // _RESOURCE_MANAGER_H