#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "controller/mongoose/mongoose.h"
#include "glog/logging.h"

#include "controller/global_var.h"
#include "controller/dbmanager.h"
#include "controller/controller.h"
#include "controller/cloud_tcp_connection.h"
#include "controller/connection_manager.h"
#include "controller/edge_udp_connection.h"

// ev_data is the client request message
static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data)
{
    if (ev == MG_EV_HTTP_MSG)
    {
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        // match the /cloud/address
        if (mg_http_match_uri(hm, "/cloud/address"))
        {
            cloud_address = hm->body.ptr;
            dbManager::getInstance().DBStoreCloudAddress();
            mg_http_reply(c, 200, "Content-Type: text/plain\r\n", "receive");
        }
        else
        {
            mg_http_reply(c, 500, NULL, "\n");
        }
    }
}

void http_server_runner()
{
    struct mg_mgr mgr;
    int count = 0;
    mg_mgr_init(&mgr);
    mg_http_listen(&mgr, http_server_address.c_str(), fn, &mgr);
    while (true)
    {
        mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);
}

ConnectionManager::ConnectionManager(std::shared_ptr<Controller> controller)
{
    controller_ = controller;
}

void ConnectionManager::RegisterCloudCallback(std::function<void(KeyAndDataPackages)> func)
{
    cloudCallback_ = func;
}

void ConnectionManager::RegisterEdgeCallback(std::function<void(KeyAndDataPackages)> func)
{
    edgeCallback_ = func;
}

bool ConnectionManager::SendMessageToCloud(std::string data)
{
    return cloud_->SendMessage(data);
}

void ConnectionManager::SetEdgeAddressRecord(std::map<std::string, std::string> record)
{
    std::lock_guard<std::mutex> locker(lock_);
    edgeAddressRecord_ = record;
}

void ConnectionManager::HttpServerThread()
{
    http_server_runner();
}

void ConnectionManager::CloudSyncThread()
{
    while (cloud_address == "")
    {
        sleep(5);
    }
    // new a tcp connection with cloud
    cloud_ = std::make_shared<CloudTCPConnection>(cloud_address);
    cloud_->RegisterCallback(cloudCallback_);

    LOG(INFO) << "cloud address already set : " << cloud_address;

    for (;;)
    {
        // check the cloud address whether change
        while (!cloud_->CheckConnection())
        {
            sleep(1);
            LOG(INFO) << "cloud address change , wait for the reconnection";
        }
        // refresh the key and version record
        controller_->RefreshKVRecord();
        // send message
        KeyAndDataPackages pack;
        pack.packageType = SyncStepOne;
        pack.hostname = device_hostname;
        for (auto &iter : controller_->versionRecord)
        {
            KeyDatapack kd;
            kd.key = iter.first;
            kd.version = iter.second;
            pack.data.emplace_back(kd);
        }
        std::string data_string = MarshalMessageStruct(pack);
        LOG(INFO) << "send message : " << data_string;
        if (!cloud_->SendMessage(data_string))
        {
            sleep(5);
            continue;
        }
        sleep(5);
    }
}

void ConnectionManager::EdgeSyncServerThread()
{
    std::string port = std::to_string(EDGE_SYNC_PORT);
    int sockfd;
    struct sockaddr_in serv_addr;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        LOG(ERROR) << "socket create fail";
        exit(1);
    }
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(port.c_str()));
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    int len = sizeof(serv_addr);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0)
    {
        LOG(ERROR) << "bind failed!";
        exit(1);
    }

    char msg[10000];
    int length;
    struct sockaddr_in addr_client;
    while (1)
    {
        memset(&msg, 0, sizeof(msg));
        length = recvfrom(sockfd, (char *)&msg, sizeof(msg), 0, (struct sockaddr *)&addr_client, (socklen_t *)&len);
        if (length < 0)
        {
            LOG(ERROR) << "recvfrom error";
            exit(1);
        }
        std::string recvmsg(msg);
        LOG(INFO) << "receive message " << recvmsg;
        KeyAndDataPackages pack;
        UnMarshalMessageStruct(recvmsg, pack);
        if (pack.packageType == EdgeSync)
        {
            edgeCallback_(pack);
        }
        else
        {
            LOG(ERROR) << pack.packageType << " cannot handle";
        }
    }
}
void ConnectionManager::EdgeSyncThread()
{
    for (;;)
    {
        std::string data = controller_->GetNonLocalFormat();
        {
            std::lock_guard<std::mutex> locker(lock_);
            for (auto &iter : edgeAddressRecord_)
            {
                auto connect = std::make_shared<EdgeUDPConnection>(iter.second);
                connect->SendMessage(data);
            }
        }
        sleep(5);
    }
}

void ConnectionManager::Run()
{
    // // start http server
    // std::thread th1(&ConnectionManager::HttpServerThread, this);
    // LOG(INFO) << "start http server success";
    // start edge server
    std::thread th2(&ConnectionManager::EdgeSyncServerThread, this);
    LOG(INFO) << "start edge sync server success";
    sleep(1);
    // // start cloud sync
    // std::thread th3(&ConnectionManager::CloudSyncThread, this);
    // start edge sync
    std::thread th4(&ConnectionManager::EdgeSyncThread, this);

    // th1.join();
    th2.join();
    // th3.join();
    th4.join();
}
