/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: database_manager.cpp
 * Description: database_manager.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "databasemgr/database_manager.h"

#include <glog/logging.h>

#include "utils/global_var.h"
#include "utils/yaml_json_converter.h"

std::vector<CrdDBStruct> DatabaseManager::crdstructs;
std::vector<InstanceDBStruct> DatabaseManager::devicestructs;
std::vector<InstanceDBStruct> DatabaseManager::abilitystructs;
char DatabaseManager::host_name[256] = {};

void DatabaseManager::Init(std::string hostname, bool cleandb)
{
    memset(host_name, '\0', sizeof(host_name));
    int rc;
    char *erroms = 0;
    std::string search_sql = "select * from HOSTNAME;";
    rc = sqlite3_exec(db, search_sql.c_str(), hostname_callback, 0, &erroms);
    if (rc) {
        DLOG(ERROR) << "sql excute error: " << erroms;
        return;
    }
    if (cleandb) {
        DBCleanCRD();
        DBCleanAbility();
        DBCleanDevice();
        DBCleanHostname();
        std::string insert_sql = "insert into HOSTNAME (NAME) VALUES (\'" + hostname + "\');";
        rc = sqlite3_exec(db, insert_sql.c_str(), hostname_callback, 0, &erroms);
        if (rc) {
            DLOG(ERROR) << "sql excute error: " << erroms;
            return;
        }

    } else if (strcmp(hostname.c_str(), host_name) != 0) {
        DLOG(WARNING) << "The hostname store in database : " << host_name << " does not match with the local hostname : " << hostname
                      << " db will be cleared";
        DBCleanCRD();
        DBCleanAbility();
        DBCleanDevice();
        DBCleanHostname();
        std::string insert_sql = "insert into HOSTNAME (NAME) VALUES (\'" + hostname + "\');";
        rc = sqlite3_exec(db, insert_sql.c_str(), hostname_callback, 0, &erroms);
        if (rc) {
            DLOG(ERROR) << "sql excute error: " << erroms;
            return;
        }
    } else {
        loadCRD();
    }
}

bool DatabaseManager::RegistCrdFromFile(const std::string &filepath)
{
    YAML::Node crd_yaml;
    Json::Value crd_json;
    try {
        crd_yaml = YAML::LoadFile(filepath);
    } catch (YAML::ParserException e) {
        DLOG(ERROR) << "yaml is malformed.";
        return false;
    } catch (YAML::BadFile e) {
        DLOG(ERROR) << "file can't be load";
        return false;
    }
    if (!YamlToJson(crd_yaml, &crd_json)) {
        DLOG(ERROR) << "convert the yaml file in: " << filepath << "error";
        return false;
    }
    // get related value
    std::string crd_key_name = crd_json["metadata"]["name"].asString();
    Json::FastWriter writer;
    std::string crd_json_string = writer.write(crd_json);
    std::string crd_group_string = crd_json["spec"]["group"].asString();
    std::string crd_kind_string = crd_json["spec"]["names"]["kind"].asString();
    std::string crd_schema_string = GetCrdSchemaPart(crd_json);

    std::string sql = "INSERT OR IGNORE INTO CRD (KEY,VALUE,APIGROUP,KIND,SCHEMA) VALUES (\'";
    sql += crd_key_name + "\',\'" + crd_json_string + "\',\'" + crd_group_string + "\',\'" + crd_kind_string + "\',\'" + crd_schema_string + "\' );";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, sql.c_str(), crd_callback, 0, &erroms);
    if (rc) {
        DLOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    DLOG(INFO) << "register success with crd name: " << crd_key_name << " in file: " << filepath;

    CrdDBStruct new_one;
    new_one.key = crd_key_name;
    new_one.value = crd_json_string;
    new_one.group = crd_group_string;
    new_one.kind = crd_kind_string;
    new_one.schema = crd_schema_string;
    this->regist_crd_list[new_one.kind + "." + new_one.group] = new_one;

    return true;
}

