#ifndef HTTPSERVER_HTTP_SERVER_H_
#define HTTPSERVER_HTTP_SERVER_H_

#include <httplib.h>

#include <memory>

#include "abilityrelationmgr/ability_relation_manager.h"
#include "lifecyclemgr/lifecycle_manager.h"
#include "resourcemgr/resource_manager.h"

class HttpServer {
 public:
  void Init(std::shared_ptr<ResourceManager> resource_,
            std::shared_ptr<LifeCycleManager> lifecycle_,
            std::shared_ptr<AbilityRelationManager> relation_);
  void Run();

 private:
  std::shared_ptr<httplib::Server> server;
  std::shared_ptr<ResourceManager> resource_manager_;
  std::shared_ptr<LifeCycleManager> lifecycle_manager_;
  std::shared_ptr<AbilityRelationManager> relation_manager_;
};

#endif  // HTTPSERVER_HTTP_SERVER_H_
