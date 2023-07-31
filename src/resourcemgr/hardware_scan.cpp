#include "hardware_scan.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <glob.h>
#include <linux/videodev2.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include "glog/logging.h"

#include "device_instance_camera.h"
#include "device_instance_loudspeaker.h"
#include "device_instance_microphone.h"
#include "global_var.h"
#include "resource_manager.h"
#include "color.h"

void HardwareScan::getCameraInfo()
{
    std::lock_guard<std::mutex> locker(camera_hardware_lock_);
    this->profile.cameraDevices.clear();

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

void HardwareScan::getDisplayHardware()
{
    std::lock_guard<std::mutex> locker(display_hardware_lock_);
    this->profile.displayDevices.clear();

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
            DisplayHardware display;
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
    std::lock_guard<std::mutex> locker(audio_hardware_lock_);
    this->loudspeakerdevices.clear();
    this->microphonedevices.clear();
    microphonedevices.clear();
    loudspeakerdevices.clear();

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

void HardwareScan::periodicHardwareScanThread()
{
    while (true)
    {
        // sleep 10 minute;
        std::this_thread::sleep_for(std::chrono::minutes(1));

        localHardwareScan();
        std::map<std::string, CameraHardware> camera_;
        std::map<std::string, AudioHardware> mic_;
        std::map<std::string, AudioHardware> speaker_;
        generateIndentifyKeyValue(camera_, mic_, speaker_);
        {
            std::lock_guard<std::mutex> locker1(this->map_lock_);
            this->resource_manager->InsertHardwareInfo(camera_, mic_, speaker_, this->hardware_instance_map);
        }
        this->AutoGenerateCR();
        this->printMap();
    }
}

void HardwareScan::printMap()
{
    std::lock_guard<std::mutex> locker1(this->map_lock_);
    for (const auto &iter : this->hardware_instance_map)
    {
        std::cout << L_BLUE << iter.first << "\t         " << iter.second << NONE << std::endl;
    }
}

void HardwareScan::compareMap()
{
    LOG(INFO) << "compare old hardware and new hardware, delete the non-exist hardware";
    std::lock_guard<std::mutex> locker1(this->map_lock_);
    std::lock_guard<std::mutex> locker2(audio_hardware_lock_);
    std::lock_guard<std::mutex> locker3(camera_hardware_lock_);
    for (auto iter = hardware_instance_map.begin(); iter != hardware_instance_map.end();)
    {
        bool delete_ = true;
        for (const auto &hw : profile.cameraDevices)
        {
            if (iter->first == hw.card)
            {
                delete_ = false;
            }
        }
        for (const auto &hw : profile.micDevices)
        {
            if (iter->first == hw.name)
            {
                delete_ = false;
            }
        }
        for (const auto &hw : profile.speakerDevices)
        {
            if (iter->first == hw.name)
            {
                delete_ = false;
            }
        }
        if (delete_)
        {
            if (this->resource_manager->deleteDeviceInstance(iter->second))
            {
                iter = hardware_instance_map.erase(iter);
            }
            else
            {
                LOG(ERROR) << "erase autogen CR fail : " << iter->second;
                ++iter;
            }
        }
        else
        {
            ++iter;
        }
    }
    LOG(INFO) << "compare finish";
}

int HardwareScan::gnerateSerialNumber(const std::string &type)
{
    if (type == "camera")
    {
        std::vector<int> numbers;
        std::string prefix = "default/Autogen-Camera-";
        for (const auto &iter : this->hardware_instance_map)
        {
            auto a = iter.second;
            if (a.rfind(prefix, 0) == 0)
            {
                a.erase(0, prefix.length());
                numbers.emplace_back(std::stoi(a));
            }
        }
        return firstMissingPositive(numbers);
    }
    else if (type == "mic")
    {
        std::vector<int> numbers;
        std::string prefix = "default/Autogen-Mic-";
        for (const auto &iter : this->hardware_instance_map)
        {
            auto a = iter.second;
            if (a.rfind(prefix, 0) == 0)
            {
                a.erase(0, prefix.length());
                numbers.emplace_back(std::stoi(a));
            }
        }
        return firstMissingPositive(numbers);
    }
    else if (type == "speaker")
    {
        std::vector<int> numbers;
        std::string prefix = "default/Autogen-Loudspeaker-";
        for (const auto &iter : this->hardware_instance_map)
        {
            auto a = iter.second;
            if (a.rfind(prefix, 0) == 0)
            {
                a.erase(0, prefix.length());
                numbers.emplace_back(std::stoi(a));
            }
        }
        return firstMissingPositive(numbers);
    }
    return 0;
}

int HardwareScan::firstMissingPositive(std::vector<int> &nums)
{
    int n = nums.size();
    for (int i = 0; i < n; ++i)
    {
        int num = abs(nums[i]);
        if (num <= n)
        {
            nums[num - 1] = -abs(nums[num - 1]);
        }
    }
    for (int i = 0; i < n; ++i)
    {
        if (nums[i] > 0)
        {
            return i + 1;
        }
    }
    return n + 1;
}

bool HardwareScan::isAutogenInstance(const std::string &key)
{
    int flag = key.find("-");
    std::string prefix = key.substr(0, flag);
    if (prefix == "default/Autogen")
    {
        return true;
    }
    return false;
}

std::string HardwareScan::GetMatchedKey(const std::string &id)
{
    std::lock_guard<std::mutex> locker1(this->map_lock_);
    if (this->hardware_instance_map.count(id) == 0)
    {
        return "";
    }
    else
    {
        return this->hardware_instance_map.at(id);
    }
}

void HardwareScan::SetMap(const std::string &id, const std::string &key)
{
    std::lock_guard<std::mutex> locker1(this->map_lock_);
    hardware_instance_map[id] = key;
}

void HardwareScan::DeleteMap(const std::string &key)
{
    std::lock_guard<std::mutex> locker1(this->map_lock_);
    for (auto iter = hardware_instance_map.begin(); iter != hardware_instance_map.end();)
    {
        if (iter->second == key)
        {
            iter = hardware_instance_map.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void HardwareScan::generateIndentifyKeyValue(std::map<std::string, CameraHardware> &camera, std::map<std::string, AudioHardware> &mic, std::map<std::string, AudioHardware> &speaker)
{
    {
        std::lock_guard<std::mutex> locker(audio_hardware_lock_);
        for (const auto &iter : profile.micDevices)
        {
            mic[iter.name] = iter;
            // LOG(INFO) << RED << iter.name << NONE;
        }
        for (const auto &iter : profile.speakerDevices)
        {
            // LOG(INFO) << RED << iter.name << NONE;
            speaker[iter.name] = iter;
        }
    }
    {
        std::lock_guard<std::mutex> locker(camera_hardware_lock_);
        for (const auto &iter : profile.cameraDevices)
        {
            // LOG(INFO) << RED << iter.card << NONE;
            camera[iter.card] = iter;
        }
    }
}

void HardwareScan::context_state_callback(pa_context *c, void *userdata)
{
    if (pa_context_get_state(c) == PA_CONTEXT_READY)
    {
        pa_operation *o;

        o = pa_context_get_sink_info_list(c, sink_info_callback, nullptr);
        if (o)
        {
            pa_operation_unref(o);
        }

        o = pa_context_get_source_info_list(c, source_info_callback, nullptr);
        if (o)
        {
            pa_operation_unref(o);
        }
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

std::vector<AudioHardware> HardwareScan::loudspeakerdevices;
std::vector<AudioHardware> HardwareScan::microphonedevices;

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

void HardwareScan::Init(std::shared_ptr<ResourceManager> manager_, std::string hostname)
{
    this->resource_manager = manager_;
    this->hostname_ = hostname;
    this->localHardwareScan();
}

void HardwareScan::Run()
{
    LOG(INFO) << "begin match the hardware with the device instance";
    std::map<std::string, CameraHardware> camera_;
    std::map<std::string, AudioHardware> mic_;
    std::map<std::string, AudioHardware> speaker_;
    generateIndentifyKeyValue(camera_, mic_, speaker_);
    {
        std::lock_guard<std::mutex> locker1(this->map_lock_);
        this->resource_manager->InsertHardwareInfo(camera_, mic_, speaker_, this->hardware_instance_map);
    }
    this->AutoGenerateCR();
    this->printMap();
    LOG(INFO) << L_GREEN << "local hardware match and autogen finish" << NONE;
    this->periodic_scan_thread = std::thread(&HardwareScan::periodicHardwareScanThread, this);
}

void HardwareScan::Wait()
{
    this->periodic_scan_thread.join();
}

void HardwareScan::localHardwareScan()
{
    LOG(INFO) << L_GREEN << "local hardware scan begin" << NONE;
    getCameraInfo();
    getDisplayHardware();
    getAudioInfo();
    LOG(INFO) << "hardware scan finish : camera: " << this->profile.cameraDevices.size()
              << "  mic: " << this->profile.micDevices.size()
              << "  speaker: " << this->profile.speakerDevices.size()
              << "  display: " << this->profile.displayDevices.size();
    compareMap();
}

void HardwareScan::AutoGenerateCR()
{
    LOG(INFO) << "begin auto generate unmatched hardware CR";
    std::lock_guard<std::mutex> locker1(this->map_lock_);
    std::lock_guard<std::mutex> locker2(audio_hardware_lock_);
    std::lock_guard<std::mutex> locker3(camera_hardware_lock_);
    for (const auto &iter : profile.cameraDevices)
    {
        if (this->hardware_instance_map.count(iter.card) == 0)
        {
            CameraInstance instance;
            instance.apiVersion = "stable.example.com/v1";
            instance.kind = CameraDeviceResourcetype;
            int count = gnerateSerialNumber("camera");
            instance.metadata.name = "Autogen-Camera-" + std::to_string(count);
            instance.metadata.namespace_name = "default";
            instance.spec.version = "1.0";
            instance.spec.hostname = hostname_;
            instance.spec.kind = "camera";
            instance.spec.hardwareidentifier = iter.card;
            // properties part
            instance.spec.properties.vendor = "unknown";
            instance.spec.properties.resolution = "unknown";
            instance.spec.properties.location = "unknown";
            instance.spec.properties.wideAngle = 0;
            instance.spec.properties.focusMethod = "unknown";
            instance.spec.properties.telephoto = false;
            instance.spec.properties.interface = "unknown";
            instance.spec.properties.devicePath = iter.device_path;
            instance.spec.properties.driverName = iter.driver;
            instance.spec.properties.card = iter.card;
            instance.spec.properties.busInfo = iter.bus_info;
            instance.spec.properties.description = "ignore";
            for (int i = 0; i < iter.formats.size(); i++)
            {
                instance.spec.properties.supportFormat.emplace_back(iter.formats[i]);
            }
            // status part
            instance.status.occupancy = false;

            std::string data = instance.Marshal();
            if (!this->resource_manager->AddDeviceInstance(data))
            {
                LOG(ERROR) << "autogen CR: " << instance.metadata.name << " add fail";
            }
            else
            {
                // LOG(INFO) << "add device instance : " << instance.metadata.name << " success";
                this->hardware_instance_map[iter.card] = instance.metadata.namespace_name + "/" + instance.metadata.name;
            }
        }
    }
    for (const auto &iter : profile.micDevices)
    {
        if (this->hardware_instance_map.count(iter.name) == 0)
        {
            MicrophoneInstance instance;
            instance.apiVersion = "stable.example.com/v1";
            instance.kind = MicrophoneDeviceResourcetype;
            int count = gnerateSerialNumber("mic");
            instance.metadata.name = "Autogen-Mic-" + std::to_string(count);
            instance.metadata.namespace_name = "default";

            instance.spec.version = "1.0";
            instance.spec.hostname = hostname_;
            instance.spec.kind = "microphone";
            instance.spec.hardwareidentifier = iter.name;
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
            if (!this->resource_manager->AddDeviceInstance(data))
            {
                LOG(ERROR) << "autogen CR: " << instance.metadata.name << " add fail";
            }
            else
            {
                // LOG(INFO) << "add device instance : " << instance.metadata.name << " success";
                this->hardware_instance_map[iter.name] = instance.metadata.namespace_name + "/" + instance.metadata.name;
            }
        }
    }
    for (const auto &iter : profile.speakerDevices)
    {
        if (this->hardware_instance_map.count(iter.name) == 0)
        {
            LoudspeakerInstance instance;
            instance.apiVersion = "stable.example.com/v1";
            instance.kind = LoudspeakerDeviceResourcetype;
            int count = gnerateSerialNumber("speaker");
            instance.metadata.name = "Autogen-Loudspeaker-" + std::to_string(count);
            instance.metadata.namespace_name = "default";

            instance.spec.version = "1.0";
            instance.spec.hostname = hostname_;
            instance.spec.kind = "loudspeaker";
            instance.spec.hardwareidentifier = iter.name;
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
            if (!this->resource_manager->AddDeviceInstance(data))
            {
                LOG(ERROR) << "autogen CR: " << instance.metadata.name << " add fail";
            }
            else
            {
                // LOG(INFO) << "add device instance : " << instance.metadata.name << " success";
                this->hardware_instance_map[instance.spec.properties.hardwareName] = instance.metadata.namespace_name + "/" + instance.metadata.name;
            }
        }
    }
    LOG(INFO) << "finish auto generate";
}

std::string HardwareScan::GetHardwareDeviceInfo(bool format)
{
    std::lock_guard<std::mutex> locker1(camera_hardware_lock_);
    std::lock_guard<std::mutex> locker2(audio_hardware_lock_);
    std::lock_guard<std::mutex> locker3(display_hardware_lock_);
    if (format)
    {
        return this->profile.toJson().toStyledString();
    }
    Json::FastWriter writer;
    return writer.write(this->profile.toJson());
}

std::optional<CameraHardware> HardwareScan::GetCameraHardware(const std::string &id, const std::string &key)
{
    if (this->isAutogenInstance(key))
    {
        return std::nullopt;
    }
    std::lock_guard<std::mutex> locker1(this->camera_hardware_lock_);
    std::lock_guard<std::mutex> locker2(this->map_lock_);
    for (const auto &iter : profile.cameraDevices)
    {
        if (iter.card == id)
        {
            if (this->hardware_instance_map.count(id) != 0 && !this->isAutogenInstance(this->hardware_instance_map[id]))
            {
                LOG(ERROR) << "the camera hardware: \"" << id << "\" already match with: " << this->hardware_instance_map[id];
                return std::nullopt;
            }
            else
            {
                return iter;
            }
        }
    }
    LOG(INFO) << "no match hardware with: \"" << id << "\"";
    return std::nullopt;
}

std::optional<AudioHardware> HardwareScan::GetMicHardware(const std::string &id, const std::string &key)
{
    if (this->isAutogenInstance(key))
    {
        return std::nullopt;
    }
    std::lock_guard<std::mutex> locker1(this->audio_hardware_lock_);
    std::lock_guard<std::mutex> locker2(this->map_lock_);
    for (const auto &iter : profile.micDevices)
    {
        if (iter.name == id)
        {
            if (this->hardware_instance_map.count(id) != 0 && !this->isAutogenInstance(this->hardware_instance_map[id]))
            {
                LOG(ERROR) << "the mic hardware: \"" << id << "\" already match with: " << this->hardware_instance_map[id];
                return std::nullopt;
            }
            else
            {
                return iter;
            }
        }
    }
    LOG(INFO) << "no match hardware with: \"" << id << "\"";
    return std::nullopt;
}

std::optional<AudioHardware> HardwareScan::GetSpeakerHardware(const std::string &id, const std::string &key)
{
    if (this->isAutogenInstance(key))
    {
        return std::nullopt;
    }
    std::lock_guard<std::mutex> locker1(this->audio_hardware_lock_);
    std::lock_guard<std::mutex> locker2(this->map_lock_);
    for (const auto &iter : profile.speakerDevices)
    {
        if (iter.name == id)
        {
            if (this->hardware_instance_map.count(id) != 0 && !this->isAutogenInstance(this->hardware_instance_map[id]))
            {
                LOG(ERROR) << "the speaker hardware: \"" << id << "\" already match with: " << this->hardware_instance_map[id];
                return std::nullopt;
            }
            else
            {
                return iter;
            }
        }
    }
    LOG(INFO) << "no match hardware with: \"" << id << "\"";
    return std::nullopt;
}
