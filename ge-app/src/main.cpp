#include "ge-hal/app.hpp"
#include "ge-hal/core.hpp"

#include <array>
#include <bgm_loop.h>

#ifdef GE_HAL_STM32
#include "ge-hal/stm/gpio.hpp"
#include "ge-hal/stm/time.hpp"
#endif

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  ge::App app{};

#ifdef GE_HAL_STM32
  using namespace ge::hal::stm;

  std::array<Pin, 2> leds = {Pin('G', 13), Pin('G', 14)};
  for (auto led : leds)
    led.set_mode(GPIOMode::Output);

  while (true) {
    for (auto led : leds) {
      led.toggle();
      delay_timed(1000);
    }

    float x = app.now() * 2.0f;
    app.log("Hello, World! %d\r\n", (int)x);
  }
#else
  app.audio_bgm_play(bgm_loop, bgm_loop_len, true);
  while (app) {
    app.begin();

    constexpr int W = ge::App::WIDTH;
    constexpr int H = ge::App::HEIGHT;
    uint16_t *fb = app.framebuffer;

    for (int y = 0; y < H; ++y) {
      // y/H -> 0..1
      uint8_t blue = 80 + (y * 120) / H;  // 80..200
      uint8_t green = 60 + (y * 100) / H; // 60..160

      uint16_t color = ((green >> 2) << 5) | // G (6 bits)
                       (blue >> 3);          // B (5 bits)

      for (int x = 0; x < W; ++x) {
        fb[y * W + x] = color;
      }
    }
    app.end();

    // app.log("Frame rendered at time %lld ms", app.now());
  }
#endif
  return 0;
}
