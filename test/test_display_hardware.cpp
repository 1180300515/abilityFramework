#include <vector>

#include "include/get_display_hardware.h"

int main(int argc, char **argv) {
  std::vector<DisplayHardwareForTest> display = getDisplayInfo();
  print(display);
}
