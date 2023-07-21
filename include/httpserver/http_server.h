#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <memory>

#include "httplib.h"

#include "resource_manager.h"
#include "lifecycle_manager.h"
#include "ability_relation_manager.h"
#include "connection_manager.h"

class HttpServer
{
public:
    void Init(std::shared_ptr<ResourceManager> resource_,
        std::shared_ptr<LifeCycleManager> lifecycle_,
        std::shared_ptr<AbilityRelationManager> relation_,
        std::shared_ptr<ConnectionManager> connection_);
    void Run();

private:
    std::shared_ptr<httplib::Server> server;
    std::shared_ptr<ResourceManager> resource_manager_;
    std::shared_ptr<LifeCycleManager> lifecycle_manager_;
    std::shared_ptr<AbilityRelationManager> relation_manager_;
    std::shared_ptr<ConnectionManager> connection_manager_;
};

#endif // HTTP_SERVER_H