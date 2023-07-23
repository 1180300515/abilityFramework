#include "lifecycle_manager.h"

#include "glog/logging.h"

#include "global_var.h"
#include "color.h"

void LifeCycleManager::HandleCommandInfo(std::optional<CommandInfo> cmd_info)
{
    if (cmd_info.value().cmd == CMD_START && resourcemgr_checkexist(cmd_info.value().abilityName))
    {
        start_process(cmd_info.value().abilityName);
    }
    std::lock_guard<std::mutex> locker(heartbeat_map_lock);

    // check timeout and clean record
    auto now = std::chrono::steady_clock::now();
    for (auto it = heartbeat_map.begin(); it != heartbeat_map.end();)
    {
        if (now - it->second.last_update > std::chrono::seconds(20))
        {
            heartbeat_map.erase(it);   // timeout, delete the record
            threads[it->first].join(); // ensure the thread is finished
            threads.erase(it->first);  // this process has ended, clean up the thread
            clients.erase(it->first);  // this process has ended, clean up the client
            LOG(INFO) << "the ability : " << it->second.abilityName << " timeout, already clean";
            ++it;
        }
        else
        {
            ++it;
        }
    }

    for (auto &hb_entry : heartbeat_map)
    {
        int port = hb_entry.first;
        HeartbeatInfo &hb = hb_entry.second;
        if (port == 0)
        {
            continue;
        }

        if (threads.count(port) == 0 && port != 0)
        {
            clients[port] = std::make_unique<AbilityClient>(grpc::CreateChannel(
                "localhost:" + std::to_string(port), grpc::InsecureChannelCredentials()));
            LOG(INFO) << RED << "Generate Client to Control LC" << NONE ;
            threads[port] = std::thread([this, port, &cmd_info]()
                                        { this->lifeCycleDeal(*clients[port], port, heartbeat_map.at(port), cmd_info); });
            LOG(INFO) << RED << "Finished Control Process" << NONE ;
        }
        else
        {
            // Logic to update process status
            LOG(INFO) << RED << "Adjust life cycle" << NONE ;
            lifeCycleDeal(*clients[port], port, heartbeat_map.at(port), cmd_info);
            LOG(INFO) << RED << "Finished life cycle adjust" << NONE ;
        }
    }
}

bool LifeCycleManager::AddHeartbeatInfo(int port, HeartbeatInfo info)
{
    std::lock_guard<std::mutex> locker(heartbeat_map_lock);
    heartbeat_map[port] = info;
    return true;
}

void LifeCycleManager::Init(std::function<bool(std::string)> callback)
{
    LOG(INFO) << L_GREEN << "init lifecycle manager" << NONE;
    this->resourcemgr_checkexist = callback;
}

std::string LifeCycleManager::GetHeartbeatMap()
{
    std::lock_guard<std::mutex> locker(heartbeat_map_lock);
    Json::Value data;
    for (const auto &iter : this->heartbeat_map)
    {
        data.append(iter.second.toJson(iter.first));
    }
    return data.toStyledString();
}

void LifeCycleManager::lifeCycleDeal(AbilityClient &client, const int &port, HeartbeatInfo &hbinfo, std::optional<CommandInfo> &cmdinfo)
{
    LOG(INFO) << RED << "Now status of Ability is :" << hbinfo.status << NONE << std::endl;
    if (hbinfo.status == STATUS_INIT)
    {
        abilityUnit::StartInfo start_info;
        start_info.set_timestamp(time(0));
        client.Start(start_info);
    }
    if (cmdinfo && port == cmdinfo->port && hbinfo.abilityName == cmdinfo->abilityName)
    {
        if (hbinfo.status == STATUS_STANDBY)
        {
            if (cmdinfo->cmd == CMD_CONNECT)
            {
                abilityUnit::ConnectInfo connect_info;
                connect_info.set_ip(cmdinfo->connectIP);
                connect_info.set_port(cmdinfo->connectPort);
                connect_info.set_timestamp(time(0));
                client.Connect(connect_info);
                cmdinfo.reset();
            }
            else if (cmdinfo->cmd == CMD_TERMINATE)
            {
                abilityUnit::TerminateInfo terminate_info;
                terminate_info.set_timestamp(time(0));
                client.Terminate(terminate_info);
                cmdinfo.reset();
            }
            else
            {
                cmdinfo.reset();
            }
        }
        else if (hbinfo.status == STATUS_RUNNING)
        {
            if (cmdinfo->cmd == CMD_DISCONNECT)
            {
                abilityUnit::DisconnectInfo disconnect_info;
                disconnect_info.set_timestamp(time(0));
                client.Disconnect(disconnect_info);
                cmdinfo.reset();
            }
            else if (cmdinfo->cmd == CMD_TERMINATE)
            {
                abilityUnit::TerminateInfo terminate_info;
                terminate_info.set_timestamp(time(0));
                client.Terminate(terminate_info);
                cmdinfo.reset();
            }
            else
            {
            }
        }
        else if (hbinfo.status == STATUS_SUSPEND)
        {
            if (cmdinfo->cmd == CMD_CONNECT)
            {
                abilityUnit::ConnectInfo connect_info;
                connect_info.set_ip(cmdinfo->connectIP);
                connect_info.set_port(cmdinfo->connectPort);
                connect_info.set_timestamp(time(0));
                client.Connect(connect_info);
                cmdinfo.reset();
            }
            else if (cmdinfo->cmd == CMD_TERMINATE)
            {
                abilityUnit::TerminateInfo terminate_info;
                terminate_info.set_timestamp(time(0));
                client.Terminate(terminate_info);
                cmdinfo.reset();
            }
            else
            {
                cmdinfo.reset();
            }
        }
        else if (hbinfo.status == STATUS_TERMINATE)
        {
            cmdinfo.reset();
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
        std::string program_path = "bin/" + abilityName;
        // 我们在子进程中，启动新的程序
        execl(program_path.c_str(), program_path.c_str(), (char *)NULL);
        // 如果execl返回，那么说明出错了
        return false;
    }
    return true;
}
