/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: database_store_struct.h
 * Description: database_store_struct.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef DATABASEMGR_DATABASE_STORE_STRUCT_H_
#define DATABASEMGR_DATABASE_STORE_STRUCT_H_

#include <string>

// describe the struct in the sqlite store data

// the struct represent the database store format
struct CrdDBStruct {
    // the name in metadata
    std::string key;
    // the whole file
    std::string value;
    // the group in the spec
    std::string group;
    // the kind in the names field of the spec
    std::string kind;
    // the schama part
    std::string schema;
};

struct InstanceDBStruct {
    // namespace/name
    std::string key;
    // the whole file
    std::string value;
    // the group in the apiVersion
    std::string group;
    // the kind in the file
    std::string kind;
    // the version in the apiVersion
    std::string version;
};

#endif  // DATABASEMGR_DATABASE_STORE_STRUCT_H_
