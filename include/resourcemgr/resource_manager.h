/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: resource_manager.h
 * Description: resource_manager.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef RESOURCEMGR_RESOURCE_MANAGER_H_
#define RESOURCEMGR_RESOURCE_MANAGER_H_

#include <map>
#include <memory>
#include <mutex>  // NOLINT [build/c++11]
#include <string>
#include <thread>  // NOLINT [build/c++11]
#include <vector>

#include "abilityrelationmgr/ability_instance_info_extract.h"
#include "resourcemgr/ability_instance_info.h"
#include "resourcemgr/device_instance_info.h"
#include "resourcemgr/hardware_device_resource_manager.h"
#include "resourcemgr/hardware_scan.h"
#include "resourcemgr/message_package_struct.h"

class ResourceManager {
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
   * @brief add a device instance (also called by the hardwarescan the add a
   * device)
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
  std::shared_ptr<AbilityInstanceInfo> GetAbilityInstance(
      const std::string &key);

  void Print();
  void PrintMatchMap();

  // callback function defination
  /**
   * @brief called by lifecycle manager, judge ability exist or not
   * @param key the ability name,don't include namespace
   * @return exist or not
   */
  bool AbilityExistJudge(const std::string &key);
  /**
   * @brief called by discovery manager, tell resource manager about discovery
   * result
   * @param result the discovery result (not include the local hostname)
   */
  void EndAddressDiscoveryResult(
      const std::map<std::string, std::string> &result);
  /**
   * @brief called by ability relation manager, get the abilityinfo extract ,
   * the namespace/name will only keep name
   * @return the list
   */
  std::vector<AbilityInfoExtract> GetAbilityInfoExtractList();
  /**
   * @brief called by ability relation manager, get the corresponding type of
   * hardware information
   * @param type the hardware type
   * @return the info
   */
  std::vector<std::string> GetHardWareResourceList(std::string type);

 private:
  /**
   * @brief get the host name
   */
  void getHostName();

  void handleNamespace(Json::Value *instance_json);
  bool handleHostname(Json::Value *instance_json);

  bool deleteDeviceInstance(const std::string &key);

  void resourceMatching();

  void autoGenerateCR();

  void periodicScanThread();

  std::shared_ptr<HardwareScan> hardware_;
  std::shared_ptr<HardwareResourceManager> hardware_manager_;

  std::map<std::string, std::shared_ptr<DeviceInstanceInfo>> devices_;
  std::map<std::string, std::shared_ptr<AbilityInstanceInfo>> abilities_;
  std::mutex abilities_lock_;
  std::mutex devices_lock_;

  std::string hostname_;

  std::thread periodic_scan_thread;
};

#endif  // RESOURCEMGR_RESOURCE_MANAGER_H_
