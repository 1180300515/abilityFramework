#ifndef _DISCMGR_INTERFACE_H
#define _DISCMGR_INTERFACE_H

#include "localhw_interface.h"
#include <chrono>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

struct HostInfo {
    std::string hostname;
    std::string ip;
    std::chrono::system_clock::time_point last_announce;
    std::chrono::duration<double> uptime;
};



#endif // _DISCMGR_INTERFACE_H