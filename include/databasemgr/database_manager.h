#ifndef _DATABASE_MANAGER_H
#define _DATABASE_MANAGER_H

#include <string>
#include <map>
#include <vector>

#include <sqlite3.h>
#include "glog/logging.h"
#include "json/json.h"

#include "database_store_struct.h"

class DatabaseManager
{
public:
    static DatabaseManager &getInstance()
    {
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
    void Init(std::string hostname);
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
    // /**
    //  * read file into the ability table
    //  *
    //  * @param filepath: the file path
    //  *
    //  * @return whether success
    //  */
    // bool AddAbilityInstance(const std::string &filepath);
    // /**
    //  * read file into the instance table
    //  *
    //  * @param filepath: the file path
    //  * @return success or not
    //  */
    // bool AddDeviceInstance(const std::string &filepath);
    /**
     * read instance into the memory
     */
    bool DBGetDeviceInstances(const std::string &resourcetype, std::map<std::string, std::string> &instance);
    bool DBGetAbilityInstances(std::map<std::string, std::string> &instance);
    bool DBGetCloudAddress(std::string &cloudaddress);
    /**
     * store the instance
     */
    bool DBStoreAbilityInstance(Json::Value &instance_json);
    bool DBStoreDeviceInstance(Json::Value &instance_json);

    /**
     * @brief store the cloud address into db , will delete the old address , insert new address
     * @param cloudaddress
     * @return
     */
    bool DBStoreCloudAddress(const std::string &cloudaddress);
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

private:
    // 暂存回调函数处理的值
    static std::vector<CrdDBStruct> crdstructs;
    static std::vector<InstanceDBStruct> devicestructs;
    static std::vector<InstanceDBStruct> abilitystructs;
    static std::string cloud_address;
    static std::string host_name;

    // the database
    sqlite3 *db;

    // the callback func of each table
    static int crd_callback(void *unused, int columenCount, char **columnValue, char **columnName);
    static int device_callback(void *unused, int columenCount, char **columnValue, char **columnName);
    static int ability_callback(void *unused, int columenCount, char **columnValue, char **columnName);
    static int cloudaddress_callback(void *unused, int columenCount, char **columnValue, char **columnName);
    static int hostname_callback(void *unused, int columenCount, char **columnValue, char **columnName);

    DatabaseManager();
};

#endif // _DATABASE_MANAGER_H