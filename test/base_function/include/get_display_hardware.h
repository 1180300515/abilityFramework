/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: get_display_hardware.h
 * Description: get_display_hardware.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef TEST_BASE_FUNCTION_INCLUDE_GET_DISPLAY_HARDWARE_H_
#define TEST_BASE_FUNCTION_INCLUDE_GET_DISPLAY_HARDWARE_H_

#include <string>
#include <vector>

class DisplayHardwareForTest {
 public:
  int screen;
  int x;
  int y;
  int width;
  int height;
  int mmWidth;
  int mmHeight;

  std::string ToKeyValue(std::string hostname) const;
  void print() const;
};

std::vector<DisplayHardwareForTest> getDisplayInfo();
void print(const std::vector<DisplayHardwareForTest> &display);

#endif  // TEST_BASE_FUNCTION_INCLUDE_GET_DISPLAY_HARDWARE_H_
