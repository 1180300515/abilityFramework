#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>

#include "glog/logging.h"

#include "controller/cloud_tcp_connection.h"
#include "controller/global_var.h"

void CloudTCPConnection::Connect()
{
    int index = cloud_address.find(":");
    std::string addr = cloud_address.substr(0, index);
    std::string port = cloud_address.substr(index + 1);
    this->tcpaddr = addr.c_str();
    tcpport = atoi(port.c_str());

    int sockfd;
    struct sockaddr_in serv_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        LOG(ERROR) << "socket create fail";
        exit(1);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(tcpport);
    serv_addr.sin_addr.s_addr = inet_addr(tcpaddr);
    bzero(&(serv_addr.sin_zero), 8);

    if ((connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr))) < 0)
    {
        LOG(ERROR) << "connect failed!";
        exit(1);
    }
    LOG(INFO) << "connect with : " << cloud_address << " success";
    this->sockid = sockfd;
    this->cloudAddress_ = cloud_address;
}

void CloudTCPConnection::CloseConnection()
{
    if (this->sockid != -1)
    {
        close(this->sockid);
    }
    this->sockid = -1;
}

bool CloudTCPConnection::SendMessage(std::string data)
{
    if (send(this->sockid, (char *)data.c_str(), strlen((char *)data.c_str()), 0) < 0)
    {
        LOG(ERROR) << "send to tcp server fail";
        return false;
    }
    return true;
}

void CloudTCPConnection::ReceiveMessage()
{
    char msg[10000];
    int length;
    while (!this->restart_flag)
    {
        memset(&msg, 0, sizeof(msg));
        length = recv(this->sockid, (char *)&msg, sizeof(msg), 0);
        std::string recvmsg(msg);
        LOG(INFO) << "receive message " << recvmsg;
        KeyAndDataPackages pack;
        UnMarshalMessageStruct(recvmsg, pack);
        if (pack.packageType == SyncStepTwo)
        {
            callback_(pack);
        }
        else if (pack.packageType == SyncAllOK)
        {
            LOG(INFO) << "Resources are already in sync";
        }
        else
        {
            LOG(ERROR) << pack.packageType << " cannot handle";
        }
    }
}