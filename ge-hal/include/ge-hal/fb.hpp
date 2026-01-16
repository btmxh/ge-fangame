#pragma once

#include <cstdint>

namespace ge {
class FramebufferRegion {
public:
  FramebufferRegion(std::uint16_t *fb_ptr, int stride, int width, int height)
      : fb_ptr(fb_ptr), stride(stride), width(width), height(height) {}

  FramebufferRegion subregion(int x, int y, int w, int h) const {
    if (x < 0)
      x = 0;
    if (y < 0)
      y = 0;
    if (x + w > width)
      w = width - x;
    if (y + h > height)
      h = height - y;
    return FramebufferRegion(&fb_ptr[y * stride + x], stride, w, h);
  }

  int region_width() const { return width; }
  int region_height() const { return height; }

  void set_pixel(int x, int y, std::uint16_t color) {
    if ((unsigned)x >= (unsigned)width || (unsigned)y >= (unsigned)height)
      return;
    fb_ptr[y * stride + x] = color;
  }

private:
  std::uint16_t *fb_ptr;
  int stride, width, height;
};
} // namespace ge
