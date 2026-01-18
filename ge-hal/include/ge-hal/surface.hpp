#pragma once

#include "ge-hal/core.hpp"
#include <cassert>

namespace ge {

// DMA2D values
enum class PixelFormat : u8 {
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

inline usize pixel_format_bpp(PixelFormat fmt) {
  switch (fmt) {
  case PixelFormat::ARGB8888:
    return 32;
  case PixelFormat::RGB888:
    return 24;
  case PixelFormat::RGB565:
  case PixelFormat::ARGB1555:
  case PixelFormat::ARGB4444:
  case PixelFormat::AL88:
    return 16;
  case PixelFormat::L8:
  case PixelFormat::AL44:
  case PixelFormat::A8:
    return 8;
  case PixelFormat::L4:
  case PixelFormat::A4:
    return 4;
  default:
    return 0;
  }
}

class Surface {
public:
  Surface(void *fb_ptr, int stride, int width, int height,
          u32 buffer_index = BUFFER_INDEX_NONE)
      : fb_ptr(fb_ptr), stride(stride), width(width), height(height),
        buffer_index{buffer_index} {}

  Surface subsurface(u32 x, u32 y, u32 w, u32 h) const {
    if (x + w > width)
      w = width - x;
    if (y + h > height)
      h = height - y;
    return Surface(pixel_at(x, y), stride, w, h, buffer_index);
  }

  u32 get_width() const { return width; }
  u32 get_height() const { return height; }
  u32 get_stride() const { return stride; }
  u32 get_buffer_index() const { return buffer_index; }
  PixelFormat get_pixel_format() const { return fmt; }

  void *pixel_at(u32 x, u32 y) const {
    if (x >= width || y >= height)
      return nullptr;
    u8 *ptr = static_cast<u8 *>(fb_ptr);
    return &ptr[(y * stride + x) * pixel_format_bpp(fmt) / 8];
  }

  template <class ColorT> void set_pixel(int x, int y, ColorT color) {
    auto ptr = pixel_at(x, y);
    if (pixel_format_bpp(fmt) >= 8) {
      assert(sizeof(ColorT) * 8 == pixel_format_bpp(fmt));
      *static_cast<ColorT *>(ptr) = color;
    } else {
      // 4bpp formats
      u8 &byte = *static_cast<u8 *>(ptr);
      if ((y * stride + x) & 1) {
        // High nibble
        byte = (byte & 0x0F) | ((color & 0x0F) << 4);
      } else {
        // Low nibble
        byte = (byte & 0xF0) | (color & 0x0F);
      }
    }
  }

  template <class ColorT = void> ColorT *data() const {
    return static_cast<ColorT *>(fb_ptr);
  }

private:
  static constexpr u32 BUFFER_INDEX_NONE = -1;
  void *fb_ptr;
  u32 stride, width, height, buffer_index;
  PixelFormat fmt = PixelFormat::RGB565;
};
} // namespace ge
