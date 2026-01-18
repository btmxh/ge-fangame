#pragma once

#include "ge-app/gfx/color.hpp"
#include "ge-app/texture.hpp"
#include "ge-hal/fb.hpp"
#include <bg_clouds.h>
#include <cassert>
#include <cstdint>
#include <sun.h>

namespace ge {
class Sky {
public:
  Sky() : sun_texture{sun_color, sun_alpha, sun_width, sun_height} {}
  void invalidate() { needs_rerender = true; }

  void set_sky_color(std::uint16_t sky_color) {
    if (this->sky_color == sky_color)
      return;
    this->sky_color = sky_color;
    invalidate();
  }

  void set_cloud_color(std::uint16_t cloud_color) {
    if (this->cloud_color == cloud_color)
      return;
    this->cloud_color = cloud_color;
    invalidate();
  }

  void set_x_offset(int x_offset) {
    if (this->x_offset == x_offset)
      return;
    this->x_offset = x_offset;
    invalidate();
  }

  static int max_x_offset() { return CLOUD_TEXTURE_WIDTH; }

  void render(FramebufferRegion render_region) {
    if (!needs_rerender)
      return;

    needs_rerender = false;

    const int W = render_region.region_width();
    const int H = render_region.region_height();
    const int TEX_W = CLOUD_TEXTURE_WIDTH;
    auto fb = render_region;

    // Fill sky
    for (int i = 0; i < W * H; ++i)
      fb.set_pixel(i % W, i / W, sky_color);

    assert(H == 80);
    int stride = clouds_len / H;

    for (int y = 0; y < H; ++y) {
      auto row_rle = &clouds[y * stride];

      int tex_x = 0;

      while (true) {
        uint8_t count = *row_rle++;
        uint8_t value = CLOUD_COLORS[*row_rle++];

        for (int i = 0; i < count; ++i, ++tex_x) {
          // Is this texture x inside the visible window?
          int dx = tex_x - x_offset;
          if (dx < 0)
            dx += TEX_W;

          if (dx < W) {
            std::uint16_t blended = blend_rgb565(sky_color, cloud_color, value);
            fb.set_pixel(dx, y, blended);
          }
        }

        if (tex_x >= TEX_W)
          break;
      }
    }
  }

private:
  Texture sun_texture;
  std::uint16_t sky_color = 0xFFFF, cloud_color = 0xFFFF;
  int x_offset = 0;
  bool needs_rerender = true;
};
} // namespace ge
