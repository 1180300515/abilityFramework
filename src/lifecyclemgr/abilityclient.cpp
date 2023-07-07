#include "abilityclient.h"

void AbilityClient::Start(const abilityUnit::StartInfo &start_info)
{
    abilityUnit::Response response;
    ClientContext context;
    Status status = stub_->Start(&context, start_info, &response);
    // Handle status and response here...
}

void AbilityClient::Connect(const abilityUnit::ConnectInfo &connect_info)
{
    abilityUnit::Response response;
    ClientContext context;
    Status status = stub_->Connect(&context, connect_info, &response);
    // Handle status and response here...
}

void AbilityClient::Disconnect(const abilityUnit::DisconnectInfo &disconnect_info)
{
    abilityUnit::Response response;
    ClientContext context;
    Status status = stub_->Disconnect(&context, disconnect_info, &response);
    // Handle status and response here...
}

void AbilityClient::Terminate(const abilityUnit::TerminateInfo &terminate_info)
{
    abilityUnit::Response response;
    ClientContext context;
    Status status = stub_->Terminate(&context, terminate_info, &response);
    // Handle status and response here...
}