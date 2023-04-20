#include <iostream>
#include "plugincore/plugin_interface.h"
#include "plugincore/obs_interface.h"
#include "plugincore/subject_mgr.h"
#include "plugincore/event_interface.h"
#include "eventhandler/eventhandler.h"

using namespace plugs;

// generate helloworld code
int main()
{
    std::cout << "Hello World!" << std::endl;
    SubjectManager *subjectManager = new SubjectManager();
    auto AMplugin = new PluginLoader("/Users/luozhaoxin/Documents/AbilityFrameworkCpp/config/testconfig.yaml");
    AMplugin->listPlugins();
    auto laninfo = new LANinfo();
    subjectManager->addSubject(laninfo);
    for (const auto &[name, plugin] : AMplugin->getPlugins())
    {
        std::cout << "Plugin name: " << name << std::endl;
        std::cout << "Plugin description: " << plugin->getDescription() << std::endl;
        plugin->registerObserver(subjectManager);
    }
    std::cout << "get laninfo observer nums: "; 
    laninfo->getObserverNums();
    std::cout << "get laninfo observer nums: " << laninfo->observers.size() << std::endl;
    AMplugin->executePlugin("ability_mgr", subjectManager);
    return 0;
}
