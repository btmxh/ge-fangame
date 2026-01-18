#include "ge-hal/stm/uart.hpp"
#include "ge-hal/stm/rcc.hpp"
#include "ge-hal/stm/time.hpp"

namespace ge {
namespace hal {
namespace stm {

UARTHandle UARTConfigInfo::init(u32 baud_rate) const {
  u32 freq = get_rcc_freq(bus);
  get_rcc_reg(bus) |= 1UL << reg_bit_pos;

  tx.set_mode(GPIOMode::AlternateFunction);
  tx.set_af(7);
  rx.set_mode(GPIOMode::AlternateFunction);
  rx.set_af(7);

  uart->CR1 = 0;
  uart->BRR = freq / baud_rate;
  uart->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

  return UARTHandle{uart};
}

void UARTHandle::write(u8 byte) {
  uart->DR = byte;
  while ((uart->SR & USART_SR_TXE) == 0)
    delay_spin(1);
}
u8 UARTHandle::read() { return uart->DR & 0xFFUL; }

bool UARTHandle::is_written() { return true; }
bool UARTHandle::is_read_ready() { return (uart->SR & USART_SR_RXNE) != 0; }

} // namespace stm
} // namespace hal
} // namespace ge