bool DatabaseManager::RegistCrd(const std::string &data)
{
    Json::Value crd_json;
    Json::Reader reader;
    reader.parse(data, crd_json);

    std::string crd_key_name = crd_json["metadata"]["name"].asString();
    Json::FastWriter writer;
    std::string crd_json_string = writer.write(crd_json);
    std::string crd_group_string = crd_json["spec"]["group"].asString();
    std::string crd_kind_string = crd_json["spec"]["names"]["kind"].asString();
    std::string crd_schema_string = GetCrdSchemaPart(crd_json);

    std::string sql = "INSERT OR IGNORE INTO CRD (KEY,VALUE,APIGROUP,KIND,SCHEMA) VALUES (\'";
    sql += crd_key_name + "\',\'" + crd_json_string + "\',\'" + crd_group_string + "\',\'" + crd_kind_string + "\',\'" + crd_schema_string + "\' );";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, sql.c_str(), crd_callback, 0, &erroms);
    if (rc) {
        DLOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    DLOG(INFO) << "register success with crd name: " << crd_key_name;

    CrdDBStruct new_one;
    new_one.key = crd_key_name;
    new_one.value = crd_json_string;
    new_one.group = crd_group_string;
    new_one.kind = crd_kind_string;
    new_one.schema = crd_schema_string;
    this->regist_crd_list[new_one.kind + "." + new_one.group] = new_one;

    return true;
}

bool DatabaseManager::UnregistCrd(const std::string &name)
{
    int rc;
    char *erroms = 0;
    std::string delete_sql = "DELETE FROM CRD WHERE KEY = " + name + ";";
    rc = sqlite3_exec(db, delete_sql.c_str(), ability_callback, 0, &erroms);
    if (rc) {
        DLOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    DLOG(INFO) << "db clean " << name << " crd resource success";

    this->regist_crd_list.erase(name);

    return true;
}

bool DatabaseManager::DBGetDeviceInstances(const std::string &resourcetype, std::map<std::string, std::string> *instance)
{
    devicestructs.clear();
    std::string search_sql = "SELECT * FROM DEVICE WHERE KIND = \'" + resourcetype + "\' ;";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, search_sql.c_str(), device_callback, 0, &erroms);
    if (rc) {
        DLOG(ERROR) << "sql: " << search_sql << "\nexcute error: " << erroms;
        return false;
    }
    for (int i = 0; i < devicestructs.size(); i++) {
        std::string key = devicestructs[i].key;
        (*instance)[key] = devicestructs[i].value;
    }
    return true;
}

bool DatabaseManager::DBGetAbilityInstances(std::map<std::string, std::string> *instance)
{
    abilitystructs.clear();
    std::string search_sql = "SELECT * FROM ABILITY ;";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, search_sql.c_str(), ability_callback, 0, &erroms);
    if (rc) {
        DLOG(ERROR) << "sql: " << search_sql << "\nexcute error: " << erroms;
        return false;
    }
    for (int i = 0; i < abilitystructs.size(); i++) {
        std::string key = abilitystructs[i].key;
        (*instance)[key] = abilitystructs[i].value;
    }
    return true;
}

bool DatabaseManager::DBStoreAbilityInstance(const Json::Value &instance_json)
{
    if (!validateJson(instance_json)) {
        return false;
    }

    if (!storeAbility(instance_json)) {
        return false;
    }
    return true;
}

bool DatabaseManager::DBStoreDeviceInstance(const Json::Value &instance_json)
{
    if (!validateJson(instance_json)) {
        return false;
    }
    if (!storeDevice(instance_json)) {
        return false;
    }
    return true;
}

bool DatabaseManager::DBUpdateDeviceInstance(const Json::Value &jnode)
{
    std::string apiVersion_string = jnode["apiVersion"].asString();
    int flag = apiVersion_string.find("/");
    std::string instance_group = apiVersion_string.substr(0, flag);
    std::string instance_version = apiVersion_string.substr(flag + 1);

    std::string instance_name = jnode["metadata"]["name"].asString();
    std::string instance_kind = jnode["kind"].asString();
    Json::FastWriter writer;
    std::string instance_value = writer.write(jnode);
    std::string instance_namespace = jnode["metadata"]["namespace"].asString();
    std::string instance_key = instance_namespace + "/" + instance_name;
    std::string update_sql = "UPDATE DEVICE SET VALUE = \'" + instance_value + "\' , APIGROUP = \'" + instance_group + "\', KIND = \'" +
                             instance_kind + "\', VERSION = \'" + instance_version;
    update_sql += "\' WHERE KEY = \'" + instance_key + "\' ;";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, update_sql.c_str(), device_callback, 0, &erroms);
    if (rc) {
        DLOG(ERROR) << "sql excute error with: " << erroms;
        return false;
    }
    DLOG(INFO) << "db update device : " << instance_key << " success";
    return true;
}

