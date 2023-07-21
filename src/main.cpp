#include <iostream>
#include <thread>

#include "glog/logging.h"

#include "resource_manager.h"
#include "lifecycle_manager.h"
#include "connection_manager.h"
#include "http_server.h"
#include "discovery_manager.h"
#include "ability_relation_manager.h"

int main(int argc, char **argv)
{
    auto resource_manager = std::make_shared<ResourceManager>();
    auto lifecycle_manager = std::make_shared<LifeCycleManager>();
    auto connection_manager = std::make_shared<ConnectionManager>();
    auto http_server = std::make_shared<HttpServer>();
    auto discovery_manager = std::make_shared<DiscoveryManager>();
    auto ability_relation_manager = std::make_shared<AbilityRelationManager>();
    discovery_manager->Init(std::bind(&ConnectionManager::OnEndAddressRecordChange, connection_manager, std::placeholders::_1),
                            std::bind(&ResourceManager::EndAddressDiscoveryResult, resource_manager, std::placeholders::_1));
    resource_manager->Init(connection_manager);
    lifecycle_manager->Init(std::bind(&ResourceManager::AbilityExistJudge, resource_manager, std::placeholders::_1));
    
    http_server->Init(resource_manager, lifecycle_manager, ability_relation_manager, connection_manager);
    return 0;
}
