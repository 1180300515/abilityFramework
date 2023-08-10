#include <vector>
#include <string>

#include "include/get_audio_hardware.h"

int main(int argc, char **argv) {
  std::vector<AudioHardwareForTest> speaker = getAudioList("playback");
  print(speaker);
}
