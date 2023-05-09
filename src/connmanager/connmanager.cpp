#include "connmanager/conn_interface.h"
#include <iostream>

std::string ConnectionManager::getName() const{
    return "ConnectionManager";
}

std::string ConnectionManager::getDescription() const{
    return "ConnectionManager works for managing connections";
}

void ConnectionManager::execute(SubjectManager* subjectManager){
    // std::cout << "ConnectionManager Observer Address: " << observer << std::endl;
    std::cout << L_BLUE << "ConnectionManager is running" << NONE << std::endl;
}

bool ConnectionManager::registerObserver(SubjectManager *subjectManager)
{
    // std::cout << "Registering Plugin: " << getName() << std::endl;
    for (auto subjectName : targetSubjects())
    {
        Subject *subject = subjectManager->getSubject(subjectName);
        // std::cout << subjectName << " Subject address :" << subject << std::endl;
        // std::cout << subjectName << " Observer address :" << observer << std::endl;
        if (subject)
        {
            subject->registerObserver(observer);
            // subject->getObserverNums();
        }
    }
    std::cout << GREEN << "Plugin: " << getName() << " is registered" << NONE << std::endl;
    return true;
}

void ConnectionManager::transportMessage2Subject(SubjectManager *subjectManager, std::string subjectName, std::string message)
{
    auto *subject = subjectManager->getSubject(subjectName);
    // std::cout << "Subject address is :" << subject << std::endl;
    // std::cout << "Sending message from " << getName() << " to " << subjectName << std::endl;
    if (subject)
    {
        // std::cout << "Subject not null" << std::endl;
        // subject->getObserverNums();
        subject->transportMessage(observer, message);
    }
}


// 公用PluginInterface接口
extern "C" PluginInterface* CreatePluginFunc()
{
    std::cout << PURPLE << "Creating ConnectionManager" << NONE << std::endl;
    return new ConnectionManager();
}

extern "C" void DestroyPluginFunc(PluginInterface* plugin)
{
    std::cout << "Destroying ConnectionManager" << std::endl;
    delete plugin;
}
