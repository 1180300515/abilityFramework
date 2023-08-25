/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: test_display_hardware.cpp
 * Description: test_display_hardware.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include <vector>

#include "include/get_display_hardware.h"

int main(int argc, char **argv) {
  std::vector<DisplayHardwareForTest> display = getDisplayInfo();
  print(display);
}
