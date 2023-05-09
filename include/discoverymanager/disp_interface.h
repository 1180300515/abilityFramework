#ifndef _DISP_INTERFACE_H
#define _DISP_INTERFACE_H

#include <iostream>
#include <vector>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

struct DisplayInfo
{
    int screen;
    int x;
    int y;
    int width;
    int height;
    int mmWidth;
    int mmHeight;
};

std::vector<DisplayInfo> getDisplayInfo();

#endif // _DISP_INTERFACE_H