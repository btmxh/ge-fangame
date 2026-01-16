#include "ge-hal/app.hpp"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ge-hal/stm/sdram.hpp"
#include "ge-hal/stm/time.hpp"
#include "stm32f429xx.h"
#include <ge-hal/stm/uart.hpp>

ge::hal::stm::UARTHandle stdout_usart = nullptr;

namespace ge {

static void enable_fpu() {
  SCB->CPACR |=
      (3UL << 20) | (3UL << 22); // enable FPU: CP10 and CP11 full access
}

static void config_flash() {
  FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_PRFTEN |
                FLASH_ACR_LATENCY_5WS; // enable cache, set latency
}

void App::system_init() {
  enable_fpu();
  config_flash();
  hal::stm::setup_clock();
  stdout_usart = hal::stm::USART_CONFIG_DEBUG.init(115200);
  hal::stm::init_sdram();
}

App::App() {}

App::operator bool() { return true; }

void App::begin() {}
void App::end() {}

std::int64_t App::now() { return hal::stm::systick_get(); }

void App::log(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  std::vprintf(fmt, args);
  std::fflush(stdout);
  va_end(args);
}

void App::audio_bgm_play(const std::uint8_t *data, std::size_t len, bool loop) {
}

void App::audio_bgm_stop() {}

bool App::audio_bgm_is_playing() { return false; }

void App::audio_sfx_play(const std::uint8_t *data, std::size_t len,
                         std::size_t /*rate*/) {}

void App::audio_sfx_stop_all() {}

void App::audio_set_master_volume(std::uint8_t vol) {}

} // namespace ge
