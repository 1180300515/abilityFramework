#include <iostream>
#include <thread>
#include "plugincore/plugin_interface.h"
#include "plugincore/obs_interface.h"
#include "plugincore/subject_mgr.h"
#include "plugincore/event_interface.h"
#include "plugincore/abimgr_interface.h"

#include "eventhandler/eventhandler.h"

#include "discoverymanager/localhw_interface.h"
#include "controller/controller.h"
#include "discoverymanager/discmgr_interface.h"
#include "discoverymanager/localhw_interface.h"
#include "glog/logging.h"

using namespace plugs;

// std::string getOsName();

int main(int argc, char **argv)
{
    // FLAGS_colorlogtostderr = true;   // log信息区分颜色
    // FLAGS_logtostderr = 1;           // 允许log信息打印到屏幕
    // google::InitGoogleLogging(argv[0]);
    // LOG(INFO) << "Found cookies";

    /*
    auto laninfo = new LANinfo();
    SubjectManager *subjectManager = new SubjectManager();
    auto AMplugin = new PluginLoader("./config/testconfig.yaml");
    AMplugin->listPlugins();
    subjectManager->addSubject(laninfo);

    for(auto [name, subject] : subjectManager->subjects){
        std::cout << name << " : " <<  subject << std::endl;
    }


    for (const auto &[name, plugin] : AMplugin->getPlugins())
    {
        // std::cout << "Plugin name: " << name << std::endl;
        // std::cout << "Plugin description: " << plugin->getDescription() << std::endl;
        plugin->registerObserver(subjectManager);
    }
    AMplugin->executePlugin("connection_mgr", subjectManager);
    AMplugin->executePlugin("ability_mgr", subjectManager);
    */
    LocalhwPrint();

    std::thread http_server_thread(run_http_server);
    std::thread timeout_thread(check_timeout);

    // std::cout << "Start a program: " << start_program("./bin/helloworld") << std::endl;


    std::cout << "Not output anything." << std::endl;

    Preprocessing();
    std::shared_ptr<Controller> controller = std::make_shared<Controller>();
    std::thread receiver_thread(udp_broadcast_receiver);
    std::thread sender_thread(udp_broadcast_sender, std::bind(&Controller::SetEdgeAddressRecord, controller , std::placeholders::_1));

    sleep(1);
    controller->Run();
    sender_thread.join();
    receiver_thread.join();
    http_server_thread.join();
    timeout_thread.join();
    return 0;
}
