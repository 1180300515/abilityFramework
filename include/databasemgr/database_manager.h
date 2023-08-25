/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: database_manager.h
 * Description: database_manager.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef DATABASEMGR_DATABASE_MANAGER_H_
#define DATABASEMGR_DATABASE_MANAGER_H_

#include <glog/logging.h>
#include <json/json.h>
#include <sqlite3.h>

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "databasemgr/database_store_struct.h"

class ResourceManager;
class DatabaseManager {
 public:
  static DatabaseManager &getInstance() {
    static DatabaseManager manager;
    return manager;
  }
  ~DatabaseManager() = default;
  DatabaseManager(const DatabaseManager &) = delete;
  DatabaseManager &operator=(const DatabaseManager) = delete;
  /**
   * @brief judge the database data whether match with local hostname
   * @param hostname the local hostname
   */
  void Init(std::string hostname, bool cleandb);
  /**
   * read file into the crd table.
   *
   * @param filepath: the file path
   *
   * @return whether success
   */
  bool RegistCrdFromFile(const std::string &filepath);
  bool RegistCrd(const std::string &data);
  bool UnregistCrd(const std::string &name);
  /**
   * read instance into the memory
   */
  bool DBGetDeviceInstances(const std::string &resourcetype,
                            std::map<std::string, std::string> *instance);
  bool DBGetAbilityInstances(std::map<std::string, std::string> *instance);
  /**
   * store the instance
   */
  bool DBStoreAbilityInstance(const Json::Value &instance_json);
  bool DBStoreDeviceInstance(const Json::Value &instance_json);
  /**
   * update the instance
   */
  bool DBUpdateDeviceInstance(const Json::Value &instance_json);
  bool DBUpdateAbilityInstance(const Json::Value &instance_json);
  /**
   * delete the instance
   */
  bool DBDelteDeviceInstance(const std::string &key);
  bool DBDeleteAbilityInstance(const std::string &key);
  /**
   * clean db
   */
  bool DBCleanDevice();
  bool DBCleanAbility();
  bool DBCleanCRD();
  bool DBCleanHostname();

 private:
  // the callback func of each table
  static int crd_callback(void *unused, int columenCount, char **columnValue,
                          char **columnName);
  static int device_callback(void *unused, int columenCount, char **columnValue,
                             char **columnName);
  static int ability_callback(void *unused, int columenCount,
                              char **columnValue, char **columnName);
  static int hostname_callback(void *unused, int columenCount,
                               char **columnValue, char **columnName);

  DatabaseManager();

  void loadCRD();
  bool validateJson(const Json::Value &instance_json) const;
  bool storeDevice(const Json::Value &instance_json) const;
  bool storeAbility(const Json::Value &instance_json) const;

  // 暂存回调函数处理的值
  static std::vector<CrdDBStruct> crdstructs;
  static std::vector<InstanceDBStruct> devicestructs;
  static std::vector<InstanceDBStruct> abilitystructs;
  static char host_name[256];

  // the database
  sqlite3 *db;
  std::unordered_map<std::string, CrdDBStruct> regist_crd_list;

  friend class ResourceManager;
};

#endif  // DATABASEMGR_DATABASE_MANAGER_H_
