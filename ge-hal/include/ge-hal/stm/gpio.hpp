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

enum class GPIOOType { PushPull = 0, OpenDrain = 1 };

enum class GPIOPuPd { NoPull = 0, PullUp = 1, PullDown = 2 };

enum class GPIOSpeed { Low = 0, Medium = 1, High = 2, VeryHigh = 3 };

enum class EXTITrigger {
  Rising = 0,
  Falling = 1,
  RisingFalling = 2
};

struct Pin {
  u8 bank : 4; // A-G
  u8 num : 4;  // 0-15

  Pin(char bankChar, uint8_t numVal);

  GPIO_TypeDef *gpio() const;
  void set_mode(GPIOMode mode) const;
  void set_otype(GPIOOType otype) const;
  void set_pupd(GPIOPuPd pupd) const;
  void set_speed(GPIOSpeed speed) const;
  void set_af(uint8_t af) const;

  void write(bool val) const;
  bool read() const;
  bool toggle() const;
  
  // EXTI interrupt configuration
  void enable_exti(EXTITrigger trigger) const;
  void disable_exti() const;
};
} // namespace stm
} // namespace hal
} // namespace ge
