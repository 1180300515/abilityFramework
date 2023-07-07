#include "database_manager.h"

bool DatabaseManager::RegisterCrd(const std::string &filepath)
{
    return false;
}

bool DatabaseManager::AddAbilityInstance(const std::string &filepath)
{
    return false;
}

bool DatabaseManager::AddDeviceInstance(const std::string &filepath)
{
    return false;
}

bool DatabaseManager::DBGetDeviceInstances(std::string resourcetype, std::map<std::string, std::string> &instance)
{
    return false;
}

bool DatabaseManager::DBGetAbilityInstances(std::map<std::string, std::string> &instance)
{
    return false;
}

bool DatabaseManager::DBStoreDeviceInstances(std::string data)
{
    return false;
}

bool DatabaseManager::DBUpdateDeviceInstance(std::string &data)
{
    return false;
}

bool DatabaseManager::DBDelteDeviceInstance(std::string &key)
{
    return false;
}

bool DatabaseManager::DBStoreCloudAddress()
{
    return false;
}

bool DatabaseManager::DBGetCloudAddress()
{
    return false;
}

bool DatabaseManager::DBCleanDeviceInstances()
{
    return false;
}

bool DatabaseManager::DBCleanAbility()
{
    return false;
}

bool DatabaseManager::DBCleanCRD()
{
    return false;
}

int DatabaseManager::crd_callback(void *unused, int columenCount, char **columnValue, char **columnName)
{
    return 0;
}

int DatabaseManager::instance_callback(void *unused, int columenCount, char **columnValue, char **columnName)
{
    return 0;
}

int DatabaseManager::ability_callback(void *unused, int columenCount, char **columnValue, char **columnName)
{
    return 0;
}

int DatabaseManager::cloud_address_callback(void *unused, int columenCount, char **columnValue, char **columnName)
{
    return 0;
}
