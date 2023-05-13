#include <iostream>
#include <thread>
#include "plugincore/plugin_interface.h"
#include "plugincore/obs_interface.h"
#include "plugincore/subject_mgr.h"
#include "plugincore/event_interface.h"
#include "plugincore/abimgr_interface.h"

#include "eventhandler/eventhandler.h"

#include "discoverymanager/discmgr_interface.h"
#include "discoverymanager/localhw_interface.h"

using namespace plugs;

// std::string getOsName();

int main()
{
    std::cout << "Hello World!" << std::endl;
    auto laninfo = new LANinfo();
    // std::cout << "LANinfo Observer vector address: " << &laninfo->observers << std::endl;
    // std::cout << "LANinfo Observer vector size: " << laninfo->observers.size() << std::endl;
    SubjectManager *subjectManager = new SubjectManager();
    // std::cout << "subjectManager address :" << subjectManager << std::endl;
    auto AMplugin = new PluginLoader("./config/testconfig.yaml");
    AMplugin->listPlugins();
    subjectManager->addSubject(laninfo);

    // std::cout << "Get subjectManager subject address: " << std::endl;
    // for(auto [name, subject] : subjectManager->subjects){
    //     std::cout << name << " : " <<  subject << std::endl;
    // }

    for (const auto &[name, plugin] : AMplugin->getPlugins())
    {
        // std::cout << "Plugin name: " << name << std::endl;
        // std::cout << "Plugin description: " << plugin->getDescription() << std::endl;
        plugin->registerObserver(subjectManager);
    }
    // std::cout << "get laninfo observer nums: ";
    // laninfo->getObserverNums();
    // std::cout << "get laninfo observer nums: " << laninfo->observers.size() << std::endl;
    // std::cout << "get laninfo observer address: " << std::endl;
    // for(auto observer : laninfo->observers){
    //     std::cout << observer << std::endl;
    // }
    AMplugin->executePlugin("connection_mgr", subjectManager);
    AMplugin->executePlugin("ability_mgr", subjectManager);

    std::cout << LocalhwPrint() << std::endl;

    std::thread sender_thread(udp_broadcast_sender);
    std::thread receiver_thread(udp_broadcast_receiver);
    std::thread http_server_thread(run_http_server);
    std::thread timeout_thread(check_timeout);

    std::cout << "Start a program: " << start_program("./bin/helloworld") << std::endl;

    http_server_thread.join();
    timeout_thread.join();
    sender_thread.join();
    receiver_thread.join();

    std::cout << "Not output anything." << std::endl;

    return 0;
}
