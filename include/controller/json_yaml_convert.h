#ifndef JSON_YAML_CONVERT_H
#define JSON_YAML_CONVERT_H

#include <iostream>
#include <string>

#include "yaml-cpp/yaml.h"
#include "jsoncpp/json/json.h"

/**
 * convert yaml to json
*/
bool YamlToJson(const YAML::Node &ynode, Json::Value &jnode);
/**
 * convert json to yaml
*/
bool JsonToYaml(const Json::Value &jnode, YAML::Node &ynode);
/**
 * convert json to string
*/
std::string JsonToString(const Json::Value &jnode);
/**
 * convert string to json
*/
void StringToJson(const std::string jsonformatstring, Json::Value &jnode);
/**
 * validate the inputjson whether pass the schema
*/
bool SchemaValidation(const std::string SchemaJsonString , const std::string inputstring);


//crd func part
/**
 * get the name in metadata from crd
*/
std::string GetCrdKey(const Json::Value &jnode);
/**
 * get the group from the spec in the crd
*/
std::string GetCrdGroup(const Json::Value &jnode);
/**
 * get the kind from the names field in the spec
*/
std::string GetCrdKind(const Json::Value &jnode);
/**
 * get the schema part from the crd
*/
std::string GetCrdSchema(const Json::Value &jnode);
/**
 * replace other type to string
*/
std::string CrdSchemaRevise(const std::string &schemastring);



//instance func part
/**
 * get the instance name in the metadata
*/
std::string GetInstanceKey(const Json::Value &jnode);
/**
 * get the apiversion value from the json
*/
std::string GetInstanceGroupAndVersion(const Json::Value &jnode);
/**
 * get the kind from the instance
*/
std::string GetInstanceKind(const Json::Value &jnode);
/**
 * get the part need to validate from the instance
*/
std::string GetInstanceValidatePart(const Json::Value &jnode);


//ability func part
/**
 * get the abilityname
*/
std::string GetAbilityKey(const Json::Value &jnode);
/**
 * get the apiVersion
*/
std::string GetAbilityGroupAndVersion(const Json::Value &jnode);
/**
 * get the kind
*/
std::string GetAbilityKind(const Json::Value &jnode);
/**
 * get the validate part
*/
std::string GetAbilityValidatePart(const Json::Value &jnode);



#endif