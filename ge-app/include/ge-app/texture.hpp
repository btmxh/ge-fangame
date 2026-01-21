#pragma once

#include "ge-hal/gpu.hpp"
#include "ge-hal/surface.hpp"
#include <algorithm>
#include <cmath>

namespace ge {
class Texture : public ConstSurface {
public:
  Texture(const u16 *color_data, u32 width, u32 height,
          PixelFormat format = PixelFormat::ARGB1555)
      : ConstSurface{color_data, width, width, height, format} {}

  void blit(Surface &region) { hal::gpu::blit_blend(region, *this, 0xFF); }

  template <class Region>
  void blit_rotated(
      Region &region, int dst_cx,
      int dst_cy,     // destination center (screen coords)
      float angle_rad // clockwise or CCW, your choice, just be consistent
  ) {
    const int W = region.get_width();
    const int H = region.get_height();

    // Precompute sin/cos
    float c = std::cos(angle_rad);
    float s = std::sin(angle_rad);

    // Sprite center in source space
    float src_cx = (get_width() - 1) * 0.5f;
    float src_cy = (get_height() - 1) * 0.5f;

    // Compute conservative bounding box radius
    float hw = get_width() * 0.5f;
    float hh = get_height() * 0.5f;
    float r = std::sqrt(hw * hw + hh * hh);

    int x0 = std::max(0, (int)std::floor(dst_cx - r));
    int x1 = std::min(W - 1, (int)std::ceil(dst_cx + r));
    int y0 = std::max(0, (int)std::floor(dst_cy - r));
    int y1 = std::min(H - 1, (int)std::ceil(dst_cy + r));

    // For each destination pixel, map back into source via inverse rotation
    // Inverse rotation matrix for [c -s; s c] is [c s; -s c]
    for (int y = y0; y <= y1; ++y) {
      float dy = (float)y - (float)dst_cy;
      for (int x = x0; x <= x1; ++x) {
        float dx = (float)x - (float)dst_cx;

        float u = c * dx + s * dy + src_cx;
        float v = -s * dx + c * dy + src_cy;

        int sx = (int)std::lround(u);
        int sy = (int)std::lround(v);

        if ((unsigned)sx < (unsigned)get_width() &&
            (unsigned)sy < (unsigned)get_height()) {
          auto col = *static_cast<const u16 *>(pixel_at(sx, sy));
          if (col & 0x8000) {
            region.set_pixel(x, y, col);
          }
        }
      }
    }
  }
};
} // namespace ge
