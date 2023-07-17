#include "connect_by_udp.h"

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "glog/logging.h"
#include "global_var.h"

ConnectByUDP::~ConnectByUDP()
{
    close(this->sockid);
}

bool ConnectByUDP::Connect(std::string address)
{
    if (address != this->address)
    {
        LOG(INFO) << "this UDP connection change destination from: " << this->address << " to: " << address;
        this->address = address;
    }
    int index = this->address.find(":");
    this->udpport = this->address.substr(index + 1);
    return true;
}

bool ConnectByUDP::Disconnect()
{
    close(this->sockid);
    return true;
}

bool ConnectByUDP::SendMessage(const std::string &data)
{
    /* socket文件描述符 */
    int sock_fd;

    /* 建立udp socket */
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0)
    {
        LOG(ERROR) << "socket error";
        return false;
    }
    int index = this->address.find(":");
    std::string addr = this->address.substr(0, index);
    std::string port = this->address.substr(index + 1);

    /* 设置address */
    struct sockaddr_in addr_serv;
    int len;
    memset(&addr_serv, 0, sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = inet_addr(addr.c_str());
    addr_serv.sin_port = htons(atoi(port.c_str()));
    len = sizeof(addr_serv);

    int send_num;

    send_num = sendto(sock_fd, (char *)data.c_str(), strlen((char *)data.c_str()), 0, (struct sockaddr *)&addr_serv, len);

    if (send_num < 0)
    {
        LOG(ERROR) << "udp send error:";
        return false;
    }
    this->sockid = sock_fd;
    return true;
}

void ConnectByUDP::StartServerToReceiveMessage(std::function<void(std::string)> callback)
{
    std::string port = this->udpport;
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
        callback(recvmsg);
    }
}
