/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: abilityclient.cpp
 * Description: abilityclient.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "lifecyclemgr/abilityclient.h"

#include <glog/logging.h>

void AbilityClient::Start(const abilityUnit::StartInfo &start_info)
{
    abilityUnit::Response response;
    ClientContext context;
    Status status = stub_->Start(&context, start_info, &response);
    // Handle status and response here...
    if (!status.ok()) {
        DLOG(ERROR) << "start error : " << status.error_message();
    }
}

void AbilityClient::Connect(const abilityUnit::ConnectInfo &connect_info)
{
    abilityUnit::Response response;
    ClientContext context;
    Status status = stub_->Connect(&context, connect_info, &response);
    // Handle status and response here...
    if (!status.ok()) {
        DLOG(ERROR) << "connect error : " << status.error_message();
    }
}

void AbilityClient::Disconnect(const abilityUnit::DisconnectInfo &disconnect_info)
{
    abilityUnit::Response response;
    ClientContext context;
    Status status = stub_->Disconnect(&context, disconnect_info, &response);
    // Handle status and response here...
    if (!status.ok()) {
        DLOG(ERROR) << "disconnect error : " << status.error_message();
    }
}

void AbilityClient::Terminate(const abilityUnit::TerminateInfo &terminate_info)
{
    abilityUnit::Response response;
    ClientContext context;
    Status status = stub_->Terminate(&context, terminate_info, &response);
    // Handle status and response here...
    if (!status.ok()) {
        DLOG(ERROR) << "terminate error : " << status.error_message();
    }
}
