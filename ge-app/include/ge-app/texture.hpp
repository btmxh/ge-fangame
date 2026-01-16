#pragma once

#include "ge-hal/fb.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace ge {
class Texture {
public:
  Texture(const std::uint16_t *color_data, const std::uint8_t *alpha_data,
          int width, int height)
      : color_data(color_data), alpha_data(alpha_data), width(width),
        height(height) {}

  void blit(FramebufferRegion &region) {
    int w = std::min(width, region.region_width());
    int h = std::min(height, region.region_height());

    for (int y = 0; y < h; ++y) {
      for (int x = 0; x < w; ++x) {
        int bit_index = y * width + x;
        int byte_index = bit_index / 8;
        int bit_in_byte = 7 - (bit_index % 8);
        bool opaque = (alpha_data[byte_index] >> bit_in_byte) & 1;
        if (opaque) {
          std::uint16_t color = color_data[bit_index];
          region.set_pixel(x, y, color);
        }
      }
    }
  }

  void blit_scaled(FramebufferRegion &region, int dst_x0, int dst_y0,
                   float scale_x, float scale_y) {
    int dst_w = region.region_width();
    int dst_h = region.region_height();

    int src_w = width;
    int src_h = height;

    for (int dy = 0; dy < dst_h; ++dy) {
      float sy_f = (dy - dst_y0) / scale_y;
      int sy = (int)sy_f;
      if (sy < 0 || sy >= src_h)
        continue;

      for (int dx = 0; dx < dst_w; ++dx) {
        float sx_f = (dx - dst_x0) / scale_x;
        int sx = (int)sx_f;
        if (sx < 0 || sx >= src_w)
          continue;

        int bit_index = sy * src_w + sx;
        int byte_index = bit_index >> 3;
        int bit_in_byte = 7 - (bit_index & 7);

        if ((alpha_data[byte_index] >> bit_in_byte) & 1) {
          region.set_pixel(dx, dy, color_data[bit_index]);
        }
      }
    }
  }

  template <class Region>
  void blit_rotated(
      Region &region, int dst_cx,
      int dst_cy,     // destination center (screen coords)
      float angle_rad // clockwise or CCW, your choice, just be consistent
  ) {
    const int W = region.region_width();
    const int H = region.region_height();

    // Precompute sin/cos
    float c = std::cos(angle_rad);
    float s = std::sin(angle_rad);

    // Sprite center in source space
    float src_cx = (width - 1) * 0.5f;
    float src_cy = (height - 1) * 0.5f;

    // Compute conservative bounding box radius
    float hw = width * 0.5f;
    float hh = height * 0.5f;
    float r = std::sqrt(hw * hw + hh * hh);

    int x0 = std::max(0, (int)std::floor(dst_cx - r));
    int x1 = std::min(W - 1, (int)std::ceil(dst_cx + r));
    int y0 = std::max(0, (int)std::floor(dst_cy - r));
    int y1 = std::min(H - 1, (int)std::ceil(dst_cy + r));

    auto a1_opaque = [&](int x, int y) {
      int bit_index = y * width + x;
      int byte_index = bit_index >> 3;
      int bit_in_byte = 7 - (bit_index & 7);
      return (alpha_data[byte_index] >> bit_in_byte) & 1;
    };

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

        if ((unsigned)sx < (unsigned)width && (unsigned)sy < (unsigned)height) {
          if (a1_opaque(sx, sy)) {
            uint16_t col = color_data[sy * width + sx];
            region.set_pixel(x, y, col);
          }
        }
      }
    }
  }

private:
  int width;
  int height;
  // rgb565 color data, one uint16_t per pixel
  const std::uint16_t *color_data;
  // bitmask alpha (1 = opaque, 0 = transparent), packed 8 pixels per byte
  const std::uint8_t *alpha_data;
};
} // namespace ge
