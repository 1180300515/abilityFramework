#include <iostream>
#include <unistd.h>

#include "glog/logging.h"
#include "yaml-cpp/yaml.h"
#include "json/json.h"
#include "controller/mongoose/mongoose.h"

#include "controller/json_yaml_convert.h"
#include "controller/global_var.h"
#include "controller/dbmanager.h"

dbManager::dbManager()
{
    int rc;
    char *errormes = 0;
    rc = sqlite3_open(DATABASE_FILE_PATH, &db);
    if (rc)
    {
        LOG(ERROR) << "open database in :" << DATABASE_FILE_PATH << "failed";
        exit(0);
    }
    else
    {
        if (!access(DATABASE_FILE_PATH, R_OK))
        {
            LOG(INFO) << "db exist,open success";
        }
        else
        {
            LOG(INFO) << "db not exist,create new one,open success";
        }
    }
    // create related table
    const char *crd_table_sql;
    const char *instance_table_sql;
    const char *ability_table_sql;
    const char *cloud_address_table_sql;

    crd_table_sql = "CREATE TABLE IF NOT EXISTS CRD ( "
                    "KEY TEXT PRIMARY KEY NOT NULL, "
                    "VALUE TEXT NOT NULL, "
                    "APIGROUP TEXT NOT NULL, "
                    "KIND TEXT NOT NULL, "
                    "SCHEMA TEXT NOT NULL );";
    instance_table_sql = "CREATE TABLE IF NOT EXISTS INSTANCE ( "
                         "NAME TEXT PRIMARY KEY     NOT NULL,"
                         "NAMESPACE TEXT   NOT NULL,"
                         "VALUE TEXT   NOT NULL,"
                         "APIGROUP TEXT   NOT NULL,"
                         "KIND  TEXT   NOT NULL,"
                         "VERSION TEXT NOT NULL);";
    ability_table_sql = "CREATE TABLE IF NOT EXISTS ABILITY ( "
                        "NAME TEXT PRIMARY KEY     NOT NULL,"
                        "NAMESPACE TEXT   NOT NULL,"
                        "VALUE TEXT   NOT NULL,"
                        "APIGROUP TEXT   NOT NULL,"
                        "KIND  TEXT   NOT NULL,"
                        "VERSION TEXT NOT NULL);";
    cloud_address_table_sql = "CREATE TABLE IF NOT EXISTS CLOUDADDRESS ( ADDRESS TEXT PRIMARY KEY   NOT NULL );";

    rc = sqlite3_exec(db, crd_table_sql, crd_callback, 0, &errormes);
    if (rc)
    {
        LOG(ERROR) << "create table CRD fail with sql command: " << crd_table_sql << "\nerror message : " << errormes;
        exit(0);
    }
    rc = sqlite3_exec(db, instance_table_sql, instance_callback, 0, &errormes);
    if (rc)
    {
        LOG(ERROR) << "create table INSTANCE failed with: " << errormes;
        exit(0);
    }
    rc = sqlite3_exec(db, ability_table_sql, ability_callback, 0, &errormes);
    if (rc)
    {
        LOG(ERROR) << "create table ABILITY failed with: " << errormes;
        exit(0);
    }
    rc = sqlite3_exec(db, cloud_address_table_sql, cloud_address_callback, 0, &errormes);
    if (rc)
    {
        LOG(ERROR) << "create table CLOUDADDRESS failed with: " << errormes;
        exit(0);
    }
    LOG(INFO) << "create table CRD,INSTANCE.ABILITY,CLOUDADDRESS success";
    profile = getLocalHWInfo();
    LOG(INFO) << "get local hardware success";
}

void dbManager::insertCameraInfo(Json::Value &jnode)
{
    CameraHardware a = profile.cameraDevices.front();
    profile.cameraDevices.erase(profile.cameraDevices.begin());
    jnode["spec"]["properties"]["deviceNode"] = a.device_path;
    jnode["spec"]["properties"]["driverName"] = a.driver;
    jnode["spec"]["properties"]["cardType"] = a.card;
    jnode["spec"]["properties"]["busInfo"] = a.bus_info;
    jnode["spec"]["properties"]["description"] = "ignore";
    for (int i = 0; i < a.formats.size(); i++)
    {
        jnode["spec"]["properties"]["supportFormat"].append(a.formats[i]);
    }
}
void dbManager::insertMicInfo(Json::Value &jnode)
{
    AudioDevice a = profile.micDevices.front();
    profile.micDevices.erase(profile.micDevices.begin());
    jnode["spec"]["properties"]["hardwareName"] = a.name;
    jnode["spec"]["properties"]["sampleRates"] = std::to_string(a.sampleRate);
    jnode["spec"]["properties"]["volume"] = (int)a.volume;
    jnode["spec"]["properties"]["mute"] = a.mute;
    jnode["spec"]["properties"]["description"] = a.description;
}
void dbManager::insertloudspeakerInfo(Json::Value &jnode)
{
    AudioDevice a = profile.speakerDevices.front();
    profile.speakerDevices.erase(profile.speakerDevices.begin());
    jnode["spec"]["properties"]["hardwareName"] = a.name;
    jnode["spec"]["properties"]["sampleRates"] = std::to_string(a.sampleRate);
    jnode["spec"]["properties"]["volume"] = (int)a.volume;
    jnode["spec"]["properties"]["mute"] = a.mute;
    jnode["spec"]["properties"]["description"] = a.description;
}
// static member init
std::vector<CrdDBStruct> dbManager::crdstructs;
std::vector<InstanceDBStruct> dbManager::instancestructs;
std::vector<AbilityDBStruct> dbManager::abilitystructs;

