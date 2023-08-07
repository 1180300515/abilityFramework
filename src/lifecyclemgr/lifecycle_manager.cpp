#include "lifecycle_manager.h"

#include "glog/logging.h"

#include "global_var.h"
#include "color.h"

void LifeCycleManager::HandleCommandInfo(const std::string &cmd)
{
    //LOG(INFO) << "receive cmd : " << cmd;
    Json::Value root;
    Json::Reader reader;
    bool success = reader.parse(cmd,root);
    if (!success)
    {
        LOG(ERROR) << "json parse error";
        return;
    }
    CommandInfo cmd_info;
    cmd_info.FromJson(root);
    LOG(INFO) << "handle the command info : " << cmd_info.toJson().toStyledString();
    if (!resourcemgr_checkexist(cmd_info.abilityName))
    {
        LOG(ERROR) << "the ability : " << cmd_info.abilityName << " is not exist";
        return;
    }
    if (cmd_info.IPCPort == 0 && cmd_info.cmd != CMD_START)
    {
        LOG(ERROR) << "the cmd info must specify the port number";
        return;
    }
    if (cmd_info.IPCPort != 0 && this->clients.count(cmd_info.IPCPort) == 0)
    {
        LOG(ERROR) << "no ability client matching the port : " << cmd_info.IPCPort;
        return;
    }
    // a new process will be start
    if (cmd_info.IPCPort == 0 && cmd_info.cmd == CMD_START)
    {
        LOG(INFO) << "ready for start : " << cmd_info.abilityName << " process";
        if (!start_process(cmd_info.abilityName))
        {
            LOG(ERROR) << "start the " << cmd_info.abilityName << " fail";
            return;
        }
    }
    std::lock_guard<std::mutex> locker(this->heartbeat_map_lock);
    for (const auto &iter : this->heartbeat_map)
    {
        // clean timeout
        if (iter.second.IsOffline())
        {
            continue;
        }
        else if (iter.second.abilityName == cmd_info.abilityName && this->clients.count(iter.first) == 0)
        {
            std::lock_guard<std::mutex> locker1(this->clients_lock_);
            clients[iter.first] = std::make_unique<AbilityClient>(grpc::CreateChannel("localhost:" + std::to_string(iter.first),
                                                                                      grpc::InsecureChannelCredentials()));
            LOG(INFO) << "create ability client for ability : " << cmd_info.abilityName << " in IPCPort : " << iter.first;
            threads[iter.first] = std::thread(
                [this, iter, cmd_info]()
                {
                    this->lifeCycleDeal(*clients[iter.first], this->heartbeat_map.at(iter.first), cmd_info);
                });
        }
        else if (iter.second.abilityName == cmd_info.abilityName && iter.first == cmd_info.IPCPort)
        {
            LOG(INFO) << RED << "Adjust life cycle" << NONE;
            lifeCycleDeal(*clients[iter.first], this->heartbeat_map.at(iter.first), cmd_info);
            LOG(INFO) << RED << "Finished life cycle adjust" << NONE;
        }
    }
}

bool LifeCycleManager::AddHeartbeatInfo(HeartbeatInfo info)
{
    std::lock_guard<std::mutex> locker(heartbeat_map_lock);
    heartbeat_map[info.IPCPort] = info;
    return true;
}

void LifeCycleManager::Init(std::function<bool(std::string)> callback)
{
    LOG(INFO) << L_GREEN << "init lifecycle manager" << NONE;
    this->resourcemgr_checkexist = callback;
    // this->resourcemgr_checkexist("camera");
}

void LifeCycleManager::Run()
{
    this->checkClientThread = std::thread(&LifeCycleManager::checkAbilityClientAndTimeout, this);
}

std::string LifeCycleManager::GetHeartbeatMap()
{
    std::lock_guard<std::mutex> locker(heartbeat_map_lock);
    Json::Value data;
    for (const auto &iter : this->heartbeat_map)
    {
        if (iter.second.IsOffline())
        {
            continue;
        }
        data.append(iter.second.toJson());
    }
    return data.toStyledString();
}

void LifeCycleManager::lifeCycleDeal(AbilityClient &client, HeartbeatInfo &hbinfo, const CommandInfo &cmdinfo)
{
    LOG(INFO) << RED << "Now the Ability : " << cmdinfo.abilityName << " in port: " << hbinfo.IPCPort << " status is :" << hbinfo.status << NONE << std::endl;
    if (hbinfo.status == STATUS_INIT)
    {
        if (cmdinfo.cmd == CMD_START)
        {
            abilityUnit::StartInfo start_info;
            start_info.set_timestamp(time(0));
            client.Start(start_info);
        }
        else
        {
            LOG(ERROR) << "illegal cmd : " << cmdinfo.cmd << " in status: " << hbinfo.status;
        }
    }
    else if (hbinfo.status == STATUS_STANDBY)
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
            LOG(ERROR) << "illegal cmd : " << cmdinfo.cmd << " in status: " << hbinfo.status;
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
            LOG(ERROR) << "illegal cmd : " << cmdinfo.cmd << " in status: " << hbinfo.status;
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
        else
        {
            LOG(ERROR) << "illegal cmd : " << cmdinfo.cmd << " in status: " << hbinfo.status;
        }
    }
    else if (hbinfo.status == STATUS_TERMINATE)
    {
        LOG(INFO) << "status : " << hbinfo.status << " is uncontrollable";
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
        execl(("bin/" + program_path).c_str(), program_path.c_str(), (char *)NULL);
        // 如果execl返回，那么说明出错了,需要终结子进程
        perror("execl fail : ");
        exit(0);
    }
    return true;
}

void LifeCycleManager::checkAbilityClientAndTimeout()
{
    while (true)
    {
        {
            std::lock_guard<std::mutex> locker(this->heartbeat_map_lock);
            std::lock_guard<std::mutex> locker1(this->clients_lock_);
            for (auto iter = this->heartbeat_map.begin(); iter != this->heartbeat_map.end();)
            {
                // clean timeout
                if (iter->second.IsOffline())
                {               
                    threads[iter->first].join(); // ensure the thread is finished
                    threads.erase(iter->first);  // this process has ended, clean up the thread
                    clients.erase(iter->first);  // this process has ended, clean up the client
                    LOG(INFO) << "the ability : " << iter->second.abilityName << "on IPCPort : " << iter->first << "  timeout, already clean";
                    heartbeat_map.erase(iter++); // timeout, delete the record
                }
                else if (this->clients.count(iter->first) == 0 && iter->second.status == STATUS_INIT)
                {
                    clients[iter->first] = std::make_unique<AbilityClient>(grpc::CreateChannel("localhost:" + std::to_string(iter->first),
                                                                                               grpc::InsecureChannelCredentials()));
                    LOG(INFO) << "create ability client for ability : " << iter->second.abilityName << " in IPCPort : " << iter->first;
                    threads[iter->first] = std::thread(
                        [this, iter]()
                        {
                            abilityUnit::StartInfo start_info;
                            start_info.set_timestamp(time(0));
                            this->clients[iter->first]->Start(start_info);
                        });
                    ++iter;
                }
                else
                {
                    ++iter;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}
