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