bool dbManager::RegisterCrd(const std::string &filepath)
{
    YAML::Node crd_yaml;
    Json::Value crd_json;
    try
    {
        crd_yaml = YAML::LoadFile(filepath);
    }
    catch (YAML::ParserException e)
    {
        LOG(ERROR) << "yaml is malformed.";
        return false;
    }
    catch (YAML::BadFile e)
    {
        LOG(ERROR) << "file can't be load";
        return false;
    }
    if (!YamlToJson(crd_yaml, crd_json))
    {
        LOG(ERROR) << "convert the yaml file in: " << filepath << "error";
        return false;
    }
    // get related value
    std::string crd_key_name = crd_json["metadata"]["name"].asString();
    std::string crd_json_string = JsonToString(crd_json);
    std::string crd_group_string = crd_json["spec"]["group"].asString();
    std::string crd_kind_string = crd_json["spec"]["names"]["kind"].asString();
    std::string crd_schema_string = GetCrdSchema(crd_json);

    std::string sql = "INSERT OR IGNORE INTO CRD (KEY,VALUE,APIGROUP,KIND,SCHEMA) VALUES (\'";
    sql += crd_key_name + "\',\'" + crd_json_string + "\',\'" + crd_group_string + "\',\'" + crd_kind_string + "\',\'" + crd_schema_string + "\' );";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, sql.c_str(), crd_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    LOG(INFO) << "register success with crd name: " << crd_key_name << " in file: " << filepath;
    return true;
}

bool dbManager::AddAbilityInstance(const std::string &filepath)
{
    YAML::Node instance_yaml;
    Json::Value instance_json;
    try
    {
        instance_yaml = YAML::LoadFile(filepath);
    }
    catch (YAML::ParserException e)
    {
        LOG(ERROR) << "yaml is malformed.";
        return false;
    }
    catch (YAML::BadFile e)
    {
        LOG(ERROR) << "file can't be load";
        return false;
    }
    if (!YamlToJson(instance_yaml, instance_json))
    {
        LOG(ERROR) << "convert the yaml file in: " << filepath << " to json format fail";
        return false;
    }

    // check the crd is exist or not in the crd table
    std::string apiVersion_string = instance_json["apiVersion"].asString();
    int flag = apiVersion_string.find("/");
    std::string instance_group = apiVersion_string.substr(0, flag);
    std::string instance_kind = instance_json["kind"].asString();

    crdstructs.clear();
    std::string search_sql = "SELECt * FROM CRD WHERE APIGROUP = \'" + instance_group + "\' AND KIND = \'" + instance_kind + "\' ;";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, search_sql.c_str(), crd_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    if (crdstructs.empty())
    {
        LOG(INFO) << "no matched crd registe,please registe before add instance";
        return false;
    }
    LOG(INFO) << "find matched crd with group: " << instance_group << ", kind: " << instance_kind;

    // json schema
    std::string crd_schema = crdstructs.begin()->schema;
    std::string instance_validate = GetAbilityValidatePart(instance_json);
    if (SchemaValidation(crd_schema, instance_validate))
    {
        LOG(INFO) << "the instance in file: " << filepath << " pass the validate ";
    }
    else
    {
        LOG(INFO) << "validate fail about the file in: " << filepath;
        return false;
    }
    // add instance into the ability table
    std::string instance_key = instance_json["abilityname"].asString();
    std::string instance_value = JsonToString(instance_json);
    std::string instance_version = apiVersion_string.substr(flag);
    std::string instance_namespace = "default";
    if (instance_json["metadata"].isMember("namespace"))
    {
        std::string instance_namespace = instance_json["metadata"]["namespace"].asString();
    }

    std::string insert_sql = "INSERT OR IGNORE INTO ABILITY (NAME,NAMESPACE,VALUE,APIGROUP,KIND,VERSION) VALUES (\'";
    insert_sql += instance_key + "\',\'" + instance_namespace + "\',\'" + instance_value + "\',\'" + instance_group + "\',\'" + instance_kind + "\',\'" + instance_version + "\' );";
    int rc2;
    char *erroms2 = 0;
    rc2 = sqlite3_exec(db, insert_sql.c_str(), ability_callback, 0, &erroms2);
    if (rc2)
    {
        LOG(ERROR) << "sql excute error with: " << erroms2;
        return false;
    }
    LOG(INFO) << "add instance in file: " << filepath << " success";
    return true;
}

