#ifndef RESOURCEMGR_HARDWARE_SCAN_H_
#define RESOURCEMGR_HARDWARE_SCAN_H_

#include <pulse/pulseaudio.h>

#include <functional>
#include <map>
#include <mutex>  // NOLINT [build/c++11]
#include <optional>
#include <string>
#include <thread>  // NOLINT [build/c++11]
#include <vector>

#include "resourcemgr/hardware_device_profile.h"

class HardwareScan {
 public:
  /**
   * @brief init the hardwarescan
   * @param callback add device callback function
   */
  void Init(std::string hostname);
  /**
   * @brief get the local hardware device json string
   * @param format format string or not
   * @return the json string
   */
  std::string GetHardwareDeviceProfile(bool format);

  std::optional<Json::Value> GetHardware(const std::string &id,
                                         const std::string &key,
                                         const std::string &type);

  bool isAutogenInstance(const std::string &key);

  std::string GetMatchedKey(const std::string &id);

  void UnmatchedHardware(std::vector<CameraHardware> *camera,
                         std::vector<AudioHardware> *mic,
                         std::vector<AudioHardware> *speaker);

  void SetMap(const std::string &id, const std::string &key);

  void DeleteMap(const std::string &id);

  /**
   * @brief scan the local hardware
   */
  void LocalHardwareScan();

  /**
   * @brief generate a autogenCR serial number(only called by AutoGenerateCR)
   * @param type the instance type
   * @return number
   */
  int GenerateSerialNumber(const std::string &type);

  /**
   * @brief print the hardware and instance map
   */
  void PrintMap();

 private:
  /**
   * @brief get the camera hardware info
   */
  void getCameraInfo();
  /**
   * @brief get the display hardware info
   */
  void getDisplayHardware();
  /**
   * @brief get the audio (mic and speaker) hardware info
   */
  void getMicInfo();

  void getSpeakerInfo();

  std::vector<AudioHardware> getAudioList(const std::string &type);

  int retriveOtherAudioParameters(snd_pcm_stream_t stream_type,
                                  AudioHardware *audioDevice);

  /**
   * @brief delete the noexist hadware
   */
  void compareMap();

  /**
   * @brief find the min positive not in the vector
   * @param nums
   * @return
   */
  int firstMissingPositive(std::vector<int> *nums);

  DeviceProfile profile;
  std::mutex camera_hardware_lock_;
  std::mutex mic_hardware_lock_;
  std::mutex speaker_hardware_lock_;
  std::mutex display_hardware_lock_;

  std::string hostname_;

  // record the hardware identifier -> the instance key
  std::map<std::string, std::string> hardware_instance_map;
  std::mutex map_lock_;
  // 申请锁顺序map -> camera -> mic -> speaker -> display
};

#endif  // RESOURCEMGR_HARDWARE_SCAN_H_
