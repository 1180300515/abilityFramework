#include "database_manager.h"

#include "glog/logging.h"

#include "yaml_json_converter.h"

bool DatabaseManager::RegisterCrd(const std::string &filepath)
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
    std::string crd_schema_string = GetCrdSchemaPart(crd_json);

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

bool DatabaseManager::AddAbilityInstance(const std::string &filepath)
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

    // check the crd is exist or not
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

bool DatabaseManager::AddDeviceInstance(const std::string &filepath)
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
        char name[256];
        gethostname(name, sizeof(name));
        std::string hostname = name;

        if (instance_json["spec"]["hostname"] != hostname && instance_json["spec"]["hostname"] != "")
        {
            LOG(ERROR) << "the " << instance_json["spec"]["hostname"] << " is not the local hostname";
            return false;
        }
        else
        {
            instance_json["spec"]["hostname"] = hostname;
        }
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

bool DatabaseManager::DBGetDeviceInstances(const std::string &resourcetype, std::map<std::string, std::string> &instance)
{
    instancestructs.clear();
    std::string search_sql = "SELECT * FROM INSTANCE WHERE KIND = \'" + resourcetype + "\' ;";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, search_sql.c_str(), instance_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql: " << search_sql << "\nexcute error: " << erroms;
        return false;
    }
    for (int i = 0; i < instancestructs.size(); i++)
    {
        std::string key = instancestructs[i].namespace_name + "/" + instancestructs[i].name;
        instance[key] = instancestructs[i].value;
    }
    return true;
}

bool DatabaseManager::DBGetAbilityInstances(std::map<std::string, std::string> &instance)
{
    abilitystructs.clear();
    std::string search_sql = "SELECT * FROM ABILITY ;";
    int rc;
    char *erroms = 0;
    rc = sqlite3_exec(db, search_sql.c_str(), ability_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql: " << search_sql << "\nexcute error: " << erroms;
        return false;
    }
    for (int i = 0; i < abilitystructs.size(); i++)
    {
        std::string key = abilitystructs[i].namespace_name + "/" + abilitystructs[i].name;
        instance[key] = abilitystructs[i].value;
    }
    return true;
}

bool DatabaseManager::DBGetCloudAddress(std::string &cloudaddress)
{
    cloud_address = "";
    int rc;
    char *erroms = 0;
    std::string search_sql = "select * from CLOUDADDRESS;";
    rc = sqlite3_exec(db, search_sql.c_str(), cloud_address_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    cloudaddress = cloud_address;
    LOG(INFO) << "db get cloud address : " << cloud_address;
    return true;
}

bool DatabaseManager::DBStoreAbilityInstance(const std::string &data)
{
    Json::Value instance_json;
    StringToJson(data, instance_json);

    std::string apiVersion_string = instance_json["apiVersion"].asString();
    int flag = apiVersion_string.find("/");
    std::string instance_group = apiVersion_string.substr(0, flag);
    std::string instance_kind = instance_json["kind"].asString();
    // check crd exist or not
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
        LOG(INFO) << "the ability pass the validate ";
    }
    else
    {
        LOG(INFO) << "validate fail about the ability";
        return false;
    }
    // add ability into the ability table
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
    LOG(INFO) << "DB add ability: " << instance_namespace + "/" + instance_key << " success";
    return true;
    return false;
}

bool DatabaseManager::DBStoreDeviceInstance(const std::string &data)
{
    Json::Value instance_json;
    StringToJson(data, instance_json);

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
    std::string crd_schema = crdstructs.begin()->schema;
    std::string instance_validate = GetInstanceValidatePart(instance_json);
    if (SchemaValidation(crd_schema, instance_validate))
    {
        LOG(INFO) << "the device instance pass the validate ";
    }
    else
    {
        LOG(INFO) << "validate fail about the device instance";
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
        char name[256];
        gethostname(name, sizeof(name));
        std::string hostname = name;

        if (instance_json["spec"]["hostname"] != hostname && instance_json["spec"]["hostname"] != "")
        {
            LOG(ERROR) << "the " << instance_json["spec"]["hostname"] << " is not the local hostname";
            return false;
        }
        else
        {
            instance_json["spec"]["hostname"] = hostname;
        }
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
    LOG(INFO) << "DB add device instance: " << instance_namespace + "/" + instance_key << " success";

    return true;
}

bool DatabaseManager::DBStoreCloudAddress(const std::string &cloudaddress)
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
    std::string insert_sql = "INSERT OR IGNORE INTO CLOUDADDRESS ( ADDRESS ) VALUES (\'" + cloudaddress + "\');";
    rc = sqlite3_exec(db, insert_sql.c_str(), cloud_address_callback, 0, &erroms);
    if (rc)
    {
        LOG(ERROR) << "sql excute error: " << erroms;
        return false;
    }
    LOG(INFO) << "db store cloud address : " << cloudaddress;
    return true;
}

bool DatabaseManager::DBUpdateDeviceInstance(const std::string &data)
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

bool DatabaseManager::DBUpdateAbilityInstance(const std::string &data)
{
    return false;
}

bool DatabaseManager::DBDelteDeviceInstance(const std::string &key)
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

int DatabaseManager::device_callback(void *unused, int columenCount, char **columnValue, char **columnName)
{
    InstanceDBStruct instance;
    for (int i = 0; i < columenCount; i++)
    {
        if (strcmp(columnName[i], "KEY") == 0)
        {
            instance.key = columnValue[i];
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
    devicestructs.push_back(instance);
    LOG(INFO) << "devicecallback: find instance:" << instance.key;
    return SQLITE_OK;
}

int DatabaseManager::ability_callback(void *unused, int columenCount, char **columnValue, char **columnName)
{
    InstanceDBStruct instance;
    for (int i = 0; i < columenCount; i++)
    {
        if (strcmp(columnName[i], "KEY") == 0)
        {
            instance.key = columnValue[i];
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
    LOG(INFO) << "abilitycallback: find ability:" << instance.key;
    return SQLITE_OK;
}

int DatabaseManager::cloud_address_callback(void *unused, int columenCount, char **columnValue, char **columnName)
{
    cloud_address = columnValue[0];
    return SQLITE_OK;
}
