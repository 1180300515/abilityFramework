#ifndef _DATABASE_STORE_STRUCT_H
#define _DATABASE_STORE_STRUCT_H

#include <string>

// describe the struct in the sqlite store data

// the struct represent the database store format
struct CrdDBStruct
{
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

struct InstanceDBStruct
{
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

#endif // _DATABASE_STORE_STRUCT_H