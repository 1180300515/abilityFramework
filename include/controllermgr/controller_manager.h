/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: lifecycle_manager.h
 * Description: lifecycle_manager.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef CONTROLLER_MANAGER_H_
#define CONTROLLER_MANAGER_H_

#include <future>  // NOLINT [build/c++11]
#include <memory>
#include <shared_mutex>
#include <string>
#include <thread>  // NOLINT [build/c++11]
#include <map>
#include <unistd.h>
#include <glog/logging.h>
#include <sys/stat.h>

#include "controllermgr/controller_heartbeat_info.h"
#include "abilityrelationmgr/ability_instance_info_extract.h"

class ControllerManager {
   public:
    void updateControllerInfo(ControllerHeartbeatInfo info);
    std::string getControllerInfo();
    bool start_process(const std::string &abilityName);
    void init(std::function<std::vector<AbilityInfoExtract>()> callback);
    void run();
    bool fileExists(const std::string &filename);

   private:
    // store controller's port
    std::map<std::string, ControllerHeartbeatInfo> httpPortMap;
    std::map<std::string, std::chrono::steady_clock::time_point> onlineTimeMap;
    std::shared_mutex port_mutex;
    std::function<std::vector<AbilityInfoExtract>()> abilityInfo_callback;
    std::thread updateControllerThread;

};

#endif  // CONTROLLER_MANAGER_H_
