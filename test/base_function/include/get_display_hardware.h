#ifndef TEST_BASE_FUNCTION_INCLUDE_GET_DISPLAY_HARDWARE_H_
#define TEST_BASE_FUNCTION_INCLUDE_GET_DISPLAY_HARDWARE_H_

#include <string>
#include <vector>

class DisplayHardwareForTest {
 public:
  int screen;
  int x;
  int y;
  int width;
  int height;
  int mmWidth;
  int mmHeight;

  std::string ToKeyValue(std::string hostname) const;
  void print() const;
};

std::vector<DisplayHardwareForTest> getDisplayInfo();
void print(const std::vector<DisplayHardwareForTest> &display);

#endif  // TEST_BASE_FUNCTION_INCLUDE_GET_DISPLAY_HARDWARE_H_
