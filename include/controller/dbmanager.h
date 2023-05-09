#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <sqlite3.h>
#include <string>
#include <memory>

#include "controller/common/ability_struct.h"
#include "controller/common/cameradevice_instance.h"
#include "controller/common/sensordevice_instance.h"
#include "controller/common/louspeakerdevice_instance.h"
#include "controller/common/database_store_struct.h"

class dbManager
{
private:
    std::string crd_table_name = "CRD";
    std::string instance_table_name = "INSTANCE";
    std::string ability_table_name = "ABILITY";
    std::string level1_table_name = "LEVEL1";
    std::string level2_table_name = "LEVEL2";
    std::string level3_table_name = "LEVEL3";

    // store the sql research result
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
    // static int level1_callback(void *unused,int columenCount,char **columnValue,char **columnName);
    // static int level2_callback(void *unused,int columenCount,char **columnValue,char **columnName);
    // static int level3_callback(void *unused,int columenCount,char **columnValue,char **columnName);

private:
    dbManager();

public:
    // singleton
    static dbManager &getInstance()
    {
        static dbManager manager;
        return manager;
    }
    ~dbManager() = default;
    dbManager(const dbManager &) = delete;
    dbManager &operator=(const dbManager) = delete;
    /**
     * 从文件读取数据，存储crd到数据库
     *
     * @param filepath: the file path
     *
     * @return whether success
     */
    bool RegisterCrd(const std::string &filepath);

    /**
     * 从文件读取数据，存储ability进入数据库
     *
     * @param filepath: the file path
     *
     * @return whether success
     */
    bool AddAbilityInstance(const std::string &filepath);
    /**
     * 从文件读取数据，存储device instance进入数据库
     *
     * @param filepath: the file path
     * @return success or not
     */
    bool AddDeviceInstance(const std::string &filepath);
    /**
     * 读取instance到结构体中
     */
    bool DBGetDeviceInstances(std::map<std::string, std::shared_ptr<CameraInstance>> &instance);
    bool DBGetDeviceInstances(std::map<std::string, std::shared_ptr<LoudspeakerInstance>> &instance);
    bool DBGetDeviceInstances(std::map<std::string, std::shared_ptr<SensorInstance>> &instance);   
    /**
     * 存储资源到数据库中
    */
    bool DBStoreDeviceInstances(std::string data);
    /**
     * 数据库更新device instance
    */
    bool DBUpdateDeviceInstance(std::string &data);
    /**
     * 数据库删除device instance
    */
    bool DBDelteDeviceInstance(std::string &key);
    /**
     * 存储云端地址进入数据库
    */
    bool DBStoreCloudAddress();
    /**
     * 从数据库中读取云端地址
    */
    bool DBGetCloudAddress();
    /**
     * 清除数据库中的对应数据
    */
    bool DBCleanDeviceInstances();
    bool DBCleanAbility();
    bool DBCleanCRD();

    // // 以下弃用
    // /**
    //  * delete a instance from the instance table
    //  *
    //  * @param filepath: the file path
    //  * @return success or not
    //  */
    // bool DeleteDeviceInstance(const std::string &filepath);
    // /**
    //  * search a instance by the key
    //  *
    //  * @param key the instance name in metadata
    //  * @param value the result struct
    //  * @return success or not
    //  */
    // bool SearchDeviceInstance(const std::string &key, InstanceDBStruct &value);
    // /**
    //  * get the Camera into the struct
    //  *
    //  * @param key the value of metadata.name
    //  * @param instance the struct of Camera
    //  * @return success or not
    //  */
    // bool GetCameraInstance(const std::string &key, CameraInstance &instance);
    // /**
    //  * unregist ability and device crd
    //  */
    // bool UnRegisterCrd(const std::string &filepath);
    // /**
    //  * delete a abiity
    //  *
    //  * @param filepath: the file path
    //  * @return delete success or not
    //  */
    // bool DeleteAbilityInstance(const std::string &filepath);
    // /**
    //  * search ability by name
    //  *
    //  * @param name ability name
    //  * @param value the result struct
    //  * @return success or not
    //  */
    // bool SearchAbility(const std::string &name, AbilityDBStruct &value);
    // /**
    //  * get the ability into the class
    //  */
    // bool GetAbilityInstance(const std::string &name, Ability &ability);
    // /**
    //  * refresh a ability
    //  *
    //  * @param filepath the file path
    //  * @return success or not
    //  */
    // bool RefreshAbilityInstance(const std::string &filepath);

    // /**
    //  * get the sensor into the struct
    //  *
    //  * @param key the value of metadata.name
    //  * @param instance the struct of Sensor
    //  * @return success or not
    //  */
    // bool GetSensorInstance(const std::string &key, SensorInstance &instance);
    // /**
    //  * get the loudspeaker into the struct
    //  *
    //  * @param key the value of metadata.name
    //  * @param instance the struct of Loudspeaker
    //  * @return success or not
    //  */
    // bool GetLoudspeakerInstance(const std::string &key, LoudspeakerInstance &instance);
    // /**
    //  * refresh a instance
    //  *
    //  * @param filepath the file path
    //  */
    // bool RefreshDeviceInstance(const std::string &filepath);

    
};

#endif