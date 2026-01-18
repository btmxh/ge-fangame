#pragma once

#include "ge-hal/core.hpp"
#include <cstdint>

namespace ge {
class Surface {
public:
  Surface(std::uint16_t *fb_ptr, int stride, int width, int height,
          int buffer_index = BUFFER_INDEX_NONE)
      : fb_ptr(fb_ptr), stride(stride), width(width), height(height),
        buffer_index{buffer_index} {}

  Surface subsurface(int x, int y, int w, int h) const {
    if (x < 0)
      x = 0;
    if (y < 0)
      y = 0;
    if (x + w > width)
      w = width - x;
    if (y + h > height)
      h = height - y;
    return Surface(&fb_ptr[y * stride + x], stride, w, h, buffer_index);
  }

  int get_width() const { return width; }
  int get_height() const { return height; }
  int get_buffer_index() const { return buffer_index; }

  void set_pixel(int x, int y, std::uint16_t color) {
    if ((unsigned)x >= (unsigned)width || (unsigned)y >= (unsigned)height)
      return;
    fb_ptr[y * stride + x] = color;
  }

  u16 *data() { return fb_ptr; }

private:
  static constexpr u32 BUFFER_INDEX_NONE = -1;
  std::uint16_t *fb_ptr;
  int stride, width, height, buffer_index;
};
} // namespace ge
