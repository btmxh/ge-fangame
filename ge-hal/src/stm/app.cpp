#include "ge-hal/app.hpp"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ge-hal/stm/dma2d.hpp"
#include "ge-hal/stm/framebuffer.hpp"
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
}

App::App() {
  stdout_usart = hal::stm::USART_CONFIG_DEBUG.init(115200);
  hal::stm::init_sdram();
  hal::stm::init_ltdc();
  hal::stm::DMA2DDevice::init();
}

App::~App() = default;

App::operator bool() { return true; }

static int buffer_index = 0;

Surface App::begin() {
  auto buffer = hal::stm::pixel_buffer(buffer_index);
  return Surface{buffer, App::WIDTH, App::WIDTH, App::HEIGHT,
                           buffer_index};
}

void App::end() { hal::stm::swap_buffers(buffer_index); }

std::int64_t App::now() { return hal::stm::systick_get(); }

JoystickState App::get_joystick_state() {
  // TODO: implement joystick reading
  return {0.0f, 0.0f};
}

void App::log(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  std::vprintf(fmt, args);
  std::fputs("\r\n", stdout);
  std::fflush(stdout);
  va_end(args);
}

void App::sleep(std::int64_t ms) { hal::stm::delay_timed(ms); }

void App::audio_bgm_play(const std::uint8_t *data, std::size_t len, bool loop) {
  (void)data;
  (void)len;
  (void)loop;
}

void App::audio_bgm_stop() {}

bool App::audio_bgm_is_playing() { return false; }

void App::audio_sfx_play(const std::uint8_t *data, std::size_t len,
                         std::size_t rate) {
  (void)data;
  (void)len;
  (void)rate;
}

void App::audio_sfx_stop_all() {}

void App::audio_set_master_volume(std::uint8_t vol) { (void)vol; }

} // namespace ge
