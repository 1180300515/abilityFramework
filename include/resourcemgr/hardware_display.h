#ifndef RESOURCEMGR_HARDWARE_DISPLAY_H_
#define RESOURCEMGR_HARDWARE_DISPLAY_H_

#include <json/json.h>

#include <string>

class DisplayHardware {
 public:
  int screen;
  int x;
  int y;
  int width;
  int height;
  int mmWidth;
  int mmHeight;

  Json::Value toJson() const;
  void fromJson(const Json::Value &root);
  std::string ToKeyValue(std::string hostname) const;
  void print() const;
};

#endif  // RESOURCEMGR_HARDWARE_DISPLAY_H_
