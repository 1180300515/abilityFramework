/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: message_package_struct.h
 * Description: message_package_struct.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef RESOURCEMGR_MESSAGE_PACKAGE_STRUCT_H_
#define RESOURCEMGR_MESSAGE_PACKAGE_STRUCT_H_

#include <string>
#include <vector>

struct KeyDatapack {
  std::string key;        // represent the key in json , format: namespace/name
  std::string version;    // represent the version in json
  std::string eventType;  // represent the eventtype in json
  std::string resourceType;  // resource type
  std::string data;          // the data
};

// as the message between server and client
struct KeyAndDataPackages {
  std::vector<KeyDatapack> data;
  std::string hostname;
  std::string packageType;  // the type of this message: kv , s2c , c2s , notify
};

bool UnMarshalMessageStruct(const std::string &data, KeyAndDataPackages *kdpak);
std::string MarshalMessageStruct(const KeyAndDataPackages &kdpak);

#endif  // RESOURCEMGR_MESSAGE_PACKAGE_STRUCT_H_
