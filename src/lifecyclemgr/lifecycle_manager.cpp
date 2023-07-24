#include "lifecycle_manager.h"

#include "glog/logging.h"

#include "global_var.h"
#include "color.h"

void LifeCycleManager::HandleCommandInfo(const CommandInfo &cmd_info)
{
    //a new process will be start
    if (cmd_info.cmd == CMD_START && resourcemgr_checkexist(cmd_info.abilityName) && heartbeat_map.count(cmd_info.abilityName) == 0)
    {
        LOG(INFO) << "ready for start : " << cmd_info.abilityName << " process";
        start_process(cmd_info.abilityName);
        // 稍微等待500ms
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    if (heartbeat_map.count(cmd_info.abilityName) == 0)
    {
        LOG(INFO) << "heartbeat map not exist the ability : " << cmd_info.abilityName << " info , wait 1 seconds";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    if (heartbeat_map.count(cmd_info.abilityName) == 0)
    {
        LOG(ERROR) << "heartbeat map not exist the ability : " << cmd_info.abilityName << " info , please check process";
        return;
    }
    else
    {
        if (heartbeat_map[cmd_info.abilityName].IsOffline())
        {
            LOG(ERROR) << "the ability : " << cmd_info.abilityName << " is offline";
            return;
        }
    }
    if (threads.count(cmd_info.abilityName) == 0)
    {
        clients[cmd_info.abilityName] = std::make_unique<AbilityClient>(grpc::CreateChannel("localhost:" + std::to_string(heartbeat_map[cmd_info.abilityName].IPCPort), grpc::InsecureChannelCredentials()));
        LOG(INFO) << RED << "Generate Client to Control LC : Port: " << heartbeat_map[cmd_info.abilityName].IPCPort << NONE;
        threads[cmd_info.abilityName] = std::thread([this, cmd_info]()
                                             { this->lifeCycleDeal(*clients[cmd_info.abilityName], heartbeat_map.at(cmd_info.abilityName), cmd_info); });
        LOG(INFO) << RED << "Finished Control Process" << NONE;
    }
    else
    {
        // Logic to update process status
        LOG(INFO) << RED << "Adjust life cycle" << NONE;
        lifeCycleDeal(*clients[cmd_info.abilityName], heartbeat_map.at(cmd_info.abilityName), cmd_info);
        LOG(INFO) << RED << "Finished life cycle adjust" << NONE;
    }
}

bool LifeCycleManager::AddHeartbeatInfo(HeartbeatInfo info)
{
    std::lock_guard<std::mutex> locker(heartbeat_map_lock);
    for (auto it = heartbeat_map.begin(); it != heartbeat_map.end();)
    {
        if (it->second.IsOffline())
        {
            threads[it->first].join(); // ensure the thread is finished
            threads.erase(it->first);  // this process has ended, clean up the thread
            clients.erase(it->first);  // this process has ended, clean up the client
            LOG(INFO) << "the ability : " << it->second.abilityName << " timeout, already clean";
            heartbeat_map.erase(it++); // timeout, delete the record
        }
        else
        {
            ++it;
        }
    }
    heartbeat_map[info.abilityName] = info;
    return true;
}

void LifeCycleManager::Init(std::function<bool(std::string)> callback)
{
    LOG(INFO) << L_GREEN << "init lifecycle manager" << NONE;
    this->resourcemgr_checkexist = callback;
    // this->resourcemgr_checkexist("camera");
}

std::string LifeCycleManager::GetHeartbeatMap()
{
    std::lock_guard<std::mutex> locker(heartbeat_map_lock);
    auto now = std::chrono::steady_clock::now();
    for (auto it = heartbeat_map.begin(); it != heartbeat_map.end();)
    {
        if (it->second.IsOffline())
        {
            threads[it->first].join(); // ensure the thread is finished
            threads.erase(it->first);  // this process has ended, clean up the thread
            clients.erase(it->first);  // this process has ended, clean up the client
            LOG(INFO) << "the ability : " << it->second.abilityName << " timeout, already clean";
            heartbeat_map.erase(it++); // timeout, delete the record
        }
        else
        {
            ++it;
        }
    }
    Json::Value data;
    for (const auto &iter : this->heartbeat_map)
    {
        data.append(iter.second.toJson(iter.second.IPCPort));
    }
    return data.toStyledString();
}

void LifeCycleManager::lifeCycleDeal(AbilityClient &client, HeartbeatInfo &hbinfo, const CommandInfo &cmdinfo)
{
    LOG(INFO) << RED << "Now status of Ability is :" << hbinfo.status << NONE << std::endl;
    if (hbinfo.status == STATUS_INIT)
    {
        abilityUnit::StartInfo start_info;
        start_info.set_timestamp(time(0));
        client.Start(start_info);
    }
    if (hbinfo.abilityName == cmdinfo.abilityName)
    {
        if (hbinfo.status == STATUS_STANDBY)
        {
            if (cmdinfo.cmd == CMD_CONNECT)
            {
                abilityUnit::ConnectInfo connect_info;
                connect_info.set_ip(cmdinfo.connectIP);
                connect_info.set_port(cmdinfo.connectPort);
                connect_info.set_timestamp(time(0));
                client.Connect(connect_info);
            }
            else if (cmdinfo.cmd == CMD_TERMINATE)
            {
                abilityUnit::TerminateInfo terminate_info;
                terminate_info.set_timestamp(time(0));
                client.Terminate(terminate_info);
            }
            else
            {
            }
        }
        else if (hbinfo.status == STATUS_RUNNING)
        {
            if (cmdinfo.cmd == CMD_DISCONNECT)
            {
                abilityUnit::DisconnectInfo disconnect_info;
                disconnect_info.set_timestamp(time(0));
                client.Disconnect(disconnect_info);
            }
            else if (cmdinfo.cmd == CMD_TERMINATE)
            {
                abilityUnit::TerminateInfo terminate_info;
                terminate_info.set_timestamp(time(0));
                client.Terminate(terminate_info);
            }
            else
            {
            }
        }
        else if (hbinfo.status == STATUS_SUSPEND)
        {
            if (cmdinfo.cmd == CMD_CONNECT)
            {
                abilityUnit::ConnectInfo connect_info;
                connect_info.set_ip(cmdinfo.connectIP);
                connect_info.set_port(cmdinfo.connectPort);
                connect_info.set_timestamp(time(0));
                client.Connect(connect_info);
            }
            else if (cmdinfo.cmd == CMD_TERMINATE)
            {
                abilityUnit::TerminateInfo terminate_info;
                terminate_info.set_timestamp(time(0));
                client.Terminate(terminate_info);
            }
            else if (cmdinfo.cmd == CMD_START)
            {
                abilityUnit::StartInfo start_info;
                start_info.set_timestamp(time(0));
                client.Start(start_info);
            }
        }
        else if (hbinfo.status == STATUS_TERMINATE)
        {
        }
    }
}

bool LifeCycleManager::start_process(const std::string &abilityName)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        // 错误处理：fork失败
        return false;
    }
    else if (pid > 0)
    {
        // 我们在父进程中，pid是子进程的PID
    }
    else
    {
        std::string program_path = abilityName;
        LOG(INFO) << "start process : " << abilityName;
        // 我们在子进程中，启动新的程序
        execl(program_path.c_str(), program_path.c_str(), (char *)NULL);
        // 如果execl返回，那么说明出错了
        return false;
    }
    return true;
}
