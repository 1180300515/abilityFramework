#include "http_server.h"

#include "glog/logging.h"

#include "color.h"

void HttpServer::Init(std::shared_ptr<ResourceManager> resource_, std::shared_ptr<LifeCycleManager> lifecycle_,std::shared_ptr<AbilityRelationManager> relation_,std::shared_ptr<ConnectionManager> connection_)
{
    LOG(INFO) << L_GREEN << "init http server" << NONE;
    this->server = std::make_shared<httplib::Server>();
    this->resource_manager_ = resource_;
    this->lifecycle_manager_ = lifecycle_;
    this->relation_manager_ = relation_;
    this->connection_manager_ = connection_;
    this->server->Post("/heartbeat", [this](const httplib::Request &req, httplib::Response &res)
    {
        LOG(INFO) << L_BLUE << "httpserver receive heartbeat" << NONE;
        int port;
        try {
            port = std::stoi(req.get_param_value("IPCPort"));
            LOG(INFO) << BLUE <<"Got heartbeat from port " << port << NONE;
        } catch (const std::exception&) {
            res.status = 400;  // Bad Request
            return;
        }
        auto new_info = HeartbeatInfo{ req.get_param_value("abilityName"), std::stoi(req.get_param_value("IPCPort")), std::stoi(req.get_param_value("abilityPort")), req.get_param_value("status"),  std::chrono::steady_clock::now()};
        this->lifecycle_manager_->AddHeartbeatInfo(new_info);
    });

    this->server->Get("/api/Devices", [this](const httplib::Request &req, httplib::Response &res)
    { 
        LOG(INFO) << L_BLUE << "httpserver receive devices" << NONE;
        res.set_content(this->resource_manager_->GetHardwareDeviceInfo(true), "application/json"); 
    });

    this->server->Get("/api/AbilityRunning", [this](const httplib::Request &req, httplib::Response &res)
    { 
        LOG(INFO) << L_BLUE << "httpserver receive abilityrunning" << NONE;
        res.set_content(this->lifecycle_manager_->GetHeartbeatMap(), "application/json"); 
    });

    this->server->Get("/api/AbilitySupport", [this](const httplib::Request &req, httplib::Response &res)
    {
        LOG(INFO) << L_BLUE << "httpserver receive abilitysupport" << NONE;
        res.set_content(this->relation_manager_->GetAbilitySupport(), "application/json"); 
    });

    this->server->Post("/api/AbilityRequest", [this](const httplib::Request &req, httplib::Response &res)
    {
        LOG(INFO) << L_BLUE << "httpserver receive abilityrequest" << NONE;
        auto cmd_g = CommandInfo{std::stoi(req.get_param_value("IPCPort")),
                                req.get_param_value("abilityName"),
                                req.get_param_value("cmd"),
                                std::stoi(req.get_param_value("connectPort")),
                                req.get_param_value("connectIP")};
        this->lifecycle_manager_->HandleCommandInfo(cmd_g);
    });

    this->server->Post("/cloudaddress",[this](const httplib::Request &req, httplib::Response &res)
    {
        std::string address = req.get_param_value("address");
        std::string type = req.get_param_value("protocol");
        ProtocolType des_type;
        if (type == "TCP")
        {
            des_type = ProtocolType::TCP;
        }
        else if (type == "UDP")
        {
            des_type = ProtocolType::UDP;
        }
        auto connect_info = ConnectInfo{address,des_type,ProtocolTendency::Random,ConnectionStatus::Uninitialized};
        this->connection_manager_->OnCloudAddressRecordChange(connect_info);
    });
}

void HttpServer::Run()
{
    LOG(INFO) << L_GREEN << "http server start" << NONE;
    this->server->listen("0.0.0.0", 8080);
}
