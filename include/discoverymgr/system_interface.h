/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: system_interface.h
 * Description: system_interface.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef DISCOVERYMGR_SYSTEM_INTERFACE_H_
#define DISCOVERYMGR_SYSTEM_INTERFACE_H_

#include <string>

enum class SystemLoad { Increasing, Decreasing, Stable, Unknown };

enum class SystemStatus { Overloaded, Busy, Optimal, Idle, Unknown };

std::string systemStatusToString(SystemStatus status);

std::string systemLoadToString(SystemLoad load);

SystemLoad getSystemLoad(double load1, double load5, double load15);

SystemStatus getSystemStatus(double load1, double load15, int cpuCores);

std::string get_system_status();

#endif  // DISCOVERYMGR_SYSTEM_INTERFACE_H_
