

#ifndef ABILITYFRAMEWORK_BROADCASTMGR_H
#define ABILITYFRAMEWORK_BROADCASTMGR_H

#include <thread>

class broadcastManager {
public:
  void Run();

private:
  void broadcastInfo();

  // std::string getHostname();

  std::thread broadcast_thread;

};

#endif // ABILITYFRAMEWORK_BROADCASTMGR_H
