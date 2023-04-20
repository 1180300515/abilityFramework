#include "abilitymanager/abi_interface.h"
#include "plugincore/event_interface.h"

#include "abilitymanager/abi_interface.h"
#include <iostream>

std::string AbilityManager::getName() const{
    return "AbilityManager";
}

std::string AbilityManager::getDescription() const{
    return "AbilityManager works for managing abilities";
}

void AbilityManager::execute(SubjectManager* subjectManager){
    std::cout << "AbilityManager is running" << std::endl;
    transportMessage2Subject(subjectManager, "LANinfo", "Hello from AbilityManager");
}

// bool AbilityManager::registerObserver(SubjectManager* subjectManager){
//     std::cout << "AbilityManager is registering" << std::endl;
//     for (auto subject : targetSubjects()){
//         Subject *s = subjectManager->getSubject(subject);
//         std::cout << "1" << std::endl;
//         if(s != nullptr){
//             s->registerObserver(observer);
//             std::cout << "2" << std::endl;
//         }
//     }
//     std::cout << "AbilityManager is registered" << std::endl;
//     return true;
// }

extern "C" PluginInterface* CreatePluginFunc()
{
    std::cout << "Creating AbilityManager" << std::endl;
    return new AbilityManager();
}

extern "C" void DestroyPluginFunc(PluginInterface* plugin)
{
    std::cout << "Destroying AbilityManager" << std::endl;
    delete plugin;
}