#pragma once

#include "ge-hal/core.hpp"

#include "ge-hal/stm/gpio.hpp"
#include "ge-hal/stm/rcc.hpp"
#include "stm32f429xx.h"

namespace ge {
namespace hal {
namespace stm {

struct SPI16bHandle {
  SPI_TypeDef *handle;

  SPI16bHandle(SPI_TypeDef *handle) : handle(handle) {}
  ~SPI16bHandle();

  bool is_busy();
  void send(u16 data);
  void send_blocking(u16 data);
};

struct SPIHandle {
  SPI_TypeDef *handle;

  SPIHandle(SPI_TypeDef *handle) : handle(handle) {}

  bool is_busy();
  void send(u8 data);
  void send_blocking(u8 data);

  // NOTE: do not use the original SPIHandle while the SPI16bHandle is active
  // RAII will automatically switch back to 8-bit mode when SPI16bHandle is
  // destroyed
  SPI16bHandle switch_to_16bit();
};

struct SPIConfigInfo {
  RCCRegister bus;
  u8 reg_bit_pos;
  Pin sck, miso, mosi;
  SPI_TypeDef *handle;

  SPIHandle init() const;
};

static const SPIConfigInfo SPI5_CONFIG = {
    .bus = RCCRegister::APB2,
    .reg_bit_pos = RCC_APB2ENR_SPI5EN_Pos,
    .sck = Pin('F', 7),
    .miso = Pin('F', 8),
    .mosi = Pin('F', 9),
    .handle = SPI5,
};

} // namespace stm
} // namespace hal
} // namespace ge
