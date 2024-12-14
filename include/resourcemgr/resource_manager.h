#ifndef RESOURCEMGR_RESOURCE_MANAGER_H_
#define RESOURCEMGR_RESOURCE_MANAGER_H_

#include <map>
#include <memory>
#include <mutex>  // NOLINT [build/c++11]
#include <string>
#include <thread>  // NOLINT [build/c++11]
#include <vector>

#include <ability.h>
#include <shared_mutex>
#include <unordered_map>

class resourceManager {
   public:
    void Init();

    void Run();

    std::unordered_map<std::string, Ability> getAbilityList();

   private:
    void parseYamlFile(const std::string& filePath);

    void changeLocation(const Json::Value& value);

    std::unordered_map<std::string, Ability> ability_list;
    std::shared_mutex lock_list;

    std::thread loop_thread;
};

#endif  // RESOURCEMGR_RESOURCE_MANAGER_H_
