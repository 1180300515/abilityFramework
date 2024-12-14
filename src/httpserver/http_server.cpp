#include "http_server.h"

#include <glog/logging.h>

#include <json/json.h>

#include <utility>

void httpServer::Init(
    std::function<bool(const Json::Value &)> set_heartbeat_,
    std::function<std::optional<std::string>(const Json::Value &)>
        set_abilityCommand_,
    std::function<std::string()> get_abilityList_,
    std::function<std::string(const std::string &)> get_abilityStatus_) {
  DLOG(INFO) << "init http server";

  set_heartbeat = std::move(set_heartbeat_);
  set_abilityCommand = std::move(set_abilityCommand_);
  get_abilityList = std::move(get_abilityList_);
  get_abilityStatus = std::move(get_abilityStatus_);

  this->server.Put("/ability/abilityHeartbeat", [this](
                                                    const httplib::Request &req,
                                                    httplib::Response &res) {
    DLOG(INFO)
        << "httpserver receive: method: Post   URL: /ability/abilityHeartbeat";

    Json::Reader reader;
    Json::Value root;
    bool success = reader.parse(req.body, root);
    if (!success) {
      DLOG(ERROR) << "json parse fail";
      res.status = 400;
      return;
    }
    auto result = set_heartbeat(root);
    if (!result) {
      res.status = 500;
      return;
    }
    res.status = 200;
    res.set_content("OK", "text/plain");
  });

  this->server.Put("/ability/abilityCommand", [this](
                                                  const httplib::Request &req,
                                                  httplib::Response &res) {
    DLOG(INFO)
        << "httpserver receive: method: Post   URL: /ability/abilityCommand";
    Json::Reader reader;
    Json::Value root;
    bool success = reader.parse(req.body, root);
    if (!success) {
      DLOG(ERROR) << "json parse fail";
      return;
    }
    auto result = set_abilityCommand(root);
    if (result.has_value()) {
      res.status = 500;
      DLOG(ERROR) << result.value();
      return;
    }
    res.status = 200;
    res.set_content("OK", "text/plain");
  });

  this->server.Get("/ability/abilityList", [this](const httplib::Request &req,
                                                  httplib::Response &res) {
    DLOG(INFO) << "httpserver receive: method: GET   URL: /ability/abilityList";
    auto result = get_abilityList();
    res.status = 200;
    res.set_content(result, "application/json");
  });

  this->server.Get("/ability/abilityStatus", [this](const httplib::Request &req,
                                                    httplib::Response &res) {
    DLOG(INFO)
        << "httpserver receive: method: GET   URL: /ability/abilityStatus";
    const auto &name = req.get_param_value("name");
    if (name.empty()) {
      // 参数不存在，返回错误信息
      res.status = 400; // Bad Request
      res.set_content("Missing required parameter: name", "text/plain");
      return;
    }
    auto result = get_abilityStatus(name);
    res.status = 200;
    res.set_content(result, "application/json");
  });
}

void httpServer::Run() {
  DLOG(INFO) << "http server start";
  this->server.listen("0.0.0.0", 8080);
}
