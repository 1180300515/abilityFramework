/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: lifecycle_manager.cpp
 * Description: lifecycle_manager.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "lifecyclemgr/lifecycle_manager.h"

#include <glog/logging.h>

#include "utils/color.h"
#include "utils/global_var.h"

int LifeCycleManager::HandleCommandInfo(const CommandInfo &cmd_info) {
  // DLOG(INFO) << "receive cmd : " << cmd;
  DLOG(INFO) << "handle the command info : "
            << cmd_info.toJson().toStyledString();
  if (!resourcemgr_checkexist(cmd_info.abilityName)) {
    DLOG(ERROR) << "the ability : " << cmd_info.abilityName << " is not exist";
    return 0;
  }
  if (cmd_info.IPCPort == 0 && cmd_info.cmd != CMD_START) {
    DLOG(ERROR) << "the cmd info must specify the port number";
    return 0;
  }
  // a new process will be start
  if (cmd_info.IPCPort == 0 && cmd_info.cmd == CMD_START) {
    DLOG(INFO) << "ready for start : " << cmd_info.abilityName << " process";
    if (!start_process(cmd_info.abilityName)) {
      DLOG(ERROR) << "start the " << cmd_info.abilityName << " fail";
      return 0;
    }
    return 1;
  }
  std::shared_lock<std::shared_mutex> lock(clients_lock_);
  std::shared_lock<std::shared_mutex> lock1(heartbeat_map_lock);
  if (cmd_info.IPCPort != 0 && this->clients.count(cmd_info.IPCPort) == 0) {
    DLOG(ERROR) << "no ability client matching the port : " << cmd_info.IPCPort;
    return 0;
  }
  if (heartbeat_map.count(cmd_info.IPCPort) == 0) {
    DLOG(ERROR) << "the IPCPort: " << cmd_info.IPCPort << " is not exist";
    return 0;
  }
  if (heartbeat_map.at(cmd_info.IPCPort).abilityName == cmd_info.abilityName) {
    std::unique_lock<std::shared_mutex> lock1(thread_lock_);
    std::future_status status =
        threads.at(cmd_info.IPCPort).wait_for(std::chrono::seconds(0));
    if (status != std::future_status::ready) {
      DLOG(ERROR) << "The last operation has not ended in IPCPort: "
                 << cmd_info.IPCPort;
      return 2;
    }
    threads.at(cmd_info.IPCPort) =
        std::async(std::launch::async, &LifeCycleManager::lifeCycleDeal, this,
                   clients[cmd_info.IPCPort],
                   this->heartbeat_map.at(cmd_info.IPCPort), cmd_info);
    return 1;
  }
  return 0;
}

bool LifeCycleManager::AddHeartbeatInfo(HeartbeatInfo info) {
  if (info.IPCPort == 0) {
    DLOG(ERROR) << "the heartbeat info IPCPort is illegal";
    return false;
  }
  {
    std::unique_lock<std::shared_mutex> locker(heartbeat_map_lock);
    heartbeat_map[info.IPCPort] = info;
  }
  bool tag = false;
  {
    std::shared_lock<std::shared_mutex> lock(clients_lock_);
    tag = (clients.count(info.IPCPort) == 0);
  }
  if (tag) {
    std::unique_lock<std::shared_mutex> lock1(thread_lock_);
    threads[info.IPCPort] =
        std::async(std::launch::async, &LifeCycleManager::createClient, this,
                   info.abilityName, info.IPCPort);
  }
  return true;
}

void LifeCycleManager::Init(std::function<bool(std::string)> callback) {
  DLOG(INFO) << L_GREEN << "init lifecycle manager" << NONE;
  this->resourcemgr_checkexist = callback;
  // this->resourcemgr_checkexist("camera");
}

void LifeCycleManager::Run() {
  this->checkClientThread = std::thread(&LifeCycleManager::checkTimeout, this);
}

std::string LifeCycleManager::GetHeartbeatMap() {
  std::shared_lock<std::shared_mutex> locker(heartbeat_map_lock);
  Json::Value data;
  for (const auto &iter : this->heartbeat_map) {
    if (iter.second.IsOffline()) {
      continue;
    }
    data.append(iter.second.toJson());
  }
  return data.toStyledString();
}

