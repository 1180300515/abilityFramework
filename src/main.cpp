#include <iostream>
#include <thread>
#include "plugincore/plugin_interface.h"
#include "plugincore/obs_interface.h"
#include "plugincore/subject_mgr.h"
#include "plugincore/event_interface.h"
#include "plugincore/abimgr_interface.h"
#include "plugincore/lifecycle_interface.h"

#include "eventhandler/eventhandler.h"

#include "controller/controller.h"
#include "discoverymanager/discmgr_interface.h"
#include "discoverymanager/localhw_interface.h"
#include "discoverymanager/devpool_interface.h"

#include "glog/logging.h"

std::shared_ptr<Controller> controller = std::make_shared<Controller>();

using namespace plugs;

int main(int argc, char **argv)
{
    std::thread http_server_thread(run_http_server);
    std::thread timeout_thread(check_timeout);
    std::thread chkproc_thread(check_process);

    std::cout << "Start a program: " << start_program("./bin/camera") << std::endl;

    Preprocessing();
    // std::shared_ptr<Controller> controller = std::make_shared<Controller>();


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
