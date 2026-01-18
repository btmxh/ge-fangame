#include "ge-hal/app.hpp"

extern "C" void SystemInit() {
  ge::App::system_init();
}
