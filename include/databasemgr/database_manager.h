#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <map>
#include <vector>

#include <sqlite3.h>

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
     * read file into the crd table.
     *
     * @param filepath: the file path
     *
     * @return whether success
     */
    bool RegisterCrd(const std::string &filepath);

    /**
     * read file into the ability table
     *
     * @param filepath: the file path
     *
     * @return whether success
     */
    bool AddAbilityInstance(const std::string &filepath);
    /**
     * read file into the instance table
     *
     * @param filepath: the file path
     * @return success or not
     */
    bool AddDeviceInstance(const std::string &filepath);
    /**
     * read instance into the memory
     */
    bool DBGetDeviceInstances(const std::string &resourcetype, std::map<std::string, std::string> &instance);
    /**
     * read ability into memory
    */
    bool DBGetAbilityInstances(std::map<std::string, std::string> &instance);
    /**
     * read cloud address from db
     */
    bool DBGetCloudAddress(std::string &cloudaddress);

    bool DBStoreAbilityInstance(const std::string &data);
    /**
     * store the instance into db
     */
    bool DBStoreDeviceInstance(const std::string &data);
    /**
     * store the cloud address into db
     */
    bool DBStoreCloudAddress(const std::string &cloudaddress);
    /**
     * update the instance
     */
    bool DBUpdateDeviceInstance(const std::string &data);
    /**
     * update the ability
    */
    bool DBUpdateAbilityInstance(const std::string &data);
    /**
     * delete the instance
     */
    bool DBDelteDeviceInstance(const std::string &key);
    
    
    /**
     * clean db
     */
    bool DBCleanDeviceInstances();
    bool DBCleanAbility();
    bool DBCleanCRD();

private:
    static std::vector<CrdDBStruct> crdstructs;
    static std::vector<InstanceDBStruct> devicestructs;
    static std::vector<InstanceDBStruct> abilitystructs;
    static std::string cloud_address;

    // the database
    sqlite3 *db;

    // the callback func of each table
    static int crd_callback(void *unused, int columenCount, char **columnValue, char **columnName);
    static int device_callback(void *unused, int columenCount, char **columnValue, char **columnName);
    static int ability_callback(void *unused, int columenCount, char **columnValue, char **columnName);
    static int cloud_address_callback(void *unused, int columenCount, char **columnValue, char **columnName);

    DatabaseManager() = default;
};

#endif // DATABASE_MANAGER_H