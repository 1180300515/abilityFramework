/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: common_struct_defination.h
 * Description: common_struct_defination.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef RESOURCEMGR_COMMON_STRUCT_DEFINATION_H_
#define RESOURCEMGR_COMMON_STRUCT_DEFINATION_H_

#include <string>

struct DeviceDescribe {
  std::string devicename;
  std::string deviceid;
  std::string deviceip;
  std::string deviceport;
  std::string status;
};

struct Metadata {
  std::string name;
  std::string namespace_name;
};

#endif  // RESOURCEMGR_COMMON_STRUCT_DEFINATION_H_
