#include "http_server.h"

#include "glog/logging.h"

#include "color.h"

void HttpServer::Init(std::shared_ptr<ResourceManager> resource_,
                      std::shared_ptr<LifeCycleManager> lifecycle_,
                      std::shared_ptr<AbilityRelationManager> relation_)
{
    LOG(INFO) << L_GREEN << "init http server" << NONE;
    this->server = std::make_shared<httplib::Server>();
    this->resource_manager_ = resource_;
    this->lifecycle_manager_ = lifecycle_;
    this->relation_manager_ = relation_;
    this->server->Post("/heartbeat",
                       [this](const httplib::Request &req, httplib::Response &res)
                       {
                           LOG(INFO) << L_BLUE << "httpserver receive: method: Post   URL: /heartbeat" << NONE;
                           int port;
                           try
                           {
                               port = std::stoi(req.get_param_value("IPCPort"));
                               LOG(INFO) << BLUE << "Got heartbeat from port " << port << NONE;
                           }
                           catch (const std::exception &)
                           {
                               res.status = 400; // Bad Request
                               return;
                           }
                           auto new_info = HeartbeatInfo{req.get_param_value("abilityName"), std::stoi(req.get_param_value("IPCPort")), std::stoi(req.get_param_value("abilityPort")), req.get_param_value("status"), std::chrono::steady_clock::now()};
                           this->lifecycle_manager_->AddHeartbeatInfo(new_info);
                       });

    this->server->Get("/api/Devices",
                      [this](const httplib::Request &req, httplib::Response &res)
                      {
                          LOG(INFO) << L_BLUE << "httpserver receive: method: GET   URL: /api/Devices" << NONE;
                          res.set_content(this->resource_manager_->GetHardwareDeviceInfo(true), "application/json");
                      });

    this->server->Get("/api/AbilityRunning",
                      [this](const httplib::Request &req, httplib::Response &res)
                      {
                          LOG(INFO) << L_BLUE << "httpserver receive: method: GET   URL: /api/AbilityRunning" << NONE;
                          res.set_content(this->lifecycle_manager_->GetHeartbeatMap(), "application/json");
                      });

    this->server->Get("/api/AbilitySupport",
                      [this](const httplib::Request &req, httplib::Response &res)
                      {
                          LOG(INFO) << L_BLUE << "httpserver receive: method: GET   URL: /api/AbilitySupport" << NONE;
                          res.set_content(this->relation_manager_->GetAbilitySupport(), "application/json");
                      });

    this->server->Post("/api/AbilityRequest",
                       [this](const httplib::Request &req, httplib::Response &res)
                       {
                           LOG(INFO) << L_BLUE << "httpserver receive: method: POST   URL: /api/AbilityRequest" << NONE;
                           this->lifecycle_manager_->HandleCommandInfo(req.body);
                       });
}

void HttpServer::Run()
{
    LOG(INFO) << L_GREEN << "http server start" << NONE;
    this->server->listen("0.0.0.0", 8080);
}
