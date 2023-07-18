#include "hardware_scan.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <glob.h>
#include <linux/videodev2.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include "glog/logging.h"

#include "cameradevice_instance.h"
#include "louspeakerdevice_instance.h"
#include "microphonedevice_instance.h"
#include "global_var.h"

void HardwareScan::insertCameraInfo(Json::Value &jnode)
{
}

void HardwareScan::insertMicInfo(Json::Value &jnode)
{
}

void HardwareScan::insertLoudspeakerInfo(Json::Value &jnode)
{
}

void HardwareScan::getCameraInfo()
{
    glob_t glob_result;

    // 找到所有的V4L设备
    glob("/dev/video*", GLOB_TILDE, nullptr, &glob_result);
    for (unsigned int i = 0; i < glob_result.gl_pathc; ++i)
    {
        const char *path = glob_result.gl_pathv[i];
        int fd = open(path, O_RDONLY);

        if (fd == -1)
        {
            std::cerr << "Failed to open " << path << std::endl;
            continue;
        }

        // 获取设备的能力
        v4l2_capability cap;
        if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)
        {
            std::cerr << "Failed to get device capabilities for " << path << std::endl;
            close(fd);
            continue;
        }

        // 枚举设备支持的像素格式
        v4l2_fmtdesc fmt = {0};
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        char fourcc[5] = {0};
        std::vector<std::string> formats;
        while (ioctl(fd, VIDIOC_ENUM_FMT, &fmt) == 0)
        {
            strncpy(fourcc, reinterpret_cast<const char *>(&fmt.pixelformat), 4);
            formats.push_back(fourcc);
            fmt.index++;
        }

        // 创建一个CameraDevice对象，并添加到列表中
        CameraHardware device;
        device.device_path = path;
        device.driver = reinterpret_cast<const char *>(cap.driver);
        device.card = reinterpret_cast<const char *>(cap.card);
        device.bus_info = reinterpret_cast<const char *>(cap.bus_info);
        device.formats = formats;
        // camera_devices.push_back(device);

        bool device_exists = false;
        for (auto &existing_device : this->profile.cameraDevices)
        {
            if (existing_device.bus_info == device.bus_info)
            {
                device_exists = true;
                if (existing_device.device_path > device.device_path)
                {
                    existing_device = device; // Note: this requires CameraDevice to be assignable
                }
                break;
            }
        }

        if (!device_exists)
        {
            this->profile.cameraDevices.push_back(device);
        }

        close(fd);
    }

    globfree(&glob_result);

    // // 打印摄像头设备列表
    // for (const auto &device : camera_devices)
    // {
    //     std::cout << "Device path: " << device.device_path << std::endl
    //               << "Driver: " << device.driver << std::endl
    //               << "Card: " << device.card << std::endl
    //               << "Bus info: " << device.bus_info << std::endl
    //               << "Supported formats: ";
    //     for (const auto &format : device.formats)
    //     {
    //         std::cout << format << " ";
    //     }
    //     std::cout << std::endl
    //               << std::endl;
    // }
}

void HardwareScan::getDisplayInfo()
{

    const char *displayEnv = getenv("DISPLAY");
    if (!displayEnv)
    {
        if (setenv("DISPLAY", ":0", 0) == -1)
        {
            std::cerr << "Failed to set DISPLAY environment variable" << std::endl;
            return;
        }
    }

    Display *dpy = XOpenDisplay(NULL);
    if (!dpy)
    {
        std::cerr << "Unable to open display" << std::endl;
        return;
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

            this->profile.displayDevices.push_back(display);
        }

        XRRFreeCrtcInfo(crtcInfo);
    }

    XRRFreeScreenResources(res);
    XCloseDisplay(dpy);
}

