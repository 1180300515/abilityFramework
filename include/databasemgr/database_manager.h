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
    bool DBGetDeviceInstances(std::string resourcetype, std::map<std::string, std::string> &instance);
    bool DBGetAbilityInstances(std::map<std::string, std::string> &instance);
    /**
     * store the instance into db
     */
    bool DBStoreDeviceInstances(std::string data);
    /**
     * update the instance
     */
    bool DBUpdateDeviceInstance(std::string &data);
    /**
     * delete the instance
     */
    bool DBDelteDeviceInstance(std::string &key);
    /**
     * store the cloud address into db
     */
    bool DBStoreCloudAddress();
    /**
     * read cloud address from db
     */
    bool DBGetCloudAddress();
    /**
     * clean db
     */
    bool DBCleanDeviceInstances();
    bool DBCleanAbility();
    bool DBCleanCRD();

private:
    static std::vector<CrdDBStruct> crdstructs;
    static std::vector<InstanceDBStruct> instancestructs;
    static std::vector<AbilityDBStruct> abilitystructs;

    // the database
    sqlite3 *db;

    // the callback func of each table
    static int crd_callback(void *unused, int columenCount, char **columnValue, char **columnName);
    static int instance_callback(void *unused, int columenCount, char **columnValue, char **columnName);
    static int ability_callback(void *unused, int columenCount, char **columnValue, char **columnName);
    static int cloud_address_callback(void *unused, int columenCount, char **columnValue, char **columnName);

    DatabaseManager()=default;
};

#endif // DATABASE_MANAGER_H