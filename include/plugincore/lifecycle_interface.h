#ifndef __LIFECYCLE_INTERFACE_H__
#define __LIFECYCLE_INTERFACE_H__

#include <unordered_map>
#include <thread>
#include <string>
#include <iostream>
#include <memory>
#include <optional>

#include <grpcpp/grpcpp.h>
#include "abimgr_interface.h"
#include "abilityProto.pb.h"
#include "abilityProto.grpc.pb.h"
#include "discoverymanager/discmgr_interface.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

#define STATUS_INIT "INIT"
#define STATUS_STANDBY "STANDBY"
#define STATUS_RUNNING "RUNNING"
#define STATUS_SUSPEND "SUSPEND"
#define STATUS_TERMINATE "TERMINATE"

#define CMD_CONNECT "connect"
#define CMD_DISCONNECT "disconnect"
#define CMD_TERMINATE "terminate"

class AbilityClient
{
public:
    AbilityClient(std::shared_ptr<Channel> channel)
        : stub_(abilityUnit::Ability::NewStub(channel)) {}

    void Start(const abilityUnit::StartInfo &start_info)
    {
        abilityUnit::Response response;
        ClientContext context;
        Status status = stub_->Start(&context, start_info, &response);
        // Handle status and response here...
    }

    void Connect(const abilityUnit::ConnectInfo &connect_info)
    {
        abilityUnit::Response response;
        ClientContext context;
        Status status = stub_->Connect(&context, connect_info, &response);
        // Handle status and response here...
    }

    void Disconnect(const abilityUnit::DisconnectInfo &disconnect_info)
    {
        abilityUnit::Response response;
        ClientContext context;
        Status status = stub_->Disconnect(&context, disconnect_info, &response);
        // Handle status and response here...
    }

    void Terminate(const abilityUnit::TerminateInfo &terminate_info)
    {
        abilityUnit::Response response;
        ClientContext context;
        Status status = stub_->Terminate(&context, terminate_info, &response);
        // Handle status and response here...
    }

private:
    std::unique_ptr<abilityUnit::Ability::Stub> stub_;
};

class CommandInfo
{
public:
    std::string cmd;
    int connectPort;
    std::string connectIP;
};

class ProcessController
{
public:
    void handleHeartbeat(std::unordered_map<int, HeartbeatInfo> &heartbeat_map, std::optional<CommandInfo> cmd_info)
    {
        // clean up the ended programs
        for (auto it = clients.begin(); it != clients.end();)
        {
            if (heartbeat_map.find(it->first) == heartbeat_map.end())
            {
                it = clients.erase(it);    // this process has ended, clean up the client
                threads[it->first].join(); // ensure the thread is finished
                threads.erase(it->first);  // this process has ended, clean up the thread
            }
            else
            {
                ++it;
            }
        }
        std::cout << RED << "Finished clean up programs" << NONE << std::endl;

        for (auto &hb_entry : heartbeat_map)
        {
            int port = hb_entry.first;
            HeartbeatInfo &hb = hb_entry.second;
            if (port == 0)
                continue; // skip for program initialization
            if (threads.count(port) == 0 && port != 0)
            {
                clients[port] = std::make_unique<AbilityClient>(grpc::CreateChannel(
                    "localhost:" + std::to_string(port), grpc::InsecureChannelCredentials()));
                std::cout << RED << "Generate Client to Control LC" << NONE << std::endl;
                threads[port] = std::thread([this, port, &heartbeat_map, &cmd_info]()
                                            { controlProcess(*clients[port], port, heartbeat_map.at(port), cmd_info); });
                std::cout << RED << "Finished Control Process" << NONE << std::endl;
            }
            else
            {
                // Logic to update process status
                std::cout << RED << "Adjust LC in Control Process" << NONE << std::endl;
                controlProcess(*clients[port], port, heartbeat_map.at(port), cmd_info);
                std::cout << RED << "Finished Control Process" << NONE << std::endl;
            }
        }
    }

    // ...

private:
    void controlProcess(AbilityClient &client, const int &port, HeartbeatInfo &hbinfo, std::optional<CommandInfo> &cmdinfo)
    {
        std::cout << RED << "Now status of Ability is :" << hbinfo.status << NONE << std::endl;
        if (hbinfo.status == STATUS_INIT)
        {
            abilityUnit::StartInfo start_info;
            start_info.set_timestamp(time(0));
            client.Start(start_info);
        }
        if (cmdinfo)
        {
            if (hbinfo.status == STATUS_STANDBY)
            {
                if (cmdinfo->cmd == CMD_CONNECT)
                {
                    abilityUnit::ConnectInfo connect_info;
                    connect_info.set_ip(cmdinfo->connectIP);
                    connect_info.set_port(cmdinfo->connectPort);
                    client.Connect(connect_info);
                }
                else if (cmdinfo->cmd == CMD_TERMINATE)
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
                if (cmdinfo->cmd == CMD_DISCONNECT)
                {
                    abilityUnit::DisconnectInfo disconnect_info;
                    disconnect_info.set_timestamp(time(0));
                    client.Disconnect(disconnect_info);
                }
                else if (cmdinfo->cmd == CMD_TERMINATE)
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
                if (cmdinfo->cmd == CMD_CONNECT)
                {
                    abilityUnit::ConnectInfo connect_info;
                    connect_info.set_ip(cmdinfo->connectIP);
                    connect_info.set_port(cmdinfo->connectPort);
                    client.Connect(connect_info);
                }
                else if (cmdinfo->cmd == CMD_TERMINATE)
                {
                    abilityUnit::TerminateInfo terminate_info;
                    terminate_info.set_timestamp(time(0));
                    client.Terminate(terminate_info);
                }
                else
                {
                }
            }
            else if (hbinfo.status == STATUS_TERMINATE)
            {
            }
        }
    }

    std::unordered_map<int, std::thread> threads;
    std::unordered_map<int, std::unique_ptr<AbilityClient>> clients;
};

class AbilityStatus
{
public:
};



#endif // __LIFECYCLE_INTERFACE_H__