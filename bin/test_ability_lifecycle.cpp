#include <grpcpp/grpcpp.h>
#include <ctime>
#include <map>
#include <thread>

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

using namespace std;

typedef void (*CallbackFunc_t)(void);
map<int, CallbackFunc_t> callbackMap;

#define EVENT_START 0x81
#define EVENT_CONNECT 0x82
#define EVENT_DISCONNECT 0x83
#define EVENT_TERMINATE 0x84

#define STATUS_INIT 0x01
#define STATUS_STANDBY 0x02
#define STATUS_RUNNING 0x03
#define STATUS_SUSPEND 0x04
#define STATUS_TERMINATE 0x05

#define RESPONSE_OK 0x01
#define RESPONSE_ERROR 0x02

int GLOBAL_STATUS = 0;
string GLOBAL_EVENT = "";

void CallbackRegister(int event_id, CallbackFunc_t callback);
void CallbackExecute(int event_id);
void CallbackUnregister(int event_id);
void CallbackRegisterAll();

std::string address;

std::unique_ptr<Server> ability_server;
std::unique_ptr<grpc::ServerCompletionQueue> ability_cq;


class AbilityImpl : public Ability::Service {
    Status Start(ServerContext* context,
                 const StartInfo* request,
                 Response* response) override {
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

    Status Connect(ServerContext* context,
                   const ConnectInfo* request,
                   Response* response) override {
        cout << "Connect" << endl;
        if (GLOBAL_STATUS == STATUS_STANDBY ||
            GLOBAL_STATUS == STATUS_SUSPEND) {
            if (time(0) - request->timestamp() > 10 ||
                request->timestamp() > time(0)) {
                cout << "request timestamp error" << endl;
                response->set_code(RESPONSE_ERROR);
                response->set_msg("request timestamp error");
            } else {
                if (request->port() != 0) {
                    cout << "port: " << request->port() << endl;
                    address = "0.0.0.0:" + to_string(request->port());
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

    Status Disconnect(ServerContext* context,
                      const DisconnectInfo* request,
                      Response* response) override {
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

    Status Terminate(ServerContext* context,
                     const TerminateInfo* request,
                     Response* response) override {
        cout << "Terminate" << endl;
        if (GLOBAL_STATUS == STATUS_RUNNING ||
            GLOBAL_STATUS == STATUS_STANDBY ||
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

void OnStart() {
    cout << "OnStart" << endl;
    GLOBAL_STATUS = STATUS_STANDBY;
}

// void create_ability_server() {
//     camera_server service2;
//     ServerBuilder builder2;
//     builder2.AddListeningPort(address, grpc::InsecureServerCredentials());
//     cout << "add listening port" << endl;
//     builder2.RegisterService(&service2);
//     cout << "register service" << endl;
//     ability_cq = builder2.AddCompletionQueue();
//     ability_server = builder2.BuildAndStart();
//     std::cout << "Server listening on port: " << address << std::endl;
//     ability_server->Wait();
// }

void OnConnect() {
    cout << "OnConnect" << endl;
    // thread t_abilityserver(create_ability_server);
    // t_abilityserver.detach();
    GLOBAL_STATUS = STATUS_RUNNING;
}

void OnDisconnect() {
    cout << "OnDisconnect" << endl;
    ability_server->Shutdown();
    ability_cq->Shutdown();
    GLOBAL_STATUS = STATUS_SUSPEND;
}

void OnTerminate() {
    cout << "OnTerminate" << endl;
    GLOBAL_STATUS = STATUS_TERMINATE;
    exit(0);
}

// 添加回调函数
void CallbackRegister(int event_id, CallbackFunc_t callback) {
    if (callbackMap[event_id] == NULL) {
        callbackMap[event_id] = callback;
    } else if (callbackMap[event_id] != callback) {
        callbackMap[event_id] = callback;
    } else {
        cout << "Callback already registered" << endl;
    }
}

// 删除回调函数
void CallbackUnregister(int event_id) {
    if (callbackMap[event_id] != NULL) {
        callbackMap.erase(event_id);
    } else {
        cout << "Callback not registered" << endl;
    }
}

// 执行回调函数
void CallbackExecute(int event_id) {
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

int main() {
    AbilityImpl service;
    ServerBuilder builder;
    CallbackRegisterAll();
    int selected_port = 0;
    builder.AddListeningPort("0.0.0.0:0",
                             grpc::InsecureServerCredentials(),
                             &selected_port);
    std::cout << "selected port: " << selected_port << std::endl;
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on :" << selected_port << std::endl;
    GLOBAL_STATUS = STATUS_INIT;
    server->Wait();
    return 0;
}