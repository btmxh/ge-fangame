#include "ge-hal/app.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <bgm_loop.h>

int main() {
  ge::App app;
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
  return 0;
}
