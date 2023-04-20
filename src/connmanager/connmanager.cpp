#include "connmanager/conn_interface.h"
#include <iostream>

std::string ConnectionManager::getName() const{
    return "ConnectionManager";
}

std::string ConnectionManager::getDescription() const{
    return "ConnectionManager works for managing connections";
}

void ConnectionManager::execute(SubjectManager* subjectManager){
    std::cout << "ConnectionManager is running" << std::endl;
}

// bool ConnectionManager::registerObserver(SubjectManager* subjuectManager){
//     std::cout << "ConnectionManager is registering" << std::endl;
//     for (auto subject : targetSubjects()){
//         subjuectManager->getSubject(subject)->registerObserver(observer);
//     }

//     return true;
// }

// 公用PluginInterface接口
extern "C" PluginInterface* CreatePluginFunc()
{
    std::cout << "Creating ConnectionManager" << std::endl;
    return new ConnectionManager();
}

extern "C" void DestroyPluginFunc(PluginInterface* plugin)
{
    std::cout << "Destroying ConnectionManager" << std::endl;
    delete plugin;
}
