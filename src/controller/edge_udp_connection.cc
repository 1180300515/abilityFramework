#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "glog/logging.h"

#include "controller/edge_udp_connection.h"

EdgeUDPConnection::~EdgeUDPConnection()
{
    if (sockId_ >= 0) 
    {
        close(sockId_);
    }
}

bool EdgeUDPConnection::SendMessage(std::string data)
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
    int index = edgeaddress_.find(":");
    std::string addr = edgeaddress_.substr(0, index);
    std::string port = edgeaddress_.substr(index + 1);

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

    send_num = sendto(sock_fd, (char *)data.c_str(), strlen((char *)data.c_str()), 0, (struct sockaddr *)&addr_serv, len);

    if (send_num < 0)
    {
        LOG(ERROR) << "sendto error:";
        return false;
    }

    // recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_serv, (socklen_t *)&len);

    // if (recv_num < 0)
    // {
    //     perror("recvfrom error:");
    //     exit(1);
    // }

    // recv_buf[recv_num] = '\0';
    // printf("client receive %d bytes: %s\n", recv_num, recv_buf);
    close(sock_fd);
    return true;
}
