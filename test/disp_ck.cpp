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

std::vector<DisplayInfo> getDisplayInfo()
{
    std::vector<DisplayInfo> displays;

    const char *displayEnv = getenv("DISPLAY");
    if (!displayEnv)
    {
        if (setenv("DISPLAY", ":0", 0) == -1)
        {
            std::cerr << "Failed to set DISPLAY environment variable" << std::endl;
            return displays;
        }
    }

    Display *dpy = XOpenDisplay(NULL);
    if (!dpy)
    {
        std::cerr << "Unable to open display" << std::endl;
        return displays;
    }

    int screen = DefaultScreen(dpy);
    Window root = RootWindow(dpy, screen);

    XRRScreenResources *res = XRRGetScreenResourcesCurrent(dpy, root);

    for (int crtc = 0; crtc < res->ncrtc; ++crtc)
    {
        XRRCrtcInfo *crtcInfo = XRRGetCrtcInfo(dpy, res, res->crtcs[crtc]);

        if (crtcInfo->mode != None)
        {
            DisplayInfo display;
            display.screen = screen;
            display.x = crtcInfo->x;
            display.y = crtcInfo->y;
            display.width = crtcInfo->width;
            display.height = crtcInfo->height;

            if (crtcInfo->noutput > 0)
            {
                XRROutputInfo *outputInfo = XRRGetOutputInfo(dpy, res, crtcInfo->outputs[0]);
                display.mmWidth = outputInfo->mm_width;
                display.mmHeight = outputInfo->mm_height;
                XRRFreeOutputInfo(outputInfo);
            }

            displays.push_back(display);
        }

        XRRFreeCrtcInfo(crtcInfo);
    }

    XRRFreeScreenResources(res);
    XCloseDisplay(dpy);
    return displays;
}

int main()
{
    std::vector<DisplayInfo> displays = getDisplayInfo();

    for (const auto &display : displays)
    {
        std::cout << "Screen: " << display.screen << std::endl;
        std::cout << "X: " << display.x << std::endl;
        std::cout << "Y: " << display.y << std::endl;
        std::cout << "Width: " << display.width << std::endl;
        std::cout << "Height: " << display.height << std::endl;
        std::cout << "Physical Width (mm): " << display.mmWidth << std::endl;
        std::cout << "Physical Height (mm): " << display.mmHeight << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
