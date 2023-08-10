#ifndef TEST_BASE_FUNCTION_INCLUDE_GET_CAMERA_HARDWARE_H_
#define TEST_BASE_FUNCTION_INCLUDE_GET_CAMERA_HARDWARE_H_

#include <string>
#include <vector>

class CameraHardwareForTest {
 public:
  std::string identifier;
  std::string device_path;
  std::string driver;
  std::string card;
  std::string bus_info;
  std::vector<std::string> formats;
  // ...更多属性和方法...

  CameraHardwareForTest &operator=(const CameraHardwareForTest &other);
  std::string ToKeyValue(std::string hostname) const;
  void print() const;
};

std::vector<CameraHardwareForTest> getCameraInfo();
void print(const std::vector<CameraHardwareForTest> &camera);

#endif  // TEST_BASE_FUNCTION_INCLUDE_GET_CAMERA_HARDWARE_H_
