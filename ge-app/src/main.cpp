#include "ge-hal/app.hpp"
#include "ge-hal/core.hpp"

#include <array>
#include <bgm_loop.h>
#include <cstdio>

#ifdef GE_HAL_STM32
#include "ge-hal/stm/gpio.hpp"
#include "ge-hal/stm/time.hpp"
#endif

// AI-generated test for SDRAM functionality
#define SDRAM_BANK_ADDR ((uint32_t)0xD0000000) // FMC Bank 6
#define SDRAM_SIZE ((uint32_t)0x800000)        // 8 MBytes (Adjust to your chip)
bool test_sdram() {
  volatile uint32_t *p_sdram = (uint32_t *)SDRAM_BANK_ADDR;
  uint32_t i = 0;
  bool status = true;

  // 1. Write pattern (Address as data is a good check for address line shorts)
  for (i = 0; i < (SDRAM_SIZE / 4); i++) {
    p_sdram[i] = (uint32_t)(SDRAM_BANK_ADDR + (i * 4));
  }

  // 2. Read back and verify
  for (i = 0; i < (SDRAM_SIZE / 4); i++) {
    if (p_sdram[i] != (uint32_t)(SDRAM_BANK_ADDR + (i * 4))) {
      status = false;
      // Optional: Breakpoint here to see exactly which address failed
      break;
    }
  }

  return status;
}

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

    char *sdram = reinterpret_cast<char *>(0xD0000000);
    snprintf(sdram, 64, "Uptime: %d ms\r\n", (int)app.now());
    app.log("%s", sdram);

    bool result = test_sdram();
    app.log("SDRAM test: %s\r\n", result ? "PASS" : "FAIL");
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
