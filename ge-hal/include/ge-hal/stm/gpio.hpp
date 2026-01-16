#pragma once

#include "ge-hal/core.hpp"
#include <stm32f429xx.h>

namespace ge {
namespace hal {
namespace stm {

enum class GPIOMode {
  Input = 0,
  Output = 1,
  AlternateFunction = 2,
  Analog = 3
};

struct Pin {
  u8 bank : 4; // A-G
  u8 num : 4;  // 0-15

  Pin(char bankChar, uint8_t numVal);

  GPIO_TypeDef *gpio() const;
  void set_mode(GPIOMode mode) const;
  void set_af(uint8_t af) const;

  void write(bool val) const;
  bool read() const;
  bool toggle() const;
};
} // namespace stm
} // namespace hal
} // namespace ge
