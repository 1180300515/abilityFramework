#include <vector>

#include "include/get_camera_hardware.h"

int main(int argc, char **argv) {
  std::vector<CameraHardwareForTest> camera = getCameraInfo();
  print(camera);
}
