#include "ge-hal/app.hpp"

#include <bgm_loop.h>

#ifdef GE_HAL_STM32
#include "ge-hal/stm/dma2d.hpp"
#endif

using namespace ge;

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

    auto start = app.now();
    for (int y = 0; y < H; ++y) {
      u8 blue = 80 + (y * 120) / H;     // 80..200
      u8 green = 60 + (y * 100) / H;    // 60..160
      u16 color = ((green >> 2) << 5) | // G (6 bits)
                  (blue >> 3);          // B (5 bits)
      // TODO: abstract the DMA2D usage into ge-hal
#ifdef GE_HAL_STM32
      hal::stm::DMA2DDevice::fill({fb, W, H, hal::stm::PixelFormat::RGB565},
                                  {0, y, W, 1}, color);
#else
      for (int x = 0; x < W; ++x) {
        fb[y * W + x] = color;
      }
#endif
    }
#ifdef GE_HAL_STM32
    hal::stm::DMA2DDevice::fill({fb, W, H, hal::stm::PixelFormat::RGB565},
                                {static_cast<int>(x_pos), 0, 20, H}, 0xFFFF);
#else
    for (u32 row = 0; row < H; row++) {
      for (u32 col = x_pos; col < x_pos + 20; col++) {
        if (col >= 0 && col < W) {
          fb[row * W + col] = 0xFFFF;
        }
      }
    }
#endif
    auto end = app.now();

    x_pos += speed;
    if (x_pos > ge::App::WIDTH)
      x_pos = 0;
    app.end();

    app.log("Frame time (excluding vblank) %ld ms", end - start);
  }
  return 0;
}
