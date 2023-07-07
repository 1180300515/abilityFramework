#include "abilitymanager/abi_interface.h"
#include "plugincore/event_interface.h"
#include "utils/color.h"

#include "abilitymanager/abi_interface.h"
#include <iostream>

std::string AbilityManager::getName() const
{
    return "AbilityManager";
}

std::string AbilityManager::getDescription() const
{
    return "AbilityManager works for managing abilities";
}

void AbilityManager::execute(SubjectManager *subjectManager)
{
    std::cout << L_BLUE << "AbilityManager execute is running" << NONE << std::endl;
    // std::cout << "AbilityManager execute Observer Address: " << observer << std::endl;
    transportMessage2Subject(subjectManager, "LANinfo", "Hello from AbilityManager");
}

bool AbilityManager::registerObserver(SubjectManager *subjectManager)
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
    std::cout << GREEN <<  "Plugin: " << getName() << " is registered" << NONE <<  std::endl;
    return true;
}

void AbilityManager::transportMessage2Subject(SubjectManager *subjectManager, std::string subjectName, std::string message)
{
    auto *subject = subjectManager->getSubject(subjectName);
    // std::cout << "Subject address is :" << subject << std::endl;
    std::cout << RED << "Sending message from " << getName() << " to " << subjectName << NONE << std::endl;
    if (subject)
    {
        // std::cout << "Subject not null" << std::endl;
        // subject->getObserverNums();
        subject->transportMessage(observer, message);
    }
}

extern "C" PluginInterface *CreatePluginFunc()
{
    std::cout << PURPLE << "Creating AbilityManager" << NONE << std::endl;
    return new AbilityManager();
}

extern "C" void DestroyPluginFunc(PluginInterface *plugin)
{
    std::cout << "Destroying AbilityManager" << std::endl;
    delete plugin;
}