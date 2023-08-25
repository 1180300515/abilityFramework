/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: test_ability_lifecycle.cpp
 * Description: test_ability_lifecycle.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include <glog/logging.h>
#include <grpcpp/grpcpp.h>
#include <httplib.h>

#include <cstdlib>
#include <ctime>
#include <map>
#include <thread>  // NOLINT [build/c++11]

#include "abilityProto.grpc.pb.h"
#include "abilityProto.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using abilityUnit::Ability;
using abilityUnit::ConnectInfo;
using abilityUnit::DisconnectInfo;
using abilityUnit::Response;
using abilityUnit::StartInfo;
using abilityUnit::TerminateInfo;

httplib::Client cli("localhost", 8080);

// using grpcComputer::CameraService;
// using grpcComputer::ImageFile;
// using grpcComputer::Request;

#include <queue>

// #ifdef __cplusplus
// extern "C" {
// #endif

// #include "./camera/camera.h"
// extern int V_HEIGHT;
// extern int V_WIDTH;
// #ifdef __cplusplus
// }
// #endif

extern int V_HEIGHT;
extern int V_WIDTH;

using namespace std;  // NOLINT [build/namespace]

typedef void (*CallbackFunc_t)(void);
map<std::string, CallbackFunc_t> callbackMap;

#define EVENT_START "start"
#define EVENT_CONNECT "connect"
#define EVENT_DISCONNECT "disconnect"
#define EVENT_TERMINATE "terminate"

#define STATUS_INIT "INIT"
#define STATUS_STANDBY "STANDBY"
#define STATUS_RUNNING "RUNNING"
#define STATUS_SUSPEND "SUSPEND"
#define STATUS_TERMINATE "TERMINATE"

#define RESPONSE_OK 0x01
#define RESPONSE_ERROR 0x02

string GLOBAL_STATUS;  // NOLINT [runtime/string]
string GLOBAL_EVENT;   // NOLINT [runtime/string]
int selected_port = 0;
int abilityPort = 0;

void CallbackRegister(std::string event_id, CallbackFunc_t callback);
void CallbackExecute(std::string event_id);
void CallbackUnregister(std::string event_id);
void CallbackRegisterAll();
std::string global2string(std::string GLOBAL_STATUS);

std::string address;  // NOLINT [runtime/string]

std::unique_ptr<Server> ability_server;
std::unique_ptr<grpc::ServerCompletionQueue> ability_cq;

class AbilityImpl : public Ability::Service {
  Status Start(ServerContext *context, const StartInfo *request,
               Response *response) override {
    cout << "Start" << endl;
    if (GLOBAL_STATUS == STATUS_INIT || GLOBAL_STATUS == STATUS_SUSPEND) {
      cout << "request time: " << request->timestamp() << endl;
      cout << "time0: " << time(0) << endl;
      if (time(0) - request->timestamp() > 10 ||
          request->timestamp() > time(0)) {
        cout << "request timestamp error" << endl;
        response->set_code(RESPONSE_ERROR);
        response->set_msg("request timestamp error");
      } else {
        response->set_code(RESPONSE_OK);
        response->set_msg("start success");
        CallbackExecute(EVENT_START);
      }
    } else {
      response->set_code(RESPONSE_ERROR);
      response->set_msg("wrong lifecycle");
    }

    return Status::OK;
  }

  Status Connect(ServerContext *context, const ConnectInfo *request,
                 Response *response) override {
    cout << "Connect" << endl;
    if (GLOBAL_STATUS == STATUS_STANDBY || GLOBAL_STATUS == STATUS_SUSPEND) {
      if (time(0) - request->timestamp() > 10 ||
          request->timestamp() > time(0)) {
        cout << "request timestamp error: "
             << "Now time is : " << time(0)
             << ". Request time is :" << request->timestamp() << endl;
        response->set_code(RESPONSE_ERROR);
        response->set_msg("request timestamp error");
      } else {
        if (request->port() != 0) {
          cout << "port: " << request->port() << endl;
          address = "localhost:" + to_string(request->port());
          response->set_code(RESPONSE_OK);
          response->set_msg("Serve success.");
          CallbackExecute(EVENT_CONNECT);
        } else {
          response->set_code(RESPONSE_ERROR);
          response->set_msg("port error");
        }
      }
    } else {
      response->set_code(RESPONSE_ERROR);
      response->set_msg("wrong lifecycle");
    }
    return Status::OK;
  }

  Status Disconnect(ServerContext *context, const DisconnectInfo *request,
                    Response *response) override {
    cout << "Disconnect" << endl;
    if (GLOBAL_STATUS == STATUS_RUNNING) {
      if (time(0) - request->timestamp() > 10 ||
          request->timestamp() > time(0)) {
        cout << "request timestamp error" << endl;
        response->set_code(RESPONSE_ERROR);
        response->set_msg("request timestamp error");
      } else {
        response->set_code(RESPONSE_OK);
        response->set_msg("disconnect success");
        CallbackExecute(EVENT_DISCONNECT);
      }
    } else {
      response->set_code(RESPONSE_ERROR);
      response->set_msg("wrong lifecycle");
    }
    return Status::OK;
  }

  Status Terminate(ServerContext *context, const TerminateInfo *request,
                   Response *response) override {
    cout << "Terminate" << endl;
    if (GLOBAL_STATUS == STATUS_RUNNING || GLOBAL_STATUS == STATUS_STANDBY ||
        GLOBAL_STATUS == STATUS_SUSPEND) {
      if (time(0) - request->timestamp() > 10 ||
          request->timestamp() > time(0)) {
        cout << "request timestamp error" << endl;
        response->set_code(RESPONSE_ERROR);
        response->set_msg("request timestamp error");
      } else {
        response->set_code(RESPONSE_OK);
        response->set_msg("terminate success");
        CallbackExecute(EVENT_TERMINATE);
      }
    } else {
      response->set_code(RESPONSE_ERROR);
      response->set_msg("wrong lifecycle");
    }
    return Status::OK;
  }
};

