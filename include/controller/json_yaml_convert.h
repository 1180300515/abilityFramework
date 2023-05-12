#ifndef JSON_YAML_CONVERT_H
#define JSON_YAML_CONVERT_H

#include <iostream>
#include <string>

#include "yaml-cpp/yaml.h"
#include "jsoncpp/json/json.h"

/**
 * convert yaml to json
*/
bool YamlToJson(const YAML::Node &ynode, Json::Value &jnode , std::string name = "");

bool YamlToJsonForInstance(const YAML::Node &ynode, Json::Value &jnode , std::string name = "");
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


/**
 * get the schema part from the crd
*/
std::string GetCrdSchema(const Json::Value &jnode);



//instance func part
/**
 * get the part need to validate from the instance
*/
std::string GetInstanceValidatePart(const Json::Value &jnode);


//ability func part
/**
 * get the validate part
*/
std::string GetAbilityValidatePart(const Json::Value &jnode);



#endif