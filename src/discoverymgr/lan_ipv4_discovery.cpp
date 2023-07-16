#include "lan_ipv4_discovery.h"

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "glog/logging.h"

#include "color.h"
#include "system_interface.h"

std::string LANIPV4Discovery::getHostName()
{
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0)
    {
        // 获取主机名失败
        return "";
    }
    hostname[sizeof(hostname) - 1] = '\0'; // 确保以 null 结尾
    return std::string(hostname);
}

void LANIPV4Discovery::RunBroadcastReceiver()
{
    this->receiver_thread = std::thread(&LANIPV4Discovery::udp_broadcast_receiver, this);
    LOG(INFO) << "start lan-ipv4-discovery broadcast receiver success";
}

void LANIPV4Discovery::udp_broadcast_receiver()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return;
    }

    struct sockaddr_in addr
    {
    };
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        close(sock);
        return;
    }

    while (true)
    {
        char recv_buf[128];
        struct sockaddr_in remote_addr
        {
        };
        socklen_t addr_len = sizeof(remote_addr);

        ssize_t len = recvfrom(sock, recv_buf, sizeof(recv_buf) - 1, 0, (struct sockaddr *)&remote_addr, &addr_len);
        if (len < 0)
        {
            perror("recvfrom");
            break;
        }

        recv_buf[len] = '\0';
        std::string message(recv_buf);

        std::istringstream iss(message);
        std::string hostname, status, timestamp_str;
        std::getline(iss, hostname, '+');
        std::getline(iss, status, '+');
        std::getline(iss, timestamp_str);
        std::time_t timestamp = std::stoll(timestamp_str);

        std::string ip = inet_ntoa(remote_addr.sin_addr);
        auto newdevice = DiscoveryDeviceInfo{hostname, ip, this->discoverySource, timestamp, 0, status};
        callback(newdevice);
    }

    close(sock);
}

void LANIPV4Discovery::RegisterCallback(std::function<void(DiscoveryDeviceInfo)> callback)
{
    this->callback = callback;
}

void LANIPV4Discovery::BroadcastSender()
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return;
    }

    int broadcast = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
    {
        perror("setsockopt");
        close(sock);
        return;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = INADDR_BROADCAST;

    char hostname[40];
    gethostname(hostname, sizeof(hostname));
    std::string loadavg = get_system_status();
    std::string message = std::string(hostname) + "+" + loadavg + "+" + std::to_string(std::time(nullptr));

    LOG(INFO) << L_PURPLE << "Broadcasting: " << message << NONE;

    if (sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("sendto");
    }

    close(sock);
}
