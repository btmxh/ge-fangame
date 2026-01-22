#include "ge-hal/app.hpp"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ge-hal/stm/dma2d.hpp"
#include "ge-hal/stm/framebuffer.hpp"
#include "ge-hal/stm/gpio.hpp"
#include "ge-hal/stm/sdram.hpp"
#include "ge-hal/stm/time.hpp"
#include "stm32f429xx.h"
#include <ge-hal/stm/uart.hpp>

ge::hal::stm::UARTHandle stdout_usart = nullptr;

namespace ge {

namespace {
// Button GPIO pins (to be configured based on actual hardware)
constexpr hal::stm::Pin BUTTON1_PIN{'A', 0};
constexpr hal::stm::Pin BUTTON2_PIN{'A', 1};
constexpr int NUM_BUTTONS = 2;

// Button state tracking for event detection
constexpr i64 BUTTON_HOLD_THRESHOLD_MS = 1000;
struct ButtonState {
  bool last_state = false; // false = not pressed, true = pressed
  i64 last_down = -1;
  i64 last_up = -1;
  bool handled_hold = false;
} button_states[NUM_BUTTONS];

constexpr hal::stm::Pin button_pins[NUM_BUTTONS] = {BUTTON1_PIN, BUTTON2_PIN};
} // anonymous namespace

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
  hal::stm::init_dma2d();

  // Initialize button GPIO pins as inputs with pull-up resistors
  for (const auto& pin : button_pins) {
    pin.set_mode(hal::stm::GPIOMode::Input);
    pin.set_pupd(hal::stm::GPIOPuPd::PullUp);
  }
}

App::~App() = default;

App::operator bool() { return true; }

static u32 buffer_index = 0;
static bool needs_rerender = true;

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

void App::tick(float dt) {
  // Read button states and trigger callbacks
  for (int i = 0; i < NUM_BUTTONS; ++i) {
    // Buttons are active-low (pull-up resistors, pressed = LOW)
    bool pressed = !button_pins[i].read();
    auto &bs = button_states[i];
    
    // Detect button down event
    if (pressed && !bs.last_state) {
      bs.last_down = now();
      bs.last_up = -1;
      bs.handled_hold = false;
      needs_rerender = true; // Button state changed, need to rerender
    }
    
    // Detect button up event
    if (!pressed && bs.last_state) {
      bs.last_up = now();
      if (bs.last_down >= 0) {
        i64 held_time = bs.last_up - bs.last_down;
        if (held_time < BUTTON_HOLD_THRESHOLD_MS) {
          on_button_clicked(static_cast<Button>(i));
        } else {
          on_button_finished_hold(static_cast<Button>(i));
        }
      }
      bs.handled_hold = false;
      needs_rerender = true; // Button state changed, need to rerender
    }
    
    // Check for hold event
    if (pressed && bs.last_down >= 0 && !bs.handled_hold) {
      i64 held_time = now() - bs.last_down;
      if (held_time >= BUTTON_HOLD_THRESHOLD_MS) {
        on_button_held(static_cast<Button>(i));
        bs.handled_hold = true;
        needs_rerender = true; // Button state changed, need to rerender
      }
    }
    
    bs.last_state = pressed;
  }
}

void App::loop() {
  i64 last_tick = now();
  while (*this) {
    i64 current = now();
    float dt = (current - last_tick) * 1e-3f;
    tick(dt);
    last_tick = current;
    
    // Master-loop architecture: only render when needed
    if (needs_rerender) {
      auto buffer = hal::stm::pixel_buffer(buffer_index);
      Surface fb_region{buffer,      App::WIDTH,          App::WIDTH,
                        App::HEIGHT, PixelFormat::RGB565, buffer_index};
      render(fb_region);
      hal::stm::swap_buffers(buffer_index);
      needs_rerender = false;
    }
    
    // TODO: Process audio when needed
    // if (needs_audio_processing) process_audio();
    
    // Wait for interrupt to save power
    __WFI();
  }
}

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
