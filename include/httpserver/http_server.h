/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: http_server.h
 * Description: http_server.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef HTTPSERVER_HTTP_SERVER_H_
#define HTTPSERVER_HTTP_SERVER_H_

#include <httplib.h>

#include <memory>

#include "abilityrelationmgr/ability_relation_manager.h"
#include "lifecyclemgr/lifecycle_manager.h"
#include "resourcemgr/resource_manager.h"

class HttpServer {
   public:
    void Init(std::shared_ptr<ResourceManager> resource_, std::shared_ptr<LifeCycleManager> lifecycle_,
              std::shared_ptr<AbilityRelationManager> relation_);
    void Run();

   private:
    std::shared_ptr<httplib::Server> server;
    std::shared_ptr<ResourceManager> resource_manager_;
    std::shared_ptr<LifeCycleManager> lifecycle_manager_;
    std::shared_ptr<AbilityRelationManager> relation_manager_;
};

#endif  // HTTPSERVER_HTTP_SERVER_H_
