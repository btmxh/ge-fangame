
#pragma once

#include "ge-hal/core.hpp"
#include "ge-hal/stm/time.hpp"
#include "stm32f429xx.h"
#include <cstddef>

namespace ge {
namespace hal {
namespace stm {

enum class RCCRegister : u8 {
  APB1 = offsetof(RCC_TypeDef, APB1ENR),
  APB2 = offsetof(RCC_TypeDef, APB2ENR),
};

inline volatile u32 &get_rcc_reg(RCCRegister reg) {
  return *reinterpret_cast<volatile u32 *>(
      reinterpret_cast<volatile u8 *>(RCC) + static_cast<u8>(reg));
}

inline u32 get_rcc_freq(RCCRegister reg) {
  switch (reg) {
  case RCCRegister::APB1:
    return APB1_FREQUENCY;
  case RCCRegister::APB2:
    return APB2_FREQUENCY;
  default:
    return 0;
  }
}

} // namespace stm
} // namespace hal
} // namespace ge
