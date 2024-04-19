/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: http_server.cpp
 * Description: http_server.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "httpserver/http_server.h"

#include <glog/logging.h>

#include "utils/color.h"

void HttpServer::Init(std::shared_ptr<ResourceManager> resource_, std::shared_ptr<LifeCycleManager> lifecycle_,
                      std::shared_ptr<AbilityRelationManager> relation_, std::shared_ptr<ControllerManager> controller_,
                      std::shared_ptr<AbilityStatusManager> status_)
{
    DLOG(INFO) << L_GREEN << "init http server" << NONE;
    this->server = std::make_shared<httplib::Server>();
    this->resource_manager_ = resource_;
    this->lifecycle_manager_ = lifecycle_;
    this->relation_manager_ = relation_;
    this->controller_manager_ = controller_;
    this->status_manager_ = status_;
    this->server->Post("/heartbeat", [this](const httplib::Request &req, httplib::Response &res) {
        DLOG(INFO) << L_BLUE << "httpserver receive: method: Post   URL: /heartbeat" << NONE;
        HeartbeatInfo new_info;
        if (req.get_header_value("Content-Type") == "application/json") {
            Json::Reader reader;
            Json::Value root;
            bool success = reader.parse(req.body, root);
            if (!success) {
                DLOG(ERROR) << "json parse fail";
                return;
            }
            new_info.FromJson(root);
        } else {
            int port;
            try {
                port = std::stoi(req.get_param_value("IPCPort"));
                // DLOG(INFO) << BLUE << "Got heartbeat from port " << port << NONE;
            } catch (const std::exception &) {
                res.status = 400;  // Bad Request
                return;
            }
            // new_info = HeartbeatInfo{req.get_param_value("abilityName"), std::stoi(req.get_param_value("IPCPort")),
            //                          std::stoi(req.get_param_value("abilityPort")), req.get_param_value("status"), std::chrono::steady_clock::now(),
            //                          std::stoul(req.get_param_value("abilityId")), std::stoul(req.get_param_value("abilityInstanceId"))};
            // new_info.abilityName = req.get_param_value("abilityName");
            // new_info.IPCPort = std::stoi(req.get_param_value("IPCPort"));
            // new_info.abilityPort = std::stoi(req.get_param_value("abilityPort"));
            // new_info.status = req.get_param_value("status");
            // new_info.last_update = std::chrono::steady_clock::now();
            // new_info.abilityId = std::stoul(req.get_param_value("abilityId"));
            // new_info.abilityInstanceId = std::stoul(req.get_param_value("abilityInstanceId"));
        }
        DLOG(INFO) << BLUE << "Got heartbeat from port " << new_info.IPCPort << NONE;
        this->lifecycle_manager_->AddHeartbeatInfo(new_info);
        res.set_content("OK", "text/plain");
    });

    this->server->Post("/controllerHeartbeat", [this](const httplib::Request &req, httplib::Response &res) {
        DLOG(INFO) << L_BLUE << "httpserver receive: method: Post   URL: /controllerHeartbeat" << NONE;
        ControllerHeartbeatInfo new_info;
        if (req.get_header_value("Content-Type") == "application/json") {
            Json::Reader reader;
            Json::Value root;
            bool success = reader.parse(req.body, root);
            if (!success) {
                DLOG(ERROR) << "json parse fail";
                return;
            }
            new_info.fromJson(root);
        }
        DLOG(INFO) << BLUE << "Got controller heartbeat from port " << new_info.port << NONE;
        this->controller_manager_->updateControllerInfo(new_info);
        res.set_content("OK", "text/plain");
    });

    this->server->Get("/api/Devices", [this](const httplib::Request &req, httplib::Response &res) {
        DLOG(INFO) << L_BLUE << "httpserver receive: method: GET   URL: /api/Devices" << NONE;
        res.set_content(this->resource_manager_->GetHardwareDeviceInfo(true), "application/json");
    });

    this->server->Get("/api/AbilityRunning", [this](const httplib::Request &req, httplib::Response &res) {
        DLOG(INFO) << L_BLUE << "httpserver receive: method: GET   URL: /api/AbilityRunning" << NONE;
        res.set_content(this->lifecycle_manager_->GetHeartbeatMap(), "application/json");
    });

    this->server->Get("/api/AbilitySupport", [this](const httplib::Request &req, httplib::Response &res) {
        DLOG(INFO) << L_BLUE << "httpserver receive: method: GET   URL: /api/AbilitySupport" << NONE;
        res.set_content(this->relation_manager_->GetAbilitySupport(), "application/json");
    });

    this->server->Get("/api/AbilityController", [this](const httplib::Request &req, httplib::Response &res) {
        DLOG(INFO) << L_BLUE << "httpserver receive: method: GET   URL: /api/AbilityController" << NONE;
        res.set_content(this->controller_manager_->getControllerInfo(), "application/json");
    });

    this->server->Get("/api/AbilityStatus", [this](const httplib::Request &req, httplib::Response &res) {
        DLOG(INFO) << L_BLUE << "httpserver receive: method: GET   URL: /api/AbilityStatus" << NONE;
        unsigned long abilityInstanceId = std::stoul(req.get_param_value("abilityInstanceId"));
        DLOG(INFO) << L_BLUE << "get abilitystatus id: " << abilityInstanceId << NONE;
        res.set_content(this->status_manager_->GetAbilityStatusById(abilityInstanceId).toStyledString(), "application/json");
    });

    this->server->Post("/api/AbilityDesireUpdate", [this](const httplib::Request &req, httplib::Response &res) {
        DLOG(INFO) << L_BLUE << "httpserver receive: method: POST   URL: /api/AbilityDesireUpdate" << NONE;
        this->status_manager_->UpdateAbilityDesireById(req.body);
        res.set_content("OK", "text/plain");
    });

    this->server->Post("/api/AbilityRequest", [this](const httplib::Request &req, httplib::Response &res) {
        // DLOG(INFO) << req.get_header_value("Content-Type");
        CommandInfo cmd;
        if (req.get_header_value("Content-Type") == "application/json") {
            Json::Value root;
            Json::Reader reader;
            bool success = reader.parse(req.body, root);
            if (!success) {
                DLOG(ERROR) << "json parse error";
                return;
            }
            cmd.FromJson(root);
        } else {
            try {
                cmd = CommandInfo{req.get_param_value("abilityName"), req.get_param_value("cmd"),
                                  std::stoi(req.get_param_value("connectPort")), req.get_param_value("connectIP"), 
                                  std::stoul(req.get_param_value("abilityId")), std::stoul(req.get_param_value("abilityInstanceId"))};
            } catch (const std::exception &) {
                res.status = 400;  // Bad Request
                return;
            }
        }
        DLOG(INFO) << L_BLUE << "httpserver receive: method: POST   URL: /api/AbilityRequest" << NONE;
        int result = lifecycle_manager_->HandleCommandInfo(cmd);
        if (result == 1) {
            res.set_content("OK", "text/plain");
        } else if (result == 0) {
            res.set_content("status scheduling fail", "text/plain");
        } else {
            res.set_content("Please wait for the last schedule with the same IPCPort to complete", "text/plain");
        }
    });
}

void HttpServer::Run()
{
    DLOG(INFO) << L_GREEN << "http server start" << NONE;
    this->server->listen("0.0.0.0", 8080);
}
