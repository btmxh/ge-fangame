#include "ge-hal/app.hpp"

#include <bgm_loop.h>

using namespace ge;

void draw_bar(u16 *buffer, int x, int width, u16 color) {
  for (int row = 0; row < ge::App::HEIGHT; row++) {
    for (int col = x; col < x + width; col++) {
      if (col >= 0 && col < ge::App::WIDTH) {
        buffer[row * ge::App::WIDTH + col] = color;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  ge::App app{};

  // this ifndef is not really necessary, but this improves loading time as
  // audio has yet to be implemented on STM32
#ifndef GE_HAL_STM32
  app.audio_bgm_play(bgm_loop, bgm_loop_len, true);
#endif
  ge::u32 x_pos = 0, speed = 8;
  while (app) {
    constexpr int W = ge::App::WIDTH;
    constexpr int H = ge::App::HEIGHT;
    auto fb = app.begin();

    for (int y = 0; y < H; ++y) {
      u8 blue = 80 + (y * 120) / H;     // 80..200
      u8 green = 60 + (y * 100) / H;    // 60..160
      u16 color = ((green >> 2) << 5) | // G (6 bits)
                  (blue >> 3);          // B (5 bits)

      for (int x = 0; x < W; ++x) {
        fb[y * W + x] = color;
      }
    }
    draw_bar(fb, x_pos, 20, 0xFFFF);
    x_pos += speed;
    if (x_pos > ge::App::WIDTH)
      x_pos = 0;
    app.end();

    app.log("Frame rendered at time %ld ms", app.now());
  }
  return 0;
}