void HardwareScan::getAudioInfo()
{
    this->loudspeakerdevices.clear();
    this->microphonedevices.clear();

    pa_threaded_mainloop *mainloop = pa_threaded_mainloop_new();
    pa_mainloop_api *mainloop_api = pa_threaded_mainloop_get_api(mainloop);
    pa_context *context = pa_context_new(mainloop_api, "Device Details");

    pa_context_set_state_callback(context, context_state_callback, nullptr);
    // std::cout << "connect " << std::endl;
    pa_context_connect(context, nullptr, PA_CONTEXT_NOFLAGS, nullptr);

    // std::cout << "Before run" << std::endl;
    pa_threaded_mainloop_start(mainloop);
    // usleep(2000 * 1000);
    // std::cout << "Quit the loop." << std::endl;
    // pa_mainloop_quit(mainloop, 0);

    // pa_context_disconnect(context);
    // pa_context_unref(context);
    // pa_mainloop_free(mainloop);

    sleep(2);
    pa_threaded_mainloop_stop(mainloop);
    pa_threaded_mainloop_free(mainloop);

    // std::cout << "Output devices:" << std::endl;
    // for (const auto &device : outputDevices)
    // {
    //     std::cout << "  " << device.description << std::endl;
    //     std::cout << "  " << device.name << std::endl;
    //     std::cout << "  " << device.volume << std::endl;
    //     std::cout << "  " << device.mute << std::endl;
    //     std::cout << "  " << device.sampleRate << std::endl;
    //     std::cout << "  " << device.channels << std::endl;
    // }

    // std::cout << "Input devices:" << std::endl;
    // for (const auto &device : inputDevices)
    // {
    //     std::cout << "  " << device.description << std::endl;
    //     std::cout << "  " << device.name << std::endl;
    //     std::cout << "  " << device.volume << std::endl;
    //     std::cout << "  " << device.mute << std::endl;
    //     std::cout << "  " << device.sampleRate << std::endl;
    //     std::cout << "  " << device.channels << std::endl;
    // }
    this->profile.speakerDevices = loudspeakerdevices;
    this->profile.micDevices = microphonedevices;
}

void HardwareScan::context_state_callback(pa_context *c, void *userdata)
{
    if (pa_context_get_state(c) == PA_CONTEXT_READY)
    {
        pa_operation *o;
        std::cout << "ready" << std::endl;

        o = pa_context_get_sink_info_list(c, sink_info_callback, nullptr);
        if (o)
        {
            pa_operation_unref(o);
        }

        // std::cout << "ccc " << std::endl;

        o = pa_context_get_source_info_list(c, source_info_callback, nullptr);
        if (o)
        {
            pa_operation_unref(o);
        }
        // counter = counter + 1;
        // std::cout << "counter : " << counter << std::endl;
    }
    // std::cout << "status: " << pa_context_get_state(c) << std::endl;
    // #define PA_CONTEXT_UNCONNECTED PA_CONTEXT_UNCONNECTED    0
    // #define PA_CONTEXT_CONNECTING PA_CONTEXT_CONNECTING      1
    // #define PA_CONTEXT_AUTHORIZING PA_CONTEXT_AUTHORIZING    2
    // #define PA_CONTEXT_SETTING_NAME PA_CONTEXT_SETTING_NAME  3
    // #define PA_CONTEXT_READY PA_CONTEXT_READY                4
    // #define PA_CONTEXT_FAILED PA_CONTEXT_FAILED              5
    // #define PA_CONTEXT_TERMINATED PA_CONTEXT_TERMINATED      6
}

std::vector<AudioDevice> HardwareScan::loudspeakerdevices;
std::vector<AudioDevice> HardwareScan::microphonedevices;

void HardwareScan::source_info_callback(pa_context *c, const pa_source_info *info, int eol, void *userdata)
{
    if (eol > 0)
        return;

    // std::cout << info->name << std::endl;
    // sourcei = sourcei + 1;
    // std::cout << "source channels: " << info->sample_spec.channels << std::endl;
    if (info && !strstr(info->name, "monitor"))
    {
        microphonedevices.emplace_back(info->name, info->description, pa_cvolume_avg(&(info->volume)), info->mute, info->sample_spec.rate, info->sample_spec.channels);
    }
}

void HardwareScan::sink_info_callback(pa_context *c, const pa_sink_info *info, int eol, void *userdata)
{
    if (eol > 0)
        return;

    // std::cout << info->name << std::endl;
    // sinki = sinki + 1;
    // std::cout << "sink channels: " << info->sample_spec.channels << std::endl;
    if (info && !strstr(info->name, "monitor"))
    {
        loudspeakerdevices.emplace_back(info->name, info->description, pa_cvolume_avg(&(info->volume)), info->mute, info->sample_spec.rate, info->sample_spec.channels);
    }
}

void HardwareScan::Init(std::function<bool(std::string, bool)> callback, std::string hostname)
{
    this->add_device_callback = callback;
    this->localHardwareScan();
    this->hostname_ = hostname;
}

void HardwareScan::localHardwareScan()
{
    getCameraInfo();
    getDisplayInfo();
    getAudioInfo();
}