bool dbManager::AddDeviceInstance(const std::string &filepath)
{
    YAML::Node instance_yaml;
    Json::Value instance_json;
    try
    {
        instance_yaml = YAML::LoadFile(filepath);
    }
    catch (YAML::ParserException e)
    {
        LOG(ERROR) << "yaml is malformed.";
        return false;
    }
    catch (YAML::BadFile e)
    {
        LOG(ERROR) << "file can't be load";
        return false;
    }
    if (!YamlToJsonForInstance(instance_yaml, instance_json))
    {
        LOG(ERROR) << "convert the yaml file in: " << filepath << " to json format fail";
        return false;
    }
    // check the crd is exist or not in the crd table
    std::string apiVersion_string = instance_json["apiVersion"].asString();
    int flag = apiVersion_string.find("/");
    std::string instance_group = apiVersion_string.substr(0, flag);
    std::string instance_kind = instance_json["kind"].asString();
    crdstructs.clear();
    std::string search_sql = "SELECT * FROM CRD WHERE APIGROUP = \'" + instance_group + "\' AND KIND = \'" + instance_kind + "\' ;";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, search_sql.c_str(), crd_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    if (crdstructs.empty())
    {
        LOG(INFO) << "no matched crd registe,please registe before add instance";
        return false;
    }
    LOG(INFO) << "find matched crd with group: " << instance_group << ", kind: " << instance_kind;

    // json schema
    // std::string crd_schema = CrdSchemaRevise(crdstructs.begin()->schema);
    std::string crd_schema = crdstructs.begin()->schema;
    std::string instance_validate = GetInstanceValidatePart(instance_json);
    if (SchemaValidation(crd_schema, instance_validate))
    {
        LOG(INFO) << "the instance in file: " << filepath << " pass the validate ";
    }
    else
    {
        LOG(INFO) << "validate fail about the file in: " << filepath;
        return false;
    }
    // add instance into the instance table
    std::string instance_key = instance_json["metadata"]["name"].asString();
    std::string instance_namespace = "default";
    if (instance_json["metadata"].isMember("namespace"))
    {
        std::string instance_namespace = instance_json["metadata"]["namespace"].asString();
    }
    if (instance_json["spec"].isMember("hostname"))
    {
        if (instance_json["spec"]["hostname"] != device_hostname && instance_json["spec"]["hostname"] != "")
        {
            LOG(ERROR) << "the " << instance_json["spec"]["hostname"] << " is not the local hostname";
            return false;
        }
        else
        {
            instance_json["spec"]["hostname"] = device_hostname;
        }
    }
    if (instance_kind == CameraDeviceResourcetype)
    {
        insertCameraInfo(instance_json);
    }
    else if (instance_kind == MicrophoneDeviceResourcetype)
    {
        insertMicInfo(instance_json);
    }
    else if (instance_kind == LoudspeakerDeviceResourcetype)
    {
        insertloudspeakerInfo(instance_json);
    }

    std::string instance_value = JsonToString(instance_json);
    std::string instance_version = apiVersion_string.substr(flag + 1, apiVersion_string.size() - flag + 1);

    std::string insert_sql = "INSERT OR IGNORE INTO INSTANCE (NAME,NAMESPACE,VALUE,APIGROUP,KIND,VERSION) VALUES (\'";
    insert_sql += instance_key + "\',\'" + instance_namespace + "\',\'" + instance_value + "\',\'" + instance_group + "\',\'" + instance_kind + "\',\'" + instance_version + "\' );";
    int rc2;
    char *erroms2 = 0;
    rc2 = sqlite3_exec(db, insert_sql.c_str(), instance_callback, 0, &erroms2);
    if (rc2)
    {
        LOG(ERROR) << "sql excute error with: " << erroms2;
        return false;
    }
    LOG(INFO) << "add instance in file: " << filepath << " success";

    return true;
}

bool dbManager::DBGetDeviceInstances(std::map<std::string, std::shared_ptr<CameraInstance>> &instance)
{
    instancestructs.clear();
    std::string search_sql = "SELECT * FROM INSTANCE WHERE KIND = \'CameraDevice\' ;";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, search_sql.c_str(), instance_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    for (int i = 0; i < instancestructs.size(); i++)
    {
        auto ins = std::make_shared<CameraInstance>();
        ins->UnMarshal(instancestructs[i].value);
        std::string key = instancestructs[i].namespace_name + "/" + instancestructs[i].name;
        instance[key] = ins;
    }
    return true;
}

