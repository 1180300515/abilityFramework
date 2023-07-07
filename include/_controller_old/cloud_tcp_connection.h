#ifndef CLOUD_TCP_CONNECTION_H
#define CLOUD_TCP_CONNECTION_H

#include "cloud_connection.h"

class CloudTCPConnection : public CloudConnection
{
private:
    const char *tcpaddr;
    uint16_t tcpport;
    int sockid = -1;

public:
    bool restart_flag = false;
    CloudTCPConnection(std::string address) : CloudConnection(address){};
    ~CloudTCPConnection() = default;
    /**
     * manage connection
     */
    void Connect();
    /**
     * close current connection
     */
    void CloseConnection();
    /**
     * send message
     */
    bool SendMessage(std::string data);
    /**
     * receive message
     */
    void ReceiveMessage();
};

#endif