/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: get_display_hardware.cpp
 * Description: get_display_hardware.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "include/get_display_hardware.h"

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <vector>

void DisplayHardwareForTest::print() const {
  std::cout << "screen: " << screen << std::endl;
  std::cout << "x: " << x << std::endl;
  std::cout << "y: " << y << std::endl;
  std::cout << "width: " << width << std::endl;
  std::cout << "height: " << height << std::endl;
  std::cout << "mmWidth: " << mmWidth << std::endl;
  std::cout << "mmHeight: " << mmHeight << std::endl;
  std::cout << std::endl << std::endl;
}

std::string DisplayHardwareForTest::ToKeyValue(std::string hostname) const {
  std::stringstream ss;
  ss << "hostname:" << hostname << ",";
  ss << "screen:" << screen << ",";
  ss << "x:" << x << ",";
  ss << "y:" << y << ",";
  ss << "width:" << width << ",";
  ss << "height:" << height << ",";
  ss << "mmWidth:" << mmWidth << ",";
  ss << "mmHeight:" << mmHeight;
  return ss.str();
}

std::vector<DisplayHardwareForTest> getDisplayInfo() {
  std::vector<DisplayHardwareForTest> hardware;
  const char *displayEnv = getenv("DISPLAY");
  if (!displayEnv) {
    if (setenv("DISPLAY", ":0", 0) == -1) {
      std::cerr << "Failed to set DISPLAY environment variable" << std::endl;
      exit(1);
    }
  }

  Display *dpy = XOpenDisplay(NULL);
  if (!dpy) {
    std::cerr << "Unable to open display" << std::endl;
    exit(1);
  }

  int screen = DefaultScreen(dpy);
  Window root = RootWindow(dpy, screen);

  XRRScreenResources *res = XRRGetScreenResourcesCurrent(dpy, root);

  for (int crtc = 0; crtc < res->ncrtc; ++crtc) {
    XRRCrtcInfo *crtcInfo = XRRGetCrtcInfo(dpy, res, res->crtcs[crtc]);

    if (crtcInfo->mode != None) {
      DisplayHardwareForTest display;
      display.screen = screen;
      display.x = crtcInfo->x;
      display.y = crtcInfo->y;
      display.width = crtcInfo->width;
      display.height = crtcInfo->height;

      if (crtcInfo->noutput > 0) {
        XRROutputInfo *outputInfo =
            XRRGetOutputInfo(dpy, res, crtcInfo->outputs[0]);
        display.mmWidth = outputInfo->mm_width;
        display.mmHeight = outputInfo->mm_height;
        XRRFreeOutputInfo(outputInfo);
      }
      hardware.emplace_back(display);
    }

    XRRFreeCrtcInfo(crtcInfo);
  }

  XRRFreeScreenResources(res);
  XCloseDisplay(dpy);
  return hardware;
}

void print(const std::vector<DisplayHardwareForTest> &display) {
  for (const auto &iter : display) {
    iter.print();
  }
}
