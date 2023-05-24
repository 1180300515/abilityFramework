#ifndef __ABIMGR_INTERFACE_H__
#define __ABIMGR_INTERFACE_H__

#include <unordered_map>
#include <thread>
#include <mutex>
#include <chrono>
#include <memory>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>

#include <httplib.h>

#include "discoverymanager/localhw_interface.h"
#include "discoverymanager/devpool_interface.h"
#include "controller/common/ability_struct.h"
#include "controller/controller.h"

// 心跳信息
struct HeartbeatInfo
{
    std::string abilityName;
    int abilityPort;
    std::string status;
    std::chrono::steady_clock::time_point last_update;

    Json::Value toJson(int port) const
    {
        Json::Value j;
        j["abilityName"] = abilityName;
        j["status"] = status;
        j["last_update"] = std::chrono::duration_cast<std::chrono::milliseconds>(last_update.time_since_epoch()).count();
        j["port"] = port;
        j["abilityPort"] = abilityPort;
        return j;
    }
};


class AbilityInfoExtract{
public:
    std::string name;
    Abilitydepends depends;
};

class AbilityInfoExtractList{
public:
    std::vector<AbilityInfoExtract> abilities;

    void print(){
        for(auto& ability: abilities){
            std::cout << BLUE << "Name: " << ability.name << NONE << std::endl;
            for(auto& depend: ability.depends.abilities){
                std::cout << "abilitydeps: " <<depend << std::endl;
            }
            for(auto& depend: ability.depends.devices){
                std::cout << "devicedeps: " << depend << std::endl;
            }
        }
    }
};

AbilityInfoExtractList buildAbilityInfoExtractList(Controller& controller);

struct TreeNode {
    AbilityInfoExtract ability;
    std::vector<TreeNode> children;
    int level;
};

class DependTreeArray {
public:
    std::vector<TreeNode> trees;

};

DependTreeArray GenerateDependTreeArray(AbilityInfoExtractList& list);

class AbilitySupported
{
public:
    std::string name;
    std::string level;
    Abilitydepends depends;
    std::string exists;
    std::string hostname;
    std::string ip;

    Json::Value toJson()
    {
        Json::Value j;
        j["name"] = name;
        j["level"] = level;
        j["exists"] = exists;
        j["hostname"] = hostname;
        j["ip"] = ip;
        return j;
    }

    AbilitySupported(const std::string &_name, const std::string &_level, const Abilitydepends &_depends,
                     const std::string &_exists, const std::string &_hostname, const std::string &_ip)
        : name(_name), level(_level), depends(_depends), exists(_exists), hostname(_hostname), ip(_ip)
    {
    }
};

Json::Value serializeNode(const TreeNode& node);
Json::Value serializeDependTreeArray(const DependTreeArray& treeArray);

class AbilitySupportedList
{
public:
    std::vector<AbilitySupported> abilities;


    Json::Value toJson()
    {
        Json::Value j;
        for (auto &ability : abilities)
        {
            j.append(ability.toJson());
        }
        return j;
    }
};

Json::Value mapToJson(const std::unordered_map<int, HeartbeatInfo> &map);
// 心跳信息映射表
using HeartbeatMap = std::unordered_map<int, HeartbeatInfo>;
extern HeartbeatMap heartbeat_map;

// 互斥锁保护心跳信息映射表
extern std::mutex heartbeat_map_mutex;

// 打印心跳信息表的函数
void print_heartbeat_info();

// 检查超时的函数
void check_timeout();

void check_process();

void run_http_server();

bool start_program(const std::string &program_path);

bool fileExists(const std::string &filename);

std::string stripSlashPrefix(const std::string &str);

void PrintTreeIterative(const TreeNode& root);

void GenerateNodes(TreeNode& node, DevicePoolExtended& devicePool, DependTreeArray& treeArray);

DependTreeArray GenerateDependTreeArrayWithDevices(DevicePoolExtended& devicePool, DependTreeArray& treeArray);

std::vector<std::vector<std::string>> cartesianProduct(const std::vector<std::vector<std::string>*>& lists);

void PrintDependTreeArray(const DependTreeArray& treeArray);

void PrintTreeNode(const TreeNode& node);


#endif // __ABIMGR_INTERFACE_H__