bool DatabaseManager::DBUpdateAbilityInstance(const Json::Value &instance_json)
{
    std::string apiVersion_string = instance_json["apiVersion"].asString();
    int flag = apiVersion_string.find("/");
    std::string instance_name = instance_json["metadata"]["name"].asString();
    Json::FastWriter writer;
    std::string instance_value = writer.write(instance_json);
    std::string instance_version = apiVersion_string.substr(flag);
    std::string instance_namespace = "default";
    std::string instance_group = apiVersion_string.substr(0, flag);
    std::string instance_kind = instance_json["kind"].asString();
    if (instance_json["metadata"].isMember("namespace")) {
        std::string instance_namespace = instance_json["metadata"]["namespace"].asString();
    }
    std::string instance_key = instance_namespace + "/" + instance_name;
    std::string update_sql = "UPDATE ABILITY SET VALUE = \'" + instance_value + "\' , APIGROUP = \'" + instance_group + "\', KIND = \'" +
                             instance_kind + "\', VERSION = \'" + instance_version;
    update_sql += "\' WHERE KEY = \'" + instance_key + "\' ;";
    int rc2;
    char *erroms2 = 0;
    rc2 = sqlite3_exec(db, update_sql.c_str(), ability_callback, 0, &erroms2);
    if (rc2) {
        DLOG(ERROR) << "sql excute error with: " << erroms2;
        return false;
    }
    DLOG(INFO) << "DB update ability: " << instance_key << " success";
    return true;
}

