#include "connect_by_tcp.h"

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

#include "glog/logging.h"

ConnectByTCP::~ConnectByTCP()
{
    close(sockid);
}

bool ConnectByTCP::Connect(std::string address)
{
    if (address != this->address)
    {
        LOG(INFO) << "this TCP connection change desaddress from: " << this->address << " to: " << address;
        this->address = address;
        close(this->sockid);
    }
    int index = address.find(":");
    std::string addr = address.substr(0, index);
    std::string port = address.substr(index + 1);
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
    LOG(INFO) << "connect with : " << address << " success";
    this->sockid = sockfd;
    return true;
}

bool ConnectByTCP::Disconnect()
{
    close(this->sockid);
    return true;
}

bool ConnectByTCP::SendMessage(const std::string &data)
{
    if (send(this->sockid, (char *)data.c_str(), strlen((char *)data.c_str()), 0) < 0)
    {
        LOG(WARNING) << "send to address: " << this->address << " fail!";
        return false;
    }
    return true;
}

void ConnectByTCP::StartServerToReceiveMessage(std::function<void(std::string)> callback)
{
    fd_set readfds;
    char msg[10000];
    int length;
    while (alive)
    {
        FD_ZERO(&readfds);
        FD_SET(this->sockid, &readfds);
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        select(this->sockid + 1, &readfds, NULL, NULL, &tv);
        if (FD_ISSET(this->sockid, &readfds))
        {
            memset(&msg, 0, sizeof(msg));
            length = recv(this->sockid, (char *)&msg, sizeof(msg), 0);
            std::string recvmsg(msg);
            LOG(INFO) << "receive message " << recvmsg;
            callback(recvmsg);
        }
    }
}

void ConnectByTCP::StopServerToReceiveMessage()
{
    alive = false;
}