void sendStateMsg() {
  httplib::Params params;
  params.emplace("abilityName", "loudspeaker");
  params.emplace("IPCPort", to_string(selected_port));
  // params.emplace("port", "0");
  // params.emplace("status", "good");
  std::cout << "my post IPCPort: " << selected_port << std::endl;
  std::cout << "my post status: " << GLOBAL_STATUS << std::endl;
  std::cout << "my post abilityPort: " << abilityPort << std::endl;
  params.emplace("status", global2string(GLOBAL_STATUS));
  // string status = (GLOBAL_STATUS);
  params.emplace("abilityPort", to_string(abilityPort));
  DLOG(INFO) << "主动post心跳";
  auto res = cli.Post("/heartbeat", params);

  if (res) {
    DLOG(INFO) << "成功";
    std::cout << res->status << std::endl;
    std::cout << res->body << std::endl;
  } else {
    DLOG(ERROR) << "post失败";
  }
}

void OnStart() {
  cout << "OnStart" << endl;
  GLOBAL_STATUS = STATUS_STANDBY;
  sendStateMsg();
}

void OnConnect() {
  cout << "OnConnect" << endl;
  // thread t_abilityserver(create_ability_server);
  // t_abilityserver.detach();
  unsigned int local_seed = time(NULL);
  abilityPort = rand_r(&local_seed);
  GLOBAL_STATUS = STATUS_RUNNING;
  sendStateMsg();
}

void OnDisconnect() {
  cout << "OnDisconnect" << endl;
  // ability_server->Shutdown();
  // ability_cq->Shutdown();
  GLOBAL_STATUS = STATUS_SUSPEND;
  sendStateMsg();
}

void OnTerminate() {
  cout << "OnTerminate" << endl;
  GLOBAL_STATUS = STATUS_TERMINATE;
  sendStateMsg();
  exit(0);
}

// 添加回调函数
void CallbackRegister(std::string event_id, CallbackFunc_t callback) {
  if (callbackMap[event_id] == NULL) {
    callbackMap[event_id] = callback;
  } else if (callbackMap[event_id] != callback) {
    callbackMap[event_id] = callback;
  } else {
    cout << "Callback already registered" << endl;
  }
}

// 删除回调函数
void CallbackUnregister(std::string event_id) {
  if (callbackMap[event_id] != NULL) {
    callbackMap.erase(event_id);
  } else {
    cout << "Callback not registered" << endl;
  }
}

// 执行回调函数
void CallbackExecute(std::string event_id) {
  if (callbackMap[event_id] != NULL) {
    callbackMap[event_id]();
  } else {
    cout << "Callback not registered" << endl;
  }
}

// 注册已知回调函数
void CallbackRegisterAll() {
  CallbackRegister(EVENT_START, OnStart);
  CallbackRegister(EVENT_CONNECT, OnConnect);
  CallbackRegister(EVENT_DISCONNECT, OnDisconnect);
  CallbackRegister(EVENT_TERMINATE, OnTerminate);
}

void heartbeat_loop() {
  // httplib::Client
  // cli("http://75d68bb1-2500-4cd5-97c9-fca0d20b169a.mock.pstmn.io");

  httplib::Params params;

  while (true) {
    params.emplace("abilityName", "cameratest");
    params.emplace("IPCPort", to_string(selected_port));

    // std::cout << "status: " << GLOBAL_STATUS << std::endl;
    params.emplace("status", global2string(GLOBAL_STATUS));
    // string status = (GLOBAL_STATUS);
    params.emplace("abilityPort", to_string(abilityPort));
    auto res = cli.Post("/heartbeat", params);

    if (res && res->status == 200) {
      std::cout << res->status << std::endl;
      std::cout << res->body << std::endl;
    } else {
      std::cout << "Failed to send request." << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));
    params.clear();
  }
}

std::string global2string(std::string GLOBAL_STATUS) {
  // std::cout << "WTF THE STATUS IS : " << GLOBAL_STATUS << std::endl;

  if (GLOBAL_STATUS == STATUS_INIT) {
    // std::cout << "RETURN INIT" << std::endl;
    return "INIT";
  } else if (GLOBAL_STATUS == STATUS_STANDBY) {
    return "STANDBY";
  } else if (GLOBAL_STATUS == STATUS_RUNNING) {
    return "RUNNING";
  } else if (GLOBAL_STATUS == STATUS_SUSPEND) {
    return "SUSPEND";
  } else if (GLOBAL_STATUS == STATUS_TERMINATE) {
    return "TERMINATE";
  } else {
    return "ERROR";
  }
}

int main() {
  FLAGS_alsologtostderr = 1;
  FLAGS_colorlogtostderr = 1;
  AbilityImpl service;
  ServerBuilder builder;
  CallbackRegisterAll();
  builder.AddListeningPort("localhost:0", grpc::InsecureServerCredentials(),
                           &selected_port);
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on :" << selected_port << std::endl;
  GLOBAL_STATUS = STATUS_INIT;
  std::cout << "NOW GLOBAL STATUS IS: " << GLOBAL_STATUS << std::endl;
  std::thread heartbeat_thread(heartbeat_loop);
  server->Wait();

  return 0;
}