void HardwareScan::AutoGenerateCR()
{
    if (profile.cameraDevices.size() != 0)
    {
        int count = 1;
        for (auto &iter : profile.cameraDevices)
        {
            CameraInstance instance;
            instance.apiVersion = "stable.example.com/v1";
            instance.kind = CameraDeviceResourcetype;
            instance.metadata.name = "Camera-autogen-" + std::to_string(count);
            count++;
            instance.metadata.namespace_name = "default";
            instance.spec.version = "1.0";
            instance.spec.hostname = hostname_;
            instance.spec.kind = "camera";
            // properties part
            instance.spec.properties.vendor = "unknown";
            instance.spec.properties.resolution = "720";
            instance.spec.properties.location = "unknown";
            instance.spec.properties.wideAngle = 90;
            instance.spec.properties.focusMethod = "unknown";
            instance.spec.properties.telephoto = false;
            instance.spec.properties.interface = "unknown";
            instance.spec.properties.deviceNode = iter.device_path;
            instance.spec.properties.driverName = iter.driver;
            instance.spec.properties.cardType = iter.card;
            instance.spec.properties.busInfo = iter.bus_info;
            instance.spec.properties.description = "ignore";
            for (int i = 0; i < iter.formats.size(); i++)
            {
                instance.spec.properties.supportFormat.emplace_back(iter.formats[i]);
            }
            // status part
            instance.status.occupancy = false;

            std::string data = instance.Marshal();
            if (!add_device_callback(data, false))
            {
                LOG(ERROR) << "add device instance : " << instance.metadata.name << " fail";
            }
            else
            {
                LOG(INFO) << "add device instance : " << instance.metadata.name << " success";
            }
        }
    }
    if (profile.micDevices.size() != 0)
    {
        int count = 0;
        for (auto &iter : profile.micDevices)
        {
            MicrophoneInstance instance;
            instance.apiVersion = "stable.example.com/v1";
            instance.kind = MicrophoneDeviceResourcetype;
            instance.metadata.name = "Mic-autogen-" + std::to_string(count);
            count++;
            instance.metadata.namespace_name = "default";
            instance.spec.version = "1.0";
            instance.spec.hostname = hostname_;
            instance.spec.kind = "microphone";
            // properties part
            instance.spec.properties.channelNumber = 0;
            instance.spec.properties.bitWidth = 0;
            instance.spec.properties.interface = "unknown";
            instance.spec.properties.hardwareName = iter.name;
            instance.spec.properties.sampleRates = std::to_string(iter.sampleRate);
            instance.spec.properties.volume = (int)iter.volume;
            instance.spec.properties.mute = iter.mute;
            instance.spec.properties.description = iter.description;
            // status part
            instance.status.occupancy = false;

            std::string data = instance.Marshal();
            if (!add_device_callback(data, false))
            {
                LOG(ERROR) << "add device instance : " << instance.metadata.name << " fail";
            }
            else
            {
                LOG(INFO) << "add device instance : " << instance.metadata.name << " success";
            }
        }
    }
    if (profile.speakerDevices.size() != 0)
    {
        int count = 0;
        for (auto &iter : profile.speakerDevices)
        {
            LoudspeakerInstance instance;
            instance.apiVersion = "stable.example.com/v1";
            instance.kind = LoudspeakerDeviceResourcetype;
            instance.metadata.name = "Louspeaker-autogen-" + std::to_string(count);
            count++;
            instance.metadata.namespace_name = "default";
            instance.spec.version = "1.0";
            instance.spec.hostname = hostname_;
            instance.spec.kind = "loudspeaker";
            // properties part
            instance.spec.properties.channelNumber = 0;
            instance.spec.properties.bitWidth = 0;
            instance.spec.properties.interface = "unknown";
            instance.spec.properties.hardwareName = iter.name;
            instance.spec.properties.sampleRates = std::to_string(iter.sampleRate);
            instance.spec.properties.volume = (int)iter.volume;
            instance.spec.properties.mute = iter.mute;
            instance.spec.properties.description = iter.description;
            // status part
            instance.status.occupancy = false;

            std::string data = instance.Marshal();
            if (!add_device_callback(data, false))
            {
                LOG(ERROR) << "add device instance : " << instance.metadata.name << " fail";
            }
            else
            {
                LOG(INFO) << "add device instance : " << instance.metadata.name << " success";
            }
        }
    }
}

std::string HardwareScan::GetHardwareDeviceInfo(bool format)
{
    if (format)
    {
        return this->profile.toJson().toStyledString();
    }
    Json::FastWriter writer;
    return writer.write(this->profile.toJson());
}