bool dbManager::DBGetDeviceInstances(std::map<std::string, std::shared_ptr<LoudspeakerInstance>> &instance)
{
    instancestructs.clear();
    std::string search_sql = "SELECT * FROM INSTANCE WHERE KIND = \'LoudspeakerDevice\' ;";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, search_sql.c_str(), instance_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    for (int i = 0; i < instancestructs.size(); i++)
    {
        auto ins = std::make_shared<LoudspeakerInstance>();
        ins->UnMarshal(instancestructs[i].value);
        std::string key = instancestructs[i].namespace_name + "/" + instancestructs[i].name;
        instance[key] = ins;
    }
    return true;
}

bool dbManager::DBGetDeviceInstances(std::map<std::string, std::shared_ptr<SensorInstance>> &instance)
{
    instancestructs.clear();
    std::string search_sql = "SELECT * FROM INSTANCE WHERE KIND = \'SensorDevice\' ;";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, search_sql.c_str(), instance_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    for (int i = 0; i < instancestructs.size(); i++)
    {
        auto ins = std::make_shared<SensorInstance>();
        ins->UnMarshal(instancestructs[i].value);
        std::string key = instancestructs[i].namespace_name + "/" + instancestructs[i].name;
        instance[key] = ins;
    }
    return true;
}

bool dbManager::DBGetDeviceInstances(std::map<std::string, std::shared_ptr<MicrophoneInstance>> &instance)
{
    instancestructs.clear();
    std::string search_sql = "SELECT * FROM INSTANCE WHERE KIND = \'MicrophoneDevice\' ;";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, search_sql.c_str(), instance_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    for (int i = 0; i < instancestructs.size(); i++)
    {
        auto ins = std::make_shared<MicrophoneInstance>();
        ins->UnMarshal(instancestructs[i].value);
        std::string key = instancestructs[i].namespace_name + "/" + instancestructs[i].name;
        instance[key] = ins;
    }
    return true;
}

bool dbManager::DBGetAbilityInstances(std::map<std::string, std::shared_ptr<Ability>> &instance)
{
    abilitystructs.clear();
    std::string search_sql = "SELECT * FROM ABILITY ;";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, search_sql.c_str(), ability_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    for (int i = 0; i < abilitystructs.size(); i++)
    {
        auto ins = std::make_shared<Ability>();
        ins->UnMarshal(abilitystructs[i].value);
        std::string key = abilitystructs[i].namespace_name + "/" + abilitystructs[i].name;
        instance[key] = ins;
    }
    return true;
}

