#include "resourcemgr/hardware_scan.h"

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <fcntl.h>
#include <glob.h>
#include <glog/logging.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>

#include "resourcemgr/device_instance_camera.h"
#include "resourcemgr/device_instance_loudspeaker.h"
#include "resourcemgr/device_instance_microphone.h"
#include "utils/color.h"
#include "utils/global_var.h"

void HardwareScan::getCameraInfo() {
  this->profile.cameraDevices.clear();
  glob_t glob_result;
  // 找到所有的V4L设备
  glob("/dev/video*", GLOB_TILDE, nullptr, &glob_result);
  for (unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
    const char *path = glob_result.gl_pathv[i];
    int fd = open(path, O_RDONLY);

    if (fd == -1) {
      std::cerr << "Failed to open " << path << std::endl;
      continue;
    }

    // 获取设备的能力
    v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
      std::cerr << "Failed to get device capabilities for " << path
                << std::endl;
      close(fd);
      continue;
    }

    // 枚举设备支持的像素格式
    v4l2_fmtdesc fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    char fourcc[5] = {0};
    std::vector<std::string> formats;
    while (ioctl(fd, VIDIOC_ENUM_FMT, &fmt) == 0) {
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
    for (auto &existing_device : this->profile.cameraDevices) {
      if (existing_device.bus_info == device.bus_info) {
        device_exists = true;
        if (existing_device.device_path > device.device_path) {
          existing_device =
              device;  // Note: this requires CameraDevice to be assignable
        }
        break;
      }
    }

    if (!device_exists) {
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

void HardwareScan::getDisplayHardware() {
  this->profile.displayDevices.clear();

  const char *displayEnv = getenv("DISPLAY");
  if (!displayEnv) {
    if (setenv("DISPLAY", ":0", 0) == -1) {
      std::cerr << "Failed to set DISPLAY environment variable" << std::endl;
      return;
    }
  }

  Display *dpy = XOpenDisplay(NULL);
  if (!dpy) {
    std::cerr << "Unable to open display" << std::endl;
    return;
  }

  int screen = DefaultScreen(dpy);
  Window root = RootWindow(dpy, screen);

  XRRScreenResources *res = XRRGetScreenResourcesCurrent(dpy, root);

  for (int crtc = 0; crtc < res->ncrtc; ++crtc) {
    XRRCrtcInfo *crtcInfo = XRRGetCrtcInfo(dpy, res, res->crtcs[crtc]);

    if (crtcInfo->mode != None) {
      DisplayHardware display;
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

      this->profile.displayDevices.push_back(display);
    }

    XRRFreeCrtcInfo(crtcInfo);
  }

  XRRFreeScreenResources(res);
  XCloseDisplay(dpy);
}

void HardwareScan::getMicInfo() {
  this->profile.micDevices = getAudioList("record");
}

void HardwareScan::getSpeakerInfo() {
  this->profile.speakerDevices = getAudioList("playback");
}

std::vector<AudioHardware> HardwareScan::getAudioList(const std::string &type) {
  std::vector<AudioHardware> audioDevices;
  const char *command;
  snd_pcm_stream_t stream;
  if (type == "record") {
    command = "arecord";
    stream = SND_PCM_STREAM_CAPTURE;
  } else if (type == "playback") {
    command = "aplay";
    stream = SND_PCM_STREAM_PLAYBACK;
  } else {
    LOG(ERROR) << "Wrong type." << std::endl;
    return audioDevices;
  }

  snd_ctl_t *handle;
  int card, err, dev, idx;
  snd_ctl_card_info_t *info;
  snd_pcm_info_t *pcminfo;
  snd_ctl_card_info_alloca(&info);
  snd_pcm_info_alloca(&pcminfo);

  card = -1;
  if (snd_card_next(&card) < 0 || card < 0) {
    LOG(ERROR) << "No soundcards found...";
    return audioDevices;
  }
  snd_pcm_stream_name(stream);
  while (card >= 0) {
    char name[32];
    snprintf(name, sizeof(name), "hw:%d", card);
    if ((err = snd_ctl_open(&handle, name, 0)) < 0) {
      LOG(ERROR) << "control open " << card << " error: " << snd_strerror(err);
      goto next_card;
    }
    if ((err = snd_ctl_card_info(handle, info)) < 0) {
      LOG(ERROR) << "control hardware info " << card
                 << " error: " << snd_strerror(err);
      snd_ctl_close(handle);
      goto next_card;
    }
    dev = -1;
    while (1) {
      unsigned int count;
      if (snd_ctl_pcm_next_device(handle, &dev) < 0)
        std::cout << "snd_ctl_pcm_next_device" << std::endl;
      if (dev < 0) break;
      snd_pcm_info_set_device(pcminfo, dev);
      snd_pcm_info_set_subdevice(pcminfo, 0);
      snd_pcm_info_set_stream(pcminfo, stream);
      if ((err = snd_ctl_pcm_info(handle, pcminfo)) < 0) {
        if (err != -ENOENT)
          LOG(ERROR) << "control digital audio info (" << card
                     << "): " << snd_strerror(err);
        continue;
      }
      std::string name;
      if (type == "record") {
        name = "mic.card-" + std::to_string(card) + ".device-" +
               std::to_string(dev) + "." +
               std::string(snd_ctl_card_info_get_name(info));
      } else if (type == "playback") {
        name = "speaker.card-" + std::to_string(card) + ".device-" +
               std::to_string(dev) + "." +
               std::string(snd_ctl_card_info_get_name(info));
      }

      AudioHardware audioDevice(name, snd_pcm_info_get_name(pcminfo), {}, {},
                                {}, std::to_string(card), std::to_string(dev));
      if (retriveOtherAudioParameters(stream, &audioDevice)) continue;
      audioDevices.push_back(audioDevice);
    }

    snd_ctl_close(handle);
  next_card:
    if (snd_card_next(&card) < 0) {
      std::cout << "snd_card_next" << std::endl;
      break;
    }
  }
  return audioDevices;
}

int HardwareScan::retriveOtherAudioParameters(snd_pcm_stream_t stream_type,
                                              AudioHardware *audioDevice) {
  snd_pcm_t *pcm;
  snd_pcm_hw_params_t *hw_params;
  unsigned int i;
  unsigned int min, max;
  int any_rate;
  int err;
  std::string deviceName =
      "hw:" + audioDevice->cardID + "," + audioDevice->deviceID;
  const char *device_name = deviceName.c_str();

  err = snd_pcm_open(&pcm, device_name, stream_type, SND_PCM_NONBLOCK);
  if (err < 0) {
    fprintf(stderr, "cannot open device '%s': %s\n", device_name,
            snd_strerror(err));
    return 1;
  }

  snd_pcm_hw_params_alloca(&hw_params);
  err = snd_pcm_hw_params_any(pcm, hw_params);
  if (err < 0) {
    fprintf(stderr, "cannot get hardware parameters: %s\n", snd_strerror(err));
    snd_pcm_close(pcm);
    return 1;
  }
  for (i = 0; i < ARRAY_SIZE(formats); ++i) {
    if (!snd_pcm_hw_params_test_format(pcm, hw_params, formats[i]))
      audioDevice->format.push_back(snd_pcm_format_name(formats[i]));
    // printf(" %s", snd_pcm_format_name(formats[i]));
  }

  err = snd_pcm_hw_params_get_channels_min(hw_params, &min);
  if (err < 0) {
    fprintf(stderr, "cannot get minimum channels count: %s\n",
            snd_strerror(err));
    snd_pcm_close(pcm);
    return 1;
  }
  err = snd_pcm_hw_params_get_channels_max(hw_params, &max);
  if (err < 0) {
    fprintf(stderr, "cannot get maximum channels count: %s\n",
            snd_strerror(err));
    snd_pcm_close(pcm);
    return 1;
  }
  // printf("Channels:");
  for (i = min; i <= max; ++i) {
    if (!snd_pcm_hw_params_test_channels(pcm, hw_params, i)) {
      audioDevice->channels.push_back(i);
      // std::cout << "*********** CHANNEL: " << i << std::endl;
    }
    // printf(" %u", i);
  }

  err = snd_pcm_hw_params_get_rate_min(hw_params, &min, NULL);
  if (err < 0) {
    fprintf(stderr, "cannot get minimum rate: %s\n", snd_strerror(err));
    snd_pcm_close(pcm);
    return 1;
  }
  err = snd_pcm_hw_params_get_rate_max(hw_params, &max, NULL);
  if (err < 0) {
    fprintf(stderr, "cannot get maximum rate: %s\n", snd_strerror(err));
    snd_pcm_close(pcm);
    return 1;
  }
  // printf("Sample rates:");
  if (min == max) {
    // printf(" %u", min);
    audioDevice->sampleRate.push_back(min);
    // std::cout << "*********** RATE: " << min << std::endl;
  } else if (!snd_pcm_hw_params_test_rate(pcm, hw_params, min + 1, 0)) {
    printf(" %u-%u", min, max);
  } else {
    any_rate = 0;
    for (i = 0; i < ARRAY_SIZE(rates); ++i) {
      if (!snd_pcm_hw_params_test_rate(pcm, hw_params, rates[i], 0)) {
        any_rate = 1;
        audioDevice->sampleRate.push_back(rates[i]);
        // std::cout << "*********** RATE: " << rates[i] << std::endl;
        // printf(" %u", rates[i]);
      }
    }
    if (!any_rate) printf(" (empty)");
  }

  snd_pcm_close(pcm);
  return 0;
}

void HardwareScan::PrintMap() {
  std::lock_guard<std::mutex> locker1(this->map_lock_);
  for (const auto &iter : this->hardware_instance_map) {
    std::cout << L_BLUE << iter.first << "\t         " << iter.second << NONE
              << std::endl;
  }
}

void HardwareScan::compareMap() {
  LOG(INFO) << "compare old hardware and new hardware, delete the non-exist "
               "hardware,add new hardware record";
  for (auto &iter : hardware_instance_map) {
    bool delete_ = true;
    for (const auto &hw : profile.cameraDevices) {
      if (iter.first == hw.card) {
        delete_ = false;
      }
    }
    for (const auto &hw : profile.micDevices) {
      if (iter.first == hw.name) {
        delete_ = false;
      }
    }
    for (const auto &hw : profile.speakerDevices) {
      if (iter.first == hw.name) {
        delete_ = false;
      }
    }
    if (delete_) {
      iter.second = "delete";
    }
  }
  for (const auto &hw : profile.cameraDevices) {
    if (hardware_instance_map.count(hw.bus_info) == 0) {
      hardware_instance_map[hw.bus_info] = "";
    }
  }
  for (const auto &hw : profile.micDevices) {
    if (hardware_instance_map.count(hw.name) == 0) {
      hardware_instance_map[hw.name] = "";
    }
  }
  for (const auto &hw : profile.speakerDevices) {
    if (hardware_instance_map.count(hw.name) == 0) {
      hardware_instance_map[hw.name] = "";
    }
  }
  LOG(INFO) << "compare finish";
}

int HardwareScan::GenerateSerialNumber(const std::string &type) {
  std::lock_guard<std::mutex> locker1(this->map_lock_);
  if (type == "camera") {
    std::vector<int> numbers;
    std::string prefix = "default/Autogen-Camera-";
    for (const auto &iter : this->hardware_instance_map) {
      auto a = iter.second;
      if (a.rfind(prefix, 0) == 0) {
        a.erase(0, prefix.length());
        numbers.emplace_back(std::stoi(a));
      }
    }
    return firstMissingPositive(&numbers);
  } else if (type == "mic") {
    std::vector<int> numbers;
    std::string prefix = "default/Autogen-Mic-";
    for (const auto &iter : this->hardware_instance_map) {
      auto a = iter.second;
      if (a.rfind(prefix, 0) == 0) {
        a.erase(0, prefix.length());
        numbers.emplace_back(std::stoi(a));
      }
    }
    return firstMissingPositive(&numbers);
  } else if (type == "speaker") {
    std::vector<int> numbers;
    std::string prefix = "default/Autogen-Loudspeaker-";
    for (const auto &iter : this->hardware_instance_map) {
      auto a = iter.second;
      if (a.rfind(prefix, 0) == 0) {
        a.erase(0, prefix.length());
        numbers.emplace_back(std::stoi(a));
      }
    }
    return firstMissingPositive(&numbers);
  }
  return 0;
}

int HardwareScan::firstMissingPositive(std::vector<int> *nums) {
  int n = nums->size();
  for (int i = 0; i < n; ++i) {
    int num = abs((*nums)[i]);
    if (num <= n) {
      (*nums)[num - 1] = -abs((*nums)[num - 1]);
    }
  }
  for (int i = 0; i < n; ++i) {
    if ((*nums)[i] > 0) {
      return i + 1;
    }
  }
  return n + 1;
}

bool HardwareScan::isAutogenInstance(const std::string &key) {
  int flag = key.find("-");
  std::string prefix = key.substr(0, flag);
  if (prefix == "default/Autogen") {
    return true;
  }
  return false;
}

std::string HardwareScan::GetMatchedKey(const std::string &id) {
  std::lock_guard<std::mutex> locker1(this->map_lock_);
  if (this->hardware_instance_map.count(id) == 0) {
    return "not-exist";
  } else {
    return this->hardware_instance_map.at(id);
  }
}

void HardwareScan::UnmatchedHardware(std::vector<CameraHardware> *camera,
                                     std::vector<AudioHardware> *mic,
                                     std::vector<AudioHardware> *speaker) {
  std::lock_guard<std::mutex> locker1(this->map_lock_);
  std::lock_guard<std::mutex> locker2(camera_hardware_lock_);
  std::lock_guard<std::mutex> locker3(mic_hardware_lock_);
  std::lock_guard<std::mutex> locker4(speaker_hardware_lock_);
  for (const auto &hw : profile.cameraDevices) {
    if (hardware_instance_map.count(hw.bus_info) != 0 &&
        hardware_instance_map.at(hw.bus_info) == "") {
      camera->emplace_back(hw);
    }
  }
  for (const auto &hw : profile.micDevices) {
    if (hardware_instance_map.count(hw.name) != 0 &&
        hardware_instance_map.at(hw.name) == "") {
      mic->emplace_back(hw);
    }
  }
  for (const auto &hw : profile.speakerDevices) {
    if (hardware_instance_map.count(hw.name) != 0 &&
        hardware_instance_map.at(hw.name) == "") {
      speaker->emplace_back(hw);
    }
  }
}

void HardwareScan::SetMap(const std::string &id, const std::string &key) {
  std::lock_guard<std::mutex> locker1(this->map_lock_);
  if (hardware_instance_map.at(id) != key) {
    LOG(INFO) << "instance : " << key << " match with hardware : " << id;
  }
  hardware_instance_map[id] = key;
}

void HardwareScan::DeleteMap(const std::string &id) {
  std::lock_guard<std::mutex> locker1(this->map_lock_);
  this->hardware_instance_map.erase(id);
}

void HardwareScan::Init(std::string hostname) {
  this->hostname_ = hostname;
  this->LocalHardwareScan();
}

void HardwareScan::LocalHardwareScan() {
  LOG(INFO) << L_GREEN << "local hardware scan begin" << NONE;
  std::lock_guard<std::mutex> locker1(this->map_lock_);
  std::lock_guard<std::mutex> locker2(camera_hardware_lock_);
  std::lock_guard<std::mutex> locker3(mic_hardware_lock_);
  std::lock_guard<std::mutex> locker4(speaker_hardware_lock_);
  std::lock_guard<std::mutex> locker5(display_hardware_lock_);
  getCameraInfo();
  getMicInfo();
  getSpeakerInfo();
  getDisplayHardware();
  LOG(INFO) << "hardware scan finish : camera: "
            << this->profile.cameraDevices.size()
            << "  mic: " << this->profile.micDevices.size()
            << "  speaker: " << this->profile.speakerDevices.size()
            << "  display: " << this->profile.displayDevices.size();
  compareMap();
}

std::string HardwareScan::GetHardwareDeviceProfile(bool format) {
  std::lock_guard<std::mutex> locker1(camera_hardware_lock_);
  std::lock_guard<std::mutex> locker2(mic_hardware_lock_);
  std::lock_guard<std::mutex> locker3(speaker_hardware_lock_);
  std::lock_guard<std::mutex> locker4(display_hardware_lock_);
  if (format) {
    return this->profile.toJson().toStyledString();
  }
  Json::FastWriter writer;
  return writer.write(this->profile.toJson());
}

std::optional<Json::Value> HardwareScan::GetHardware(const std::string &id,
                                                     const std::string &key,
                                                     const std::string &type) {
  if (id == "") {
    return std::nullopt;
  }
  if (this->isAutogenInstance(key)) {
    return std::nullopt;
  }
  if (type == CameraDeviceResourcetype) {
    std::lock_guard<std::mutex> locker2(this->map_lock_);
    std::lock_guard<std::mutex> locker1(this->camera_hardware_lock_);
    for (const auto &iter : profile.cameraDevices) {
      if (iter.bus_info == id) {
        auto key_in_map = hardware_instance_map.at(id);
        if (key_in_map != "" && key_in_map != key &&
            !isAutogenInstance(key_in_map)) {
          LOG(ERROR) << "the camera hardware: \"" << id
                     << "\" already match with: " << key_in_map;
          return std::nullopt;
        } else {
          return iter.toJson();
        }
      }
    }
  } else if (type == LoudspeakerDeviceResourcetype) {
    std::lock_guard<std::mutex> locker2(this->map_lock_);
    std::lock_guard<std::mutex> locker4(speaker_hardware_lock_);
    for (const auto &iter : profile.speakerDevices) {
      if (iter.name == id) {
        auto key_in_map = hardware_instance_map.at(id);
        if (key_in_map != "" && key_in_map != key &&
            !isAutogenInstance(key_in_map)) {
          LOG(ERROR) << "the speaker hardware: \"" << id
                     << "\" already match with: " << key_in_map;
          return std::nullopt;
        } else {
          return iter.toJson();
        }
      }
    }
  } else if (type == MicrophoneDeviceResourcetype) {
    std::lock_guard<std::mutex> locker2(this->map_lock_);
    std::lock_guard<std::mutex> locker3(mic_hardware_lock_);
    for (const auto &iter : profile.micDevices) {
      if (iter.name == id) {
        auto key_in_map = hardware_instance_map.at(id);
        if (key_in_map != "" && key_in_map != key &&
            !isAutogenInstance(key_in_map)) {
          LOG(ERROR) << "the mic hardware: \"" << id
                     << "\" already match with: " << key_in_map;
          return std::nullopt;
        } else {
          return iter.toJson();
        }
      }
    }
  } else {
    return std::nullopt;
  }
  return std::nullopt;
}
