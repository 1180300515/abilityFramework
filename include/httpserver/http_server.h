#ifndef HTTPSERVER_HTTP_SERVER_H_
#define HTTPSERVER_HTTP_SERVER_H_

#include <httplib.h>

#include <memory>

#include <json/json.h>
#include <optional>

class httpServer {
public:
  void Init(std::function<bool(const Json::Value &)> set_heartbeat_,
            std::function<std::optional<std::string>(const Json::Value &)>
                set_abilityCommand_,
            std::function<std::string ()> get_abilityList_,
            std::function<std::string (const std::string&)> get_abilityStatus_);
  void Run();

private:
  std::function<bool(const Json::Value &)> set_heartbeat;
  std::function<std::optional<std::string>(const Json::Value &)>
      set_abilityCommand;
  std::function<std::string ()> get_abilityList;
  std::function<std::string (const std::string&)> get_abilityStatus;

  httplib::Server server;
};

#endif // HTTPSERVER_HTTP_SERVER_H_
