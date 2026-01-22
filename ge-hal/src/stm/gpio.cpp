#include "ge-hal/stm/gpio.hpp"

namespace ge {
namespace hal {
namespace stm {
Pin::Pin(char bankChar, uint8_t numVal) : bank(bankChar - 'A'), num(numVal) {}

GPIO_TypeDef *Pin::gpio() const {
  return reinterpret_cast<GPIO_TypeDef *>(GPIOA_BASE + bank * 0x400);
}

void Pin::set_mode(GPIOMode mode) const {
  auto reg = gpio();
  RCC->AHB1ENR |= (1U << bank);     // Enable GPIO clock for this bank
  reg->MODER &= ~(3U << (num * 2)); // Clear existing setting
  reg->MODER |= (static_cast<uint32_t>(mode) & 3U) << (num * 2); // Set new mode
}

void Pin::set_otype(GPIOOType otype) const {
  auto reg = gpio();
  reg->OTYPER &= ~(1U << num); // Clear existing setting
  reg->OTYPER |= (static_cast<uint32_t>(otype) & 1U) << num; // Set new otype
}

void Pin::set_pupd(GPIOPuPd pupd) const {
  auto reg = gpio();
  reg->PUPDR &= ~(3U << (num * 2)); // Clear existing setting
  reg->PUPDR |= (static_cast<uint32_t>(pupd) & 3U) << (num * 2); // Set new pupd
}

void Pin::set_speed(GPIOSpeed speed) const {
  auto reg = gpio();
  reg->OSPEEDR &= ~(3U << (num * 2)); // Clear existing setting
  reg->OSPEEDR |= (static_cast<uint32_t>(speed) & 3U)
                  << (num * 2); // Set new speed
}

void Pin::set_af(uint8_t af) const {
  auto reg = gpio();
  u32 shift = (num & 0x7UL) * 4;
  reg->AFR[num >> 3] &= ~(0xFUL << shift);     // Clear existing setting
  reg->AFR[num >> 3] |= (af & 0xFUL) << shift; // Set new AF
}

void Pin::write(bool val) const {
  auto reg = gpio();
  reg->BSRR = (1UL << num) << (val ? 0 : 16);
}

bool Pin::read() const {
  auto reg = gpio();
  return (reg->IDR & (1UL << num)) != 0;
}

bool Pin::toggle() const {
  bool current = read();
  write(!current);
  return !current;
}

void Pin::enable_exti(EXTITrigger trigger) const {
  // Enable SYSCFG clock for EXTI configuration
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  
  // Configure EXTI line to this GPIO bank
  u32 exti_idx = num / 4;
  u32 exti_shift = (num % 4) * 4;
  SYSCFG->EXTICR[exti_idx] &= ~(0xFU << exti_shift);
  SYSCFG->EXTICR[exti_idx] |= (bank << exti_shift);
  
  // Configure trigger
  u32 mask = (1U << num);
  EXTI->IMR |= mask;  // Unmask interrupt
  
  if (trigger == EXTITrigger::Rising || trigger == EXTITrigger::RisingFalling) {
    EXTI->RTSR |= mask;  // Enable rising edge
  } else {
    EXTI->RTSR &= ~mask;
  }
  
  if (trigger == EXTITrigger::Falling || trigger == EXTITrigger::RisingFalling) {
    EXTI->FTSR |= mask;  // Enable falling edge
  } else {
    EXTI->FTSR &= ~mask;
  }
  
  // Enable NVIC interrupt for this EXTI line
  IRQn_Type irqn;
  if (num <= 4) {
    irqn = static_cast<IRQn_Type>(EXTI0_IRQn + num);
  } else if (num <= 9) {
    irqn = EXTI9_5_IRQn;
  } else {
    irqn = EXTI15_10_IRQn;
  }
  NVIC_EnableIRQ(irqn);
}

void Pin::disable_exti() const {
  u32 mask = (1U << num);
  EXTI->IMR &= ~mask;  // Mask interrupt
  EXTI->RTSR &= ~mask;
  EXTI->FTSR &= ~mask;
}
} // namespace stm
} // namespace hal
} // namespace ge