bool DatabaseManager::DBDelteDeviceInstance(const std::string &key)
{
    // delete the instance
    int rc;
    char *erroms = 0;
    std::string delete_sql = "DELETE FROM DEVICE WHERE KEY = \'" + key + "\';";
    rc = sqlite3_exec(db, delete_sql.c_str(), device_callback, 0, &erroms);
    if (rc) {
        DLOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    DLOG(INFO) << "db delete resource : " << key << " success";

    return true;
}

bool DatabaseManager::DBDeleteAbilityInstance(const std::string &key)
{
    // delete the instance
    int rc;
    char *erroms = 0;
    std::string delete_sql = "DELETE FROM ABILITY WHERE KEY = \'" + key + "\';";
    rc = sqlite3_exec(db, delete_sql.c_str(), ability_callback, 0, &erroms);
    if (rc) {
        DLOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    DLOG(INFO) << "db delete resource : " << key << " success";

    return true;
}

bool DatabaseManager::DBCleanDevice()
{
    int rc;
    char *erroms = 0;
    std::string delete_sql = "DELETE FROM DEVICE;";
    rc = sqlite3_exec(db, delete_sql.c_str(), device_callback, 0, &erroms);
    if (rc) {
        DLOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    DLOG(INFO) << "db clean instance resource success";

    return true;
}

bool DatabaseManager::DBCleanAbility()
{
    int rc;
    char *erroms = 0;
    std::string delete_sql = "DELETE FROM ABILITY;";
    rc = sqlite3_exec(db, delete_sql.c_str(), ability_callback, 0, &erroms);
    if (rc) {
        DLOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    DLOG(INFO) << "db clean ability resource success";

    return true;
}

bool DatabaseManager::DBCleanCRD()
{
    int rc;
    char *erroms = 0;
    std::string delete_sql = "DELETE FROM CRD;";
    rc = sqlite3_exec(db, delete_sql.c_str(), ability_callback, 0, &erroms);
    if (rc) {
        DLOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    DLOG(INFO) << "db clean crd resource success";

    return true;
}

bool DatabaseManager::DBCleanHostname()
{
    int rc;
    char *erroms = 0;
    std::string delete_sql = "DELETE FROM HOSTNAME;";
    rc = sqlite3_exec(db, delete_sql.c_str(), hostname_callback, 0, &erroms);
    if (rc) {
        DLOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    DLOG(INFO) << "db clean hostname success";

    return true;
}

int DatabaseManager::crd_callback(void *unused, int columenCount, char **columnValue, char **columnName)
{
    CrdDBStruct crd;
    for (int i = 0; i < columenCount; i++) {
        if (strcmp(columnName[i], "KEY") == 0) {
            crd.key = columnValue[i];
        } else if (strcmp(columnName[i], "VALUE") == 0) {
            crd.value = columnValue[i];
        } else if (strcmp(columnName[i], "APIGROUP") == 0) {
            crd.group = columnValue[i];
        } else if (strcmp(columnName[i], "KIND") == 0) {
            crd.kind = columnValue[i];
        } else if (strcmp(columnName[i], "SCHEMA") == 0) {
            crd.schema = columnValue[i];
        }
    }
    crdstructs.push_back(crd);
    // DLOG(INFO) << "crdcallback: find matched crd struct with key: " << crd.key;
    return SQLITE_OK;
}

int DatabaseManager::device_callback(void *unused, int columenCount, char **columnValue, char **columnName)
{
    InstanceDBStruct instance;
    for (int i = 0; i < columenCount; i++) {
        if (strcmp(columnName[i], "KEY") == 0) {
            instance.key = columnValue[i];
        } else if (strcmp(columnName[i], "VALUE") == 0) {
            instance.value = columnValue[i];
        } else if (strcmp(columnName[i], "APIGROUP") == 0) {
            instance.group = columnValue[i];
        } else if (strcmp(columnName[i], "KIND") == 0) {
            instance.kind = columnValue[i];
        } else if (strcmp(columnName[i], "VERSION") == 0) {
            instance.version = columnValue[i];
        }
    }
    devicestructs.push_back(instance);
    // DLOG(INFO) << "devicecallback: find instance:" << instance.key;
    return SQLITE_OK;
}

int DatabaseManager::ability_callback(void *unused, int columenCount, char **columnValue, char **columnName)
{
    InstanceDBStruct instance;
    for (int i = 0; i < columenCount; i++) {
        if (strcmp(columnName[i], "KEY") == 0) {
            instance.key = columnValue[i];
        } else if (strcmp(columnName[i], "VALUE") == 0) {
            instance.value = columnValue[i];
        } else if (strcmp(columnName[i], "APIGROUP") == 0) {
            instance.group = columnValue[i];
        } else if (strcmp(columnName[i], "KIND") == 0) {
            instance.kind = columnValue[i];
        } else if (strcmp(columnName[i], "VERSION") == 0) {
            instance.version = columnValue[i];
        }
    }
    abilitystructs.push_back(instance);
    // DLOG(INFO) << "abilitycallback: find ability:" << instance.key;
    return SQLITE_OK;
}

int DatabaseManager::hostname_callback(void *unused, int columenCount, char **columnValue, char **columnName)
{
    if (columnValue != NULL) {
        // DLOG(INFO) << columnValue[0];
        // strcpy(host_name, columnValue[0]);
        snprintf(host_name, sizeof(host_name), "%s", columnValue[0]);
    }

    return 0;
}

DatabaseManager::DatabaseManager()
{
    int rc;
    char *errormes = 0;
    rc = sqlite3_open(DATABASE_FILE_PATH, &db);
    if (rc) {
        DLOG(ERROR) << "open database in :" << DATABASE_FILE_PATH << "failed";
        exit(0);
    } else {
        if (!access(DATABASE_FILE_PATH, R_OK)) {
            DLOG(INFO) << "db exist,open success";
        } else {
            DLOG(INFO) << "db not exist,create new one,open success";
        }
    }
    // create related table
    const char *hostname_table_sql;
    const char *crd_table_sql;
    const char *device_table_sql;
    const char *ability_table_sql;

    hostname_table_sql =
        "CREATE TABLE IF NOT EXISTS HOSTNAME ( NAME TEXT PRIMARY KEY   NOT NULL "
        ");";

    crd_table_sql =
        "CREATE TABLE IF NOT EXISTS CRD ( "
        "KEY TEXT PRIMARY KEY NOT NULL, "
        "VALUE TEXT NOT NULL, "
        "APIGROUP TEXT NOT NULL, "
        "KIND TEXT NOT NULL, "
        "SCHEMA TEXT NOT NULL );";
    device_table_sql =
        "CREATE TABLE IF NOT EXISTS DEVICE ( "
        "KEY TEXT PRIMARY KEY     NOT NULL,"
        "VALUE TEXT   NOT NULL,"
        "APIGROUP TEXT   NOT NULL,"
        "KIND  TEXT   NOT NULL,"
        "VERSION TEXT NOT NULL);";
    ability_table_sql =
        "CREATE TABLE IF NOT EXISTS ABILITY ( "
        "KEY TEXT PRIMARY KEY     NOT NULL,"
        "VALUE TEXT   NOT NULL,"
        "APIGROUP TEXT   NOT NULL,"
        "KIND  TEXT   NOT NULL,"
        "VERSION TEXT NOT NULL);";
    rc = sqlite3_exec(db, hostname_table_sql, hostname_callback, 0, &errormes);
    if (rc) {
        DLOG(ERROR) << "create table HOSTNAME fail with sql command: " << hostname_table_sql << "\nerror message : " << errormes;
        exit(0);
    }
    rc = sqlite3_exec(db, crd_table_sql, crd_callback, 0, &errormes);
    if (rc) {
        DLOG(ERROR) << "create table CRD fail with sql command: " << crd_table_sql << "\nerror message : " << errormes;
        exit(0);
    }
    rc = sqlite3_exec(db, device_table_sql, device_callback, 0, &errormes);
    if (rc) {
        DLOG(ERROR) << "create table DEVICE failed with: " << errormes;
        exit(0);
    }
    rc = sqlite3_exec(db, ability_table_sql, ability_callback, 0, &errormes);
    if (rc) {
        DLOG(ERROR) << "create table ABILITY failed with: " << errormes;
        exit(0);
    }
    DLOG(INFO) << "create table HOSTNAME,CRD,DEVICE,ABILITY success";
}

void DatabaseManager::loadCRD()
{
    this->crdstructs.clear();
    std::string search_sql = "SELECT * FROM CRD;";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, search_sql.c_str(), crd_callback, 0, &erroms);
    if (rc) {
        DLOG(ERROR) << "sql: " << search_sql << "\nexcute error: " << erroms;
        return;
    }
    for (const auto &iter : crdstructs) {
        std::string key = iter.kind + "." + iter.group;
        this->regist_crd_list[key] = iter;
    }
}

bool DatabaseManager::validateJson(const Json::Value &instance_json) const
{
    std::string apiVersion = instance_json["apiVersion"].asString();
    int flag = apiVersion.find("/");
    std::string instance_group = apiVersion.substr(0, flag);
    std::string instance_kind = instance_json["kind"].asString();

    if (this->regist_crd_list.count(instance_kind + "." + instance_group) == 0) {
        DLOG(WARNING) << "no matched crd with : " << instance_kind + "." + instance_group << ",please registe before add instance";
        return false;
    }
    std::string schema = this->regist_crd_list.at(instance_kind + "." + instance_group).schema;
    std::string instance_validate = GetValidatePart(instance_json);
    if (!SchemaValidation(schema, instance_validate)) {
        DLOG(WARNING) << GetInstanceKey(instance_json) << "can't pass validate";
        return false;
    }
    // DLOG(INFO) << "the instance : " << GetInstanceKey(instance_json) << " pass
    // the validate";
    return true;
}

bool DatabaseManager::storeDevice(const Json::Value &instance_json) const
{
    std::string apiVersion = instance_json["apiVersion"].asString();
    int flag = apiVersion.find("/");
    std::string instance_version = apiVersion.substr(flag);
    std::string instance_group = apiVersion.substr(0, flag);
    std::string instance_kind = instance_json["kind"].asString();
    std::string instance_name = instance_json["metadata"]["name"].asString();
    std::string instance_namespace = instance_json["metadata"]["namespace"].asString();
    std::string instance_key = instance_namespace + "/" + instance_name;

    Json::FastWriter writer;
    std::string instance_value = writer.write(instance_json);

    std::string insert_sql =
        "INSERT OR IGNORE INTO DEVICE (KEY,VALUE,APIGROUP,KIND,VERSION) VALUES "
        "(\'";
    insert_sql += instance_key + "\',\'" + instance_value + "\',\'" + instance_group + "\',\'" + instance_kind + "\',\'" + instance_version + "\' );";
    int rc2;
    char *erroms2 = 0;
    rc2 = sqlite3_exec(db, insert_sql.c_str(), device_callback, 0, &erroms2);
    if (rc2) {
        DLOG(ERROR) << "sql excute error with: " << erroms2;
        return false;
    }
    DLOG(INFO) << "DB add device instance: " << instance_key << " success";

    return true;
}

bool DatabaseManager::storeAbility(const Json::Value &instance_json) const
{
    std::string apiVersion = instance_json["apiVersion"].asString();
    int flag = apiVersion.find("/");
    std::string instance_version = apiVersion.substr(flag);
    std::string instance_group = apiVersion.substr(0, flag);
    std::string instance_kind = instance_json["kind"].asString();

    std::string instance_name = instance_json["metadata"]["name"].asString();
    std::string instance_namespace = instance_json["metadata"]["namespace"].asString();
    std::string instance_key = instance_namespace + "/" + instance_name;
    Json::FastWriter writer;
    std::string instance_value = writer.write(instance_json);

    std::string insert_sql =
        "INSERT OR IGNORE INTO ABILITY (KEY,VALUE,APIGROUP,KIND,VERSION) VALUES "
        "(\'";
    insert_sql += instance_key + "\',\'" + instance_value + "\',\'" + instance_group + "\',\'" + instance_kind + "\',\'" + instance_version + "\' );";
    int rc2;
    char *erroms2 = 0;
    rc2 = sqlite3_exec(db, insert_sql.c_str(), ability_callback, 0, &erroms2);
    if (rc2) {
        DLOG(ERROR) << "sql excute error with: " << erroms2;
        return false;
    }
    DLOG(INFO) << "DB add ability: " << instance_key << " success";
    return true;
}
