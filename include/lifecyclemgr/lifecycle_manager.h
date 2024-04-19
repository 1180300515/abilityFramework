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

#ifndef LIFECYCLEMGR_LIFECYCLE_MANAGER_H_
#define LIFECYCLEMGR_LIFECYCLE_MANAGER_H_

#include <future>  // NOLINT [build/c++11]
#include <memory>
#include <shared_mutex>
#include <string>
#include <thread>  // NOLINT [build/c++11]
#include <unordered_map>

#include "lifecyclemgr/abilityclient.h"
#include "lifecyclemgr/command_info.h"
#include "lifecyclemgr/heartbeat_info.h"

class LifeCycleManager {
   public:
    /**
     * @brief handle the command info
     * @param cmd_info
     *
     * @return 1: ok , 0 : error , 2 : last operation has not ended
     */
    int HandleCommandInfo(const CommandInfo &cmd_info);
    /**
     * @brief add a heart beart info
     * @param info
     * @return
     */
    bool AddHeartbeatInfo(HeartbeatInfo info);
    /**
     * @brief
     * @param callback
     */
    void Init(std::function<bool(std::string)> callback1, std::function<bool(unsigned long)> callback2);
    /**
     * @brief start lifecycle manager
     */
    void Run();
    /**
     * @brief get the heart beat map
     * @return
     */
    std::string GetHeartbeatMap();

   private:
    // store abilityInstanceId and async thread
    std::unordered_map<size_t, std::future<void>> threads;
    std::shared_mutex thread_lock_;
    // store abilityInstanceId and ability client (by grpc)
    std::unordered_map<size_t, std::shared_ptr<AbilityClient>> clients;
    std::shared_mutex clients_lock_;

    // abilityInstance and the heart beat info
    std::unordered_map<size_t, HeartbeatInfo> heartbeat_map;
    std::shared_mutex heartbeat_map_lock;

    std::thread checkClientThread;

    // resource manager check resource exist function
    std::function<bool(std::string)> resourcemgr_checkexist;
    std::function<bool(unsigned long)> relationmgr_checkexist;

    void lifeCycleDeal(std::shared_ptr<AbilityClient> client, const HeartbeatInfo &hbinfo, const CommandInfo &cmdinfo);

    bool start_process(const std::string &abilityName, unsigned long abilityId, unsigned long abilityInstanceId);

    void checkTimeout();

    void createClient(const std::string &name, int ipcport, unsigned long abilityInstanceId);
};

#endif  // LIFECYCLEMGR_LIFECYCLE_MANAGER_H_
