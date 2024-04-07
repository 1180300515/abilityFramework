#ifndef ABILITY_STATUS_MANAGER_H
#define ABILITY_STATUS_MANAGER_H

#include <future>  // NOLINT [build/c++11]
#include <memory>
#include <shared_mutex>
#include <string>
#include <thread>  // NOLINT [build/c++11]
#include <map>
#include <unistd.h>
#include <glog/logging.h>
#include <sys/stat.h>
#include <json/json.h>
#include "httplib.h"

class AbilityStatusManager
{
private:
    std::function<std::string()> get_controller_info;
    std::function<std::string()> get_ability_info;
public:
    AbilityStatusManager(/* args */);
    ~AbilityStatusManager();
    void Init(std::function<std::string()> controller_info_callback, std::function<std::string()> ability_info_callback);
    Json::Value GetAbilityStatusById(unsigned long abilityInstanceId);
    int UpdateAbilityDesireById(std::string req);
    int GetAbilityPort(unsigned long abilityInstanceId);
    int GetControllerPort(unsigned long abilityInstanceId);
};

#endif // ABILITY_STATUS_MANAGER_H