bool dbManager::DBStoreCloudAddress()
{
    int rc;
    char *erroms = 0;
    std::string delete_sql = "DELETE FROM CLOUDADDRESS;";
    rc = sqlite3_exec(db, delete_sql.c_str(), cloud_address_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    std::string insert_sql = "INSERT OR IGNORE INTO CLOUDADDRESS ( ADDRESS ) VALUES (\'" + cloud_address + "\');";
    rc = sqlite3_exec(db, insert_sql.c_str(), cloud_address_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    LOG(INFO) << "db store cloud address : " << cloud_address;
    return true;
}

bool dbManager::DBGetCloudAddress()
{
    int rc;
    char *erroms = 0;
    std::string search_sql = "select * from CLOUDADDRESS;";
    rc = sqlite3_exec(db, search_sql.c_str(), cloud_address_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    LOG(INFO) << "db get cloud address : " << cloud_address;
    return true;
}

bool dbManager::DBUpdateDeviceInstance(std::string &data)
{
    Json::Value jnode;
    StringToJson(data, jnode);
    std::string apiVersion_string = jnode["apiVersion"].asString();
    int flag = apiVersion_string.find("/");
    std::string instance_group = apiVersion_string.substr(0, flag);
    std::string instance_version = apiVersion_string.substr(flag + 1);

    std::string instance_key = jnode["metadata"]["name"].asString();
    std::string instance_kind = jnode["kind"].asString();
    std::string instance_value = data;
    std::string instance_namespace = jnode["metadata"]["namespace"].asString();

    std::string update_sql = "UPDATE INSTANCE SET VALUE = \'" + instance_value + "\' , NAMESPACE = \'" + instance_namespace + "\' , APIGROUP = \'" + instance_group + "\', KIND = \'" + instance_kind + "\', VERSION = \'" + instance_version;
    update_sql += "\' WHERE NAME = \'" + instance_key + "\' ;";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, update_sql.c_str(), instance_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error with: " << erroms;
        return false;
    }
    LOG(INFO) << "db update instance : " << instance_key << " success";
    return true;
}

bool dbManager::DBDelteDeviceInstance(std::string &key)
{
    int index = key.find("/");
    std::string namespace_ = key.substr(0, index);
    std::string name_ = key.substr(index + 1);
    // delete the instance
    int rc;
    char *erroms = 0;
    std::string delete_sql = "DELETE FROM INSTANCE WHERE NAME = \'" + name_ + "\' AND NAMESPACE = \'" + namespace_ + "\';";
    rc = sqlite3_exec(db, delete_sql.c_str(), instance_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    LOG(INFO) << "db delete resource : " << key << " success";

    return true;
}

bool dbManager::DBStoreDeviceInstances(std::string data)
{
    Json::Value jnode;
    StringToJson(data, jnode);
    std::string apiVersion_string = jnode["apiVersion"].asString();
    int flag = apiVersion_string.find("/");
    std::string instance_group = apiVersion_string.substr(0, flag);
    std::string instance_version = apiVersion_string.substr(flag + 1);

    std::string instance_key = jnode["metadata"]["name"].asString();
    std::string instance_kind = jnode["kind"].asString();
    std::string instance_value = data;
    std::string instance_namespace = jnode["metadata"]["namespace"].asString();

    std::string insert_sql = "INSERT OR IGNORE INTO INSTANCE (NAME,NAMESPACE,VALUE,APIGROUP,KIND,VERSION) VALUES (\'";
    insert_sql += instance_key + "\',\'" + instance_namespace + "\',\'" + instance_value + "\',\'" + instance_group + "\',\'" + instance_kind + "\',\'" + instance_version + "\' );";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, insert_sql.c_str(), instance_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error with: " << erroms;
        return false;
    }
    LOG(INFO) << "db add instance : " << instance_key << " success";
    return true;
}

bool dbManager::DBCleanDeviceInstances()
{
    int rc;
    char *erroms = 0;
    std::string delete_sql = "DELETE FROM INSTANCE;";
    rc = sqlite3_exec(db, delete_sql.c_str(), instance_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    LOG(INFO) << "db clean instance resource success";

    return true;
}

bool dbManager::DBCleanAbility()
{
    int rc;
    char *erroms = 0;
    std::string delete_sql = "DELETE FROM ABILITY;";
    rc = sqlite3_exec(db, delete_sql.c_str(), ability_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    LOG(INFO) << "db clean ability resource success";

    return true;
}

bool dbManager::DBCleanCRD()
{
    int rc;
    char *erroms = 0;
    std::string delete_sql = "DELETE FROM CRD;";
    rc = sqlite3_exec(db, delete_sql.c_str(), ability_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    LOG(INFO) << "db clean crd resource success";

    return true;
}

int dbManager::crd_callback(void *unused, int columenCount, char **columnValue, char **columnName)
{
    CrdDBStruct crd;
    for (int i = 0; i < columenCount; i++)
    {
        if (strcmp(columnName[i], "KEY") == 0)
        {
            crd.key = columnValue[i];
        }
        else if (strcmp(columnName[i], "VALUE") == 0)
        {
            crd.value = columnValue[i];
        }
        else if (strcmp(columnName[i], "APIGROUP") == 0)
        {
            crd.group = columnValue[i];
        }
        else if (strcmp(columnName[i], "KIND") == 0)
        {
            crd.kind = columnValue[i];
        }
        else if (strcmp(columnName[i], "SCHEMA") == 0)
        {
            crd.schema = columnValue[i];
        }
    }
    crdstructs.push_back(crd);
    LOG(INFO) << "crdcallback: find matched crd struct with key: " << crd.key;
    return SQLITE_OK;
}

int dbManager::instance_callback(void *unused, int columenCount, char **columnValue, char **columnName)
{
    InstanceDBStruct instance;
    for (int i = 0; i < columenCount; i++)
    {
        if (strcmp(columnName[i], "NAME") == 0)
        {
            instance.name = columnValue[i];
        }
        else if (strcmp(columnName[i], "NAMESPACE") == 0)
        {
            instance.namespace_name = columnValue[i];
        }
        else if (strcmp(columnName[i], "VALUE") == 0)
        {
            instance.value = columnValue[i];
        }
        else if (strcmp(columnName[i], "APIGROUP") == 0)
        {
            instance.group = columnValue[i];
        }
        else if (strcmp(columnName[i], "KIND") == 0)
        {
            instance.kind = columnValue[i];
        }
        else if (strcmp(columnName[i], "VERSION") == 0)
        {
            instance.version = columnValue[i];
        }
    }
    instancestructs.push_back(instance);
    LOG(INFO) << "instancecallback: find a instance with name:" << instance.name;
    return SQLITE_OK;
}

int dbManager::ability_callback(void *unused, int columenCount, char **columnValue, char **columnName)
{
    AbilityDBStruct instance;
    for (int i = 0; i < columenCount; i++)
    {
        if (strcmp(columnName[i], "NAME") == 0)
        {
            instance.name = columnValue[i];
        }
        else if (strcmp(columnName[i], "NAMESPACE") == 0)
        {
            instance.namespace_name = columnValue[i];
        }
        else if (strcmp(columnName[i], "VALUE") == 0)
        {
            instance.value = columnValue[i];
        }
        else if (strcmp(columnName[i], "APIGROUP") == 0)
        {
            instance.group = columnValue[i];
        }
        else if (strcmp(columnName[i], "KIND") == 0)
        {
            instance.kind = columnValue[i];
        }
        else if (strcmp(columnName[i], "VERSION") == 0)
        {
            instance.version = columnValue[i];
        }
    }
    abilitystructs.push_back(instance);
    LOG(INFO) << "abilitycallback: find a ability with name:" << instance.name;
    return SQLITE_OK;
}

int dbManager::cloud_address_callback(void *unused, int columenCount, char **columnValue, char **columnName)
{
    cloud_address = columnValue[0];
    return SQLITE_OK;
}

// bool dbManager::UnRegisterCrd(const std::string &filepath)
// {
//     YAML::Node crd_yaml;
//     Json::Value crd_json;
//     try
//     {
//         crd_yaml = YAML::LoadFile(filepath);
//     }
//     catch (YAML::ParserException e)
//     {
//         LOG(ERROR) << "yaml is malformed.";
//         return false;
//     }
//     catch (YAML::BadFile e)
//     {
//         LOG(ERROR) << "file can't be load";
//         return false;
//     }
//     if (!YamlToJson(crd_yaml, crd_json))
//     {
//         LOG(ERROR) << "convert the yaml file in: " << filepath << "error";
//         return false;
//     }

//     std::string crd_key = GetCrdKey(crd_json);
//     std::string delete_sql = "DELETE FROM CRD WHERE KEY = \'" + crd_key + "\';";
//     int rc;
//     char *erroms = 0;
//     rc = sqlite3_exec(db, delete_sql.c_str(), ability_callback, 0, &erroms);
//     if (rc)
//     {
//         LOG(ERROR) << "sql excute error: " << erroms;
//         return false;
//     }
//     LOG(INFO) << "delete crd: " << crd_key << " success";

//     return true;
// }

// bool dbManager::DeleteAbilityInstance(const std::string &filepath)
// {
//     YAML::Node instance_yaml;
//     Json::Value instance_json;
//     try
//     {
//         instance_yaml = YAML::LoadFile(filepath);
//     }
//     catch (YAML::ParserException e)
//     {
//         LOG(ERROR) << "yaml is malformed.";
//         return false;
//     }
//     catch (YAML::BadFile e)
//     {
//         LOG(ERROR) << "file can't be load";
//         return false;
//     }
//     if (!YamlToJson(instance_yaml, instance_json))
//     {
//         LOG(ERROR) << "convert the yaml file in: " << filepath << "error";
//         return false;
//     }

//     // check this instance is exist or not in the table
//     abilitystructs.clear();
//     std::string instance_key = GetAbilityKey(instance_json);
//     std::string search_sql = "SELECT * FROM ABILITY WHERE NAME = \'" + instance_key + "\';";
//     int rc;
//     char *erroms = 0;
//     rc = sqlite3_exec(db, search_sql.c_str(), ability_callback, 0, &erroms);
//     if (rc)
//     {
//         LOG(ERROR) << "sql excute error: " << erroms;
//     }
//     if (abilitystructs.size() == 0)
//     {
//         LOG(INFO) << "the ability: " << instance_key << " is not exist";
//         return false;
//     }

//     // delete the instance
//     std::string delete_sql = "DELETE FROM ABILITY WHERE NAME = \'" + instance_key + "\';";
//     rc = sqlite3_exec(db, delete_sql.c_str(), ability_callback, 0, &erroms);
//     if (rc)
//     {
//         LOG(ERROR) << "sql excute error: " << erroms;
//         return false;
//     }
//     LOG(INFO) << "delete ability: " << instance_key << " success";

//     return true;
// }

// bool dbManager::SearchAbility(const std::string &name, AbilityDBStruct &value)
// {
//     abilitystructs.clear();
//     std::string search_sql = "SELECT * FROM ABILITY WHERE NAME = \'" + name + "\' ;";
//     int rc;
//     char *erroms = 0;
//     rc = sqlite3_exec(db, search_sql.c_str(), ability_callback, 0, &erroms);
//     if (rc)
//     {
//         LOG(ERROR) << "sql excute error: " << erroms;
//     }
//     LOG(INFO) << "search instance success with key: " << name;
//     if (abilitystructs.size() == 0)
//     {
//         LOG(INFO) << "the ability: " << name << " is not exist";
//         return false;
//     }
//     value = abilitystructs.front();
//     return true;
// }

// bool dbManager::GetAbilityInstance(const std::string &name, Ability &ability)
// {
//     AbilityDBStruct abilitystruct;
//     if (!SearchAbility(name, abilitystruct))
//     {
//         return false;
//     }
//     UnMarshal(abilitystruct.value, ability);
//     return true;
// }

// bool dbManager::RefreshAbilityInstance(const std::string &filepath)
// {
//     YAML::Node instance_yaml;
//     Json::Value instance_json;
//     try
//     {
//         instance_yaml = YAML::LoadFile(filepath);
//     }
//     catch (YAML::ParserException e)
//     {
//         LOG(ERROR) << "yaml is malformed.";
//         return false;
//     }
//     catch (YAML::BadFile e)
//     {
//         LOG(ERROR) << "file in: " << filepath << "can't be load";
//         return false;
//     }
//     if (!YamlToJson(instance_yaml, instance_json))
//     {
//         LOG(ERROR) << "convert the yaml file in: " << filepath << "error";
//         return false;
//     }

//     // check the instance is exist or not
//     abilitystructs.clear();
//     std::string instance_key = GetAbilityKey(instance_json);
//     std::string search_sql = "SELECT * FROM ABILITY WHERE NAME = \'" + instance_key + "\';";
//     int rc;
//     char *erroms = 0;
//     rc = sqlite3_exec(db, search_sql.c_str(), ability_callback, 0, &erroms);
//     if (rc)
//     {
//         LOG(ERROR) << "sql excute error with: " << erroms;
//     }
//     if (abilitystructs.size() == 0)
//     {
//         LOG(INFO) << "the instance: " << instance_key << " is not exist";
//         return false;
//     }

//     // update the instance
//     std::string apiVersion_string = GetAbilityGroupAndVersion(instance_json);
//     int flag = apiVersion_string.find("/");
//     std::string instance_group = apiVersion_string.substr(flag);
//     std::string instance_kind = GetAbilityKind(instance_json);
//     std::string instance_version = apiVersion_string.substr(flag + 1, apiVersion_string.size() - flag + 1);
//     std::string instance_value = JsonToString(instance_json);

//     std::string update_sql = "UPDATE ABILITY SET VALUE = \'" + instance_value + "\' , APIGROUP = \'" + instance_group + "\', KIND = \'" + instance_kind + "\', VERSION = \'" + instance_version;
//     update_sql += "\' WHERE NAME = \'" + instance_key + "\' ;";
//     rc = sqlite3_exec(db, update_sql.c_str(), ability_callback, 0, &erroms);
//     if (rc)
//     {
//         LOG(ERROR) << "sql excute error with: " << erroms;
//         return false;
//     }
//     LOG(INFO) << "update ability success with key: " << instance_key;
//     return true;
// }

// bool dbManager::DeleteDeviceInstance(const std::string &filepath)
// {
//     YAML::Node instance_yaml;
//     Json::Value instance_json;
//     try
//     {
//         instance_yaml = YAML::LoadFile(filepath);
//     }
//     catch (YAML::ParserException e)
//     {
//         LOG(ERROR) << "yaml is malformed.";
//         return false;
//     }
//     catch (YAML::BadFile e)
//     {
//         LOG(ERROR) << "file can't be load";
//         return false;
//     }
//     if (!YamlToJson(instance_yaml, instance_json))
//     {
//         LOG(ERROR) << "convert the yaml file in: " << filepath << "error";
//         return false;
//     }

//     // check this instance is exist or not in the table
//     instancestructs.clear();
//     std::string instance_key = GetInstanceKey(instance_json);
//     std::string search_sql = "SELECT * FROM INSTANCE WHERE NAME = \'" + instance_key + "\';";
//     int rc;
//     char *erroms = 0;
//     rc = sqlite3_exec(db, search_sql.c_str(), instance_callback, 0, &erroms);
//     if (rc)
//     {
//         LOG(ERROR) << "sql excute error: " << erroms;
//     }
//     if (instancestructs.size() == 0)
//     {
//         LOG(INFO) << "the instance: " << instance_key << " is not exist";
//         return false;
//     }

//     // delete the instance
//     std::string delete_sql = "DELETE FROM INSTANCE WHERE NAME = \'" + instance_key + "\';";
//     rc = sqlite3_exec(db, delete_sql.c_str(), instance_callback, 0, &erroms);
//     if (rc)
//     {
//         LOG(ERROR) << "sql excute error: " << erroms;
//         return false;
//     }
//     LOG(INFO) << "delete instance: " << instance_key << " success";

//     return true;
// }

// bool dbManager::SearchDeviceInstance(const std::string &key, InstanceDBStruct &value)
// {
//     instancestructs.clear();
//     std::string search_sql = "SELECT * FROM INSTANCE WHERE NAME = \'" + key + "\' ;";
//     int rc;
//     char *erroms = 0;
//     rc = sqlite3_exec(db, search_sql.c_str(), instance_callback, 0, &erroms);
//     if (rc)
//     {
//         LOG(ERROR) << "sql excute error: " << erroms;
//     }
//     LOG(INFO) << "search instance success with key: " << key;
//     if (instancestructs.size() == 0)
//     {
//         LOG(INFO) << "the instance:  " << key << " is not exist";
//         return false;
//     }
//     value = instancestructs.front();
//     return true;
// }

// bool dbManager::GetCameraInstance(const std::string &key, CameraInstance &instance)
// {
//     InstanceDBStruct ins;
//     if (!SearchDeviceInstance(key, ins))
//     {
//         return false;
//     }
//     if (ins.kind != "camera")
//     {
//         LOG(INFO) << "the key: " << key << " is not a camera kind";
//         return false;
//     }
//     if (!UnMarshal(ins.value, instance))
//     {
//         LOG(INFO) << "intance init error";
//         return false;
//     }
//     return true;
// }

// bool dbManager::GetSensorInstance(const std::string &key, SensorInstance &instance)
// {
//     InstanceDBStruct ins;
//     if (!SearchDeviceInstance(key, ins))
//     {
//         return false;
//     }
//     if (ins.kind != "sensor")
//     {
//         LOG(INFO) << "the key: " << key << " is not a camera kind";
//         return false;
//     }
//     if (!UnMarshal(ins.value, instance))
//     {
//         LOG(INFO) << "intance init error";
//         return false;
//     }
//     return true;
// }

// bool dbManager::GetLoudspeakerInstance(const std::string &key, LoudspeakerInstance &instance)
// {
//     InstanceDBStruct ins;
//     if (!SearchDeviceInstance(key, ins))
//     {
//         return false;
//     }
//     if (ins.kind != "loudspeaker")
//     {
//         LOG(INFO) << "the key: " << key << " is not a camera kind";
//         return false;
//     }
//     if (!UnMarshal(ins.value, instance))
//     {
//         LOG(INFO) << "intance init error";
//         return false;
//     }
//     return true;
// }

// bool dbManager::RefreshDeviceInstance(const std::string &filepath)
// {
//     YAML::Node instance_yaml;
//     Json::Value instance_json;
//     try
//     {
//         instance_yaml = YAML::LoadFile(filepath);
//     }
//     catch (YAML::ParserException e)
//     {
//         LOG(ERROR) << "yaml is malformed.";
//         return false;
//     }
//     catch (YAML::BadFile e)
//     {
//         LOG(ERROR) << "file in: " << filepath << "can't be load";
//         return false;
//     }
//     if (!YamlToJson(instance_yaml, instance_json))
//     {
//         LOG(ERROR) << "convert the yaml file in: " << filepath << "error";
//         return false;
//     }

//     // check the instance is exist or not
//     instancestructs.clear();
//     std::string instance_key = GetInstanceKey(instance_json);
//     std::string search_sql = "SELECT * FROM INSTANCE WHERE NAME = \'" + instance_key + "\';";
//     int rc;
//     char *erroms = 0;
//     rc = sqlite3_exec(db, search_sql.c_str(), instance_callback, 0, &erroms);
//     if (rc)
//     {
//         LOG(ERROR) << "sql excute error with: " << erroms;
//     }
//     if (instancestructs.size() == 0)
//     {
//         LOG(INFO) << "the instance: " << instance_key << " is not exist";
//         return false;
//     }

//     // update the instance
//     std::string apiVersion_string = GetInstanceGroupAndVersion(instance_json);
//     int flag = apiVersion_string.find("/");
//     std::string instance_group = apiVersion_string.substr(flag);
//     std::string instance_kind = GetInstanceKind(instance_json);
//     std::string instance_version = apiVersion_string.substr(flag + 1, apiVersion_string.size() - flag + 1);
//     std::string instance_value = JsonToString(instance_json);

//     std::string update_sql = "UPDATE INSTANCE SET VALUE = \'" + instance_value + "\' , APIGROUP = \'" + instance_group + "\', KIND = \'" + instance_kind + "\', VERSION = \'" + instance_version;
//     update_sql += "\' WHERE NAME = \'" + instance_key + "\' ;";
//     rc = sqlite3_exec(db, update_sql.c_str(), instance_callback, 0, &erroms);
//     if (rc)
//     {
//         LOG(ERROR) << "sql excute error with: " << erroms;
//         return false;
//     }
//     LOG(INFO) << "update instance success with key: " << instance_key;
//     return true;
// }

// int dbManager::level1_callback(void *unused, int columenCount, char **columnValue, char **columnName)
// {
//     return SQLITE_OK;
// }
// int dbManager::level2_callback(void *unused, int columenCount, char **columnValue, char **columnName)
// {
//     return SQLITE_OK;
// }
// int dbManager::level3_callback(void *unused, int columenCount, char **columnValue, char **columnName)
// {
//     return SQLITE_OK;
// }