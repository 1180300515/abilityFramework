/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: abilityclient.h
 * Description: abilityclient.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef LIFECYCLEMGR_ABILITYCLIENT_H_
#define LIFECYCLEMGR_ABILITYCLIENT_H_

#include <grpcpp/grpcpp.h>

#include <memory>

#include "abilityProto.grpc.pb.h"
#include "abilityProto.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class AbilityClient {
   public:
    explicit AbilityClient(std::shared_ptr<Channel> channel) : stub_(abilityUnit::Ability::NewStub(channel)) {}

    void Start(const abilityUnit::StartInfo &start_info);

    void Connect(const abilityUnit::ConnectInfo &connect_info);

    void Disconnect(const abilityUnit::DisconnectInfo &disconnect_info);

    void Terminate(const abilityUnit::TerminateInfo &terminate_info);

   private:
    std::unique_ptr<abilityUnit::Ability::Stub> stub_;
};

#endif  // LIFECYCLEMGR_ABILITYCLIENT_H_
