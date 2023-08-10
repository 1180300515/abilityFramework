#ifndef TEST_BASE_FUNCTION_INCLUDE_GET_AUDIO_HARDWARE_H_
#define TEST_BASE_FUNCTION_INCLUDE_GET_AUDIO_HARDWARE_H_

#include <alsa/asoundlib.h>
#include <json/json.h>
#include <unistd.h>

#include <string>
#include <vector>

class AudioHardwareForTest {
 public:
  std::string name;
  std::string description;
  std::vector<uint32_t> sampleRate;
  std::vector<uint8_t> channels;
  std::vector<std::string> format;
  std::string cardID;
  std::string deviceID;

  AudioHardwareForTest() = default;
  AudioHardwareForTest(const std::string &name, const std::string &description,
                const std::vector<uint32_t> &sampleRate,
                const std::vector<uint8_t> &channels,
                const std::vector<std::string> &format,
                const std::string &cardID, const std::string &deviceID)
      : name(name),
        description(description),
        sampleRate(sampleRate),
        channels(channels),
        format(format),
        cardID(cardID),
        deviceID(deviceID) {}

  std::string ToKeyValue(std::string hostname) const;
  void print() const;
};

extern const unsigned int rates[13];
extern const snd_pcm_format_t formats[38];

std::vector<AudioHardwareForTest> getAudioList(const std::string &type);
void print(const std::vector<AudioHardwareForTest> &audio);

#endif  // TEST_BASE_FUNCTION_INCLUDE_GET_AUDIO_HARDWARE_H_
