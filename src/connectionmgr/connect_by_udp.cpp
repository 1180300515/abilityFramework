#include "connect_by_udp.h"

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "glog/logging.h"

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
    return true;
}

bool ConnectByUDP::Disconnect()
{
    close(this->sockid);
    return true;
}

std::optional<std::string> ConnectByUDP::SendAndReceviceMessage(std::string data)
{
    /* socket文件描述符 */
    int sock_fd;

    /* 建立udp socket */
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0)
    {
        LOG(ERROR) << "socket error";
        return std::nullopt;
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
    int recv_num;
    char recv_buf[10000];

    send_num = sendto(sock_fd, (char *)data.c_str(), strlen((char *)data.c_str()), 0, (struct sockaddr *)&addr_serv, len);

    if (send_num < 0)
    {
        LOG(ERROR) << "udp send error:";
        return std::nullopt;
    }
    this->sockid = sock_fd;

    recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_serv, (socklen_t *)&len);

    if (recv_num < 0)
    {
        perror("recvfrom error:");
        exit(1);
    }

    recv_buf[recv_num] = '\0';
    std::string recvmsg(recv_buf);
    close(sock_fd);
    return recvmsg;
}
