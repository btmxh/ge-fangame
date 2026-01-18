#include "ge-hal/app.hpp"
#include "ge-hal/core.hpp"

#include <array>
#include <bgm_loop.h>
#include <cstdio>

#ifdef GE_HAL_STM32
#include "ge-hal/stm/framebuffer.hpp"
#include "ge-hal/stm/gpio.hpp"
#include "ge-hal/stm/time.hpp"
#endif

using namespace ge;

// Helper: Simple rectangle drawer
void draw_bar(u16 *buffer, int x, int width, u16 color) {
  for (int row = 0; row < ge::App::HEIGHT; row++) {
    for (int col = x; col < x + width; col++) {
      if (col >= 0 && col < ge::App::WIDTH) {
        buffer[row * ge::App::WIDTH + col] = color;
      }
    }
  }
}

// Helper: Simple Clear Screen (Slow CPU fill, use DMA2D later!)
void clear_screen(u16 *buffer, u16 color) {
  for (int i = 0; i < ge::App::WIDTH * ge::App::HEIGHT; i++) {
    buffer[i] = color;
  }
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  ge::App app{};

#ifdef GE_HAL_STM32
  using namespace ge::hal::stm;

  ge::u32 x_pos = 0, speed = 8;
  int buffer_index = 0;
  while (true) {
    auto start = app.now();
    auto buffer = pixel_buffer(buffer_index);

    clear_screen(buffer, 0x0000);
    draw_bar(buffer, x_pos, 20, 0xFFFF);
    x_pos += speed;
    if (x_pos > ge::App::WIDTH)
      x_pos = 0;

    swap_buffers(buffer_index);

    auto end = app.now();
    app.log("VBlank wait time: %lu ms\r\n", end - start);
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
