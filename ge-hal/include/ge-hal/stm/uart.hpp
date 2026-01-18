#pragma once

#include "ge-hal/core.hpp"

#include "ge-hal/stm/gpio.hpp"
#include "ge-hal/stm/rcc.hpp"

#include "stm32f429xx.h"

namespace ge {
namespace hal {
namespace stm {

struct UARTHandle {
  USART_TypeDef *uart;

  constexpr UARTHandle(USART_TypeDef *uart = nullptr) : uart(uart) {}

  void write(u8 byte);
  u8 read();

  bool is_written();
  bool is_read_ready();
};

struct UARTConfigInfo {
  RCCRegister bus;
  u8 reg_bit_pos;
  Pin tx, rx;
  USART_TypeDef *uart;

  UARTHandle init(u32 baud_rate) const;
};

static const UARTConfigInfo USART1_CONFIG = {
    .bus = RCCRegister::APB2,
    .reg_bit_pos = RCC_APB2ENR_USART1EN_Pos,
    .tx = Pin('A', 9),
    .rx = Pin('A', 10),
    .uart = USART1,
};

static const UARTConfigInfo USART_CONFIG_DEBUG = USART1_CONFIG;

} // namespace stm
} // namespace hal
} // namespace ge
