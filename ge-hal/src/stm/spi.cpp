#include "ge-hal/stm/spi.hpp"

#include "ge-hal/stm/time.hpp"
#include "stm32f429xx.h"
#include <initializer_list>

namespace ge {
namespace hal {
namespace stm {

SPIHandle SPIConfigInfo::init() const {
  for (Pin pin : {sck, miso, mosi}) {
    pin.set_mode(GPIOMode::AlternateFunction);
    pin.set_otype(GPIOOType::PushPull);
    pin.set_pupd(GPIOPuPd::NoPull);
    pin.set_speed(GPIOSpeed::VeryHigh);
    pin.set_af(5);
  }

  get_rcc_reg(bus) |= 1UL << reg_bit_pos;

  handle->CR1 = 0;
  handle->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;
  handle->CR1 |= SPI_CR1_SPE;

  return handle;
}

bool SPIHandle::is_busy() {
  // (((SPIx)->SR & (SPI_SR_TXE | SPI_SR_RXNE)) == 0 || ((SPIx)->SR &
  // SPI_SR_BSY))
  return (handle->SR & (SPI_SR_TXE | SPI_SR_RXNE)) == 0 ||
         (handle->SR & SPI_SR_BSY);
}

void SPIHandle::send(u8 data) { handle->DR = data; }

void SPIHandle::send_blocking(u8 data) {
  while (!((handle->SR) & SPI_SR_TXE))
    delay_spin(1);
  send(data);
}

SPI16bHandle SPIHandle::switch_to_16bit() {
  while (is_busy())
    delay_spin(1);
  handle->CR1 &= ~SPI_CR1_SPE;
  handle->CR1 |= SPI_CR1_DFF;
  handle->CR1 |= SPI_CR1_SPE;
  return SPI16bHandle(handle);
}

SPI16bHandle::~SPI16bHandle() {
  while (is_busy())
    delay_spin(1);
  handle->CR1 &= ~SPI_CR1_SPE;
  handle->CR1 &= ~SPI_CR1_DFF;
  handle->CR1 |= SPI_CR1_SPE;
}

bool SPI16bHandle::is_busy() { return SPIHandle{handle}.is_busy(); }

void SPI16bHandle::send(u16 data) { handle->DR = data; }
void SPI16bHandle::send_blocking(u16 data) {
  while (!((handle->SR) & SPI_SR_TXE))
    delay_spin(1);
  send(data);
}

} // namespace stm
} // namespace hal
} // namespace ge
