#pragma once
#include "ge-hal/core.hpp"
#include "stm32f429xx.h"

namespace ge {
namespace hal {
namespace stm {

// Standard STM32 DMA2D Formats
enum class PixelFormat : u32 {
  ARGB8888 = 0,
  RGB888 = 1,
  RGB565 = 2, // Standard for ILI9341
  ARGB1555 = 3,
  ARGB4444 = 4,
  L8 = 5, // 8-bit Indexed (CLUT)
  AL44 = 6,
  AL88 = 7,
  L4 = 8,
  A8 = 9,
  A4 = 10
};

struct Surface {
  void *pixels; // Pointer to buffer (u16* or u8* depending on fmt)
  int width;    // Total width of the image buffer (stride)
  int height;   // Total height
  PixelFormat fmt;
};

struct Rect {
  int x, y, w, h;
};

struct Palette {
  const u32 *colors; // Array of ARGB8888 colors
  int count;         // Typically 16 or 256
};

struct DMA2DDevice {
  // Enable the hardware clock
  static void init() { RCC->AHB1ENR |= RCC_AHB1ENR_DMA2DEN; }

  // 1. CLEAR / FILL (Solid Color)
  //    Uses R2M mode. Extremely fast.
  static void fill(Surface dst, Rect rect, u32 color);

  // 2. BLIT (Copy Texture)
  //    Automatically handles Format Conversion (e.g. RGB888 -> RGB565).
  //    Uses M2M or M2M_PFC.
  static void blit(Surface dst, Surface src, int x, int y);

  // 3. BLIT PART (Sprite Sheet)
  //    Copies only a sub-rectangle from the source to the dest.
  static void blit_ex(Surface dst, Surface src, Rect src_rect, int x, int y);

  // 4. BLIT BLEND (Alpha Transparency)
  //    Mixes source alpha with destination background.
  //    'global_alpha': 0-255 (255 = Opaque). Multiplied with pixel alpha.
  static void blit_blend(Surface dst, Surface src, int x, int y,
                         u8 global_alpha = 255);

  // 5. CLUT (Indexed Color)
  //    Loads a palette into hardware and draws an L8/L4 image.
  static void load_palette(Palette pal);
  static void blit_indexed(Surface dst, Surface src, int x, int y);

private:
  // The "Pipelined" Wait
  // Returns immediately if hardware is idle.
  // Blocks only if previous job is still running.
  static inline void wait_idle() {
    while (DMA2D->CR & DMA2D_CR_START)
      __NOP();
  }
};

} // namespace stm
} // namespace hal
} // namespace ge