void LifeCycleManager::lifeCycleDeal(std::shared_ptr<AbilityClient> client,
                                     const HeartbeatInfo &hbinfo,
                                     const CommandInfo &cmdinfo) {
  DLOG(INFO) << RED << "Adjust life cycle" << NONE;
  DLOG(INFO) << RED << "Now the Ability : " << cmdinfo.abilityName
            << " in port: " << hbinfo.IPCPort << " status is :" << hbinfo.status
            << NONE << std::endl;
  if (hbinfo.status == STATUS_INIT) {
    if (cmdinfo.cmd == CMD_START) {
      abilityUnit::StartInfo start_info;
      start_info.set_timestamp(time(0));
      client->Start(start_info);
    } else {
      DLOG(ERROR) << "illegal cmd : " << cmdinfo.cmd
                 << " in status: " << hbinfo.status;
    }
  } else if (hbinfo.status == STATUS_STANDBY) {
    if (cmdinfo.cmd == CMD_CONNECT) {
      abilityUnit::ConnectInfo connect_info;
      connect_info.set_ip(cmdinfo.connectIP);
      connect_info.set_port(cmdinfo.connectPort);
      connect_info.set_timestamp(time(0));
      client->Connect(connect_info);
    } else if (cmdinfo.cmd == CMD_TERMINATE) {
      abilityUnit::TerminateInfo terminate_info;
      terminate_info.set_timestamp(time(0));
      client->Terminate(terminate_info);
    } else {
      DLOG(ERROR) << "illegal cmd : " << cmdinfo.cmd
                 << " in status: " << hbinfo.status;
    }
  } else if (hbinfo.status == STATUS_RUNNING) {
    if (cmdinfo.cmd == CMD_DISCONNECT) {
      abilityUnit::DisconnectInfo disconnect_info;
      disconnect_info.set_timestamp(time(0));
      client->Disconnect(disconnect_info);
    } else if (cmdinfo.cmd == CMD_TERMINATE) {
      abilityUnit::TerminateInfo terminate_info;
      terminate_info.set_timestamp(time(0));
      client->Terminate(terminate_info);
    } else {
      DLOG(ERROR) << "illegal cmd : " << cmdinfo.cmd
                 << " in status: " << hbinfo.status;
    }
  } else if (hbinfo.status == STATUS_SUSPEND) {
    if (cmdinfo.cmd == CMD_CONNECT) {
      abilityUnit::ConnectInfo connect_info;
      connect_info.set_ip(cmdinfo.connectIP);
      connect_info.set_port(cmdinfo.connectPort);
      connect_info.set_timestamp(time(0));
      client->Connect(connect_info);
    } else if (cmdinfo.cmd == CMD_TERMINATE) {
      abilityUnit::TerminateInfo terminate_info;
      terminate_info.set_timestamp(time(0));
      client->Terminate(terminate_info);
    } else if (cmdinfo.cmd == CMD_START) {
      abilityUnit::StartInfo start_info;
      start_info.set_timestamp(time(0));
      client->Start(start_info);
    } else {
      DLOG(ERROR) << "illegal cmd : " << cmdinfo.cmd
                 << " in status: " << hbinfo.status;
    }
  } else if (hbinfo.status == STATUS_TERMINATE) {
    DLOG(INFO) << "status : " << hbinfo.status << " is uncontrollable";
  }
  DLOG(INFO) << RED << "Finished life cycle adjust" << NONE;
}

bool LifeCycleManager::start_process(const std::string &abilityName) {
  pid_t pid = fork();

  if (pid < 0) {
    // 错误处理：fork失败
    return false;
  } else if (pid > 0) {
    // 我们在父进程中，pid是子进程的PID
  } else {
    std::string program_path = abilityName;
    DLOG(INFO) << "start process : " << abilityName;
    // 我们在子进程中，启动新的程序
    execl(("bin/" + program_path).c_str(), program_path.c_str(),
          reinterpret_cast<char *>(NULL));
    // 如果execl返回，那么说明出错了,需要终结子进程
    perror("execl fail : ");
    exit(0);
  }
  return true;
}

void LifeCycleManager::checkTimeout() {
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::vector<int> timeoutList;
    {
      std::shared_lock<std::shared_mutex> locker(this->heartbeat_map_lock);
      for (const auto &iter : this->heartbeat_map) {
        if (iter.second.IsOffline()) {
          timeoutList.emplace_back(iter.first);
        }
      }
    }
    if (timeoutList.size() != 0) {
      std::unique_lock<std::shared_mutex> locker1(heartbeat_map_lock);
      std::unique_lock<std::shared_mutex> locker2(clients_lock_);
      std::unique_lock<std::shared_mutex> locker3(thread_lock_);
      for (const auto &iter : timeoutList) {
        if (threads.at(iter).wait_for(std::chrono::seconds(1)) ==
            std::future_status::ready) {
          threads.erase(iter);
        } else {
          if (threads.at(iter).wait_for(std::chrono::seconds(1)) !=
              std::future_status::ready) {
            DLOG(ERROR) << "the thread which bind in IPCPort: " << iter
                       << " cant't finish";
          }
          threads.erase(iter);
        }
        clients.erase(iter);  // this process has ended, clean up the client
        DLOG(INFO) << "the ability : " << heartbeat_map.at(iter).abilityName
                  << "on IPCPort : " << heartbeat_map.at(iter).IPCPort
                  << "  timeout, already clean";
        heartbeat_map.erase(iter);  // timeout, delete the record
      }
    }
  }
}
void LifeCycleManager::createClient(const std::string &name, int ipcport) {
  {
    std::unique_lock<std::shared_mutex> locker(clients_lock_);
    clients[ipcport] = std::make_unique<AbilityClient>(
        grpc::CreateChannel("localhost:" + std::to_string(ipcport),
                            grpc::InsecureChannelCredentials()));
  }
  DLOG(INFO) << "create ability client for ability : " << name
            << " in IPCPort : " << ipcport;
  abilityUnit::StartInfo start_info;
  start_info.set_timestamp(time(0));
  std::shared_lock<std::shared_mutex> locker(clients_lock_);
  this->clients[ipcport]->Start(start_info);
}
