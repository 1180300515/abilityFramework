#include "broadcast_manager.h"

#include <arpa/inet.h>
#include <glog/logging.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void broadcastManager::Run() {
  this->broadcast_thread = std::thread([this](){
    this->broadcastInfo();
  });
}

void broadcastManager::broadcastInfo() {
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    return;
  }

  int broadcast = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
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
  std::string message = std::string(hostname) + "+" + std::to_string(std::time(nullptr));

  while(true) {
    if (sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      perror("sendto");
      close(sock);
      exit(EXIT_FAILURE);
    }
    sleep(5);
  }
}
//
//std::string broadcastManager::getHostname() {
//  char hostname[256];
//  if (gethostname(hostname, sizeof(hostname)) != 0) {
//    // fail
//    exit(EXIT_FAILURE);
//  }
//  hostname[sizeof(hostname) - 1] = '\0';
//  return std::string(hostname);
//}
