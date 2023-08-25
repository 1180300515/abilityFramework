/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: hardware_display.h
 * Description: hardware_display.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

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
