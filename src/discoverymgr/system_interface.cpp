/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: system_interface.cpp
 * Description: system_interface.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "discoverymgr/system_interface.h"

#include <sys/sysinfo.h>

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <thread> // NOLINT [build/c++11]

std::string systemStatusToString(SystemStatus status) {
  switch (status) {
    case SystemStatus::Overloaded:
      return "Overloaded";
    case SystemStatus::Busy:
      return "Busy";
    case SystemStatus::Optimal:
      return "Optimal";
    case SystemStatus::Idle:
      return "Idle";
    default:
      return "Unknown";
  }
}

std::string systemLoadToString(SystemLoad load) {
  switch (load) {
    case SystemLoad::Increasing:
      return "Increasing";
    case SystemLoad::Decreasing:
      return "Decreasing";
    case SystemLoad::Stable:
      return "Stable";
    default:
      return "Unknown";
  }
}

SystemLoad getSystemLoad(double load1, double load5, double load15) {
  if (load1 > load5 && load5 > load15) {
    return SystemLoad::Increasing;
  } else if (load1 < load5 && load5 < load15) {
    return SystemLoad::Decreasing;
  } else {
    return SystemLoad::Stable;
  }
}

SystemStatus getSystemStatus(double load1, double load15, int cpuCores) {
  if (load15 > cpuCores) {
    return SystemStatus::Overloaded;
  } else if (load1 < cpuCores / 2.0) {
    return SystemStatus::Idle;
  } else if (load1 >= cpuCores / 2.0 && load1 <= cpuCores * 1.5) {
    return SystemStatus::Optimal;
  } else {
    return SystemStatus::Busy;
  }
}

std::string get_system_status() {
  std::ifstream file("/proc/loadavg");
  if (!file) {
    std::cerr << "Cannot open /proc/loadavg\n";
  }

  double load1, load5, load15;
  if (!(file >> load1 >> load5 >> load15)) {
    std::cerr << "Cannot read /proc/loadavg\n";
  }

  int cpuCores = std::thread::hardware_concurrency();

  SystemStatus status = getSystemStatus(load1, load15, cpuCores);
  SystemLoad load = getSystemLoad(load1, load5, load15);

  return systemStatusToString(status) + "," + systemLoadToString(load);
}
