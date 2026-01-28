#pragma once

#include "assets/out/textures/clouds.h"
#include "assets/out/textures/sun.h"
#include "ge-app/gfx/color.hpp"
#include "ge-app/texture.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/surface.hpp"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <utility>

namespace ge {
class Sky {
public:
  Sky() : sun_texture{sun, sun_WIDTH, sun_HEIGHT} {}

  void set_sky_color(std::uint16_t sky_color) {
    if (this->sky_color == sky_color)
      return;
    this->sky_color = sky_color;
  }

  void set_cloud_color(std::uint16_t cloud_color) {
    if (this->cloud_color == cloud_color)
      return;
    this->cloud_color = cloud_color;
  }

  void set_x_offset(int x_offset) {
    if (this->x_offset == x_offset)
      return;
    this->x_offset = x_offset;
  }

  static int max_x_offset() { return CLOUD_TEXTURE_WIDTH; }

  void render(Surface render_region) {
    const int W = render_region.get_width();
    const int H = render_region.get_height();
    const int TEX_W = CLOUD_TEXTURE_WIDTH;
    auto fb = render_region;

    // Fill sky
    hal::gpu::fill(render_region, sky_color);

    // temporary: recalculate cloud LUT every frame
    for (usize i = 0; i < sizeof(CLOUD_COLORS) / sizeof(CLOUD_COLORS[0]); ++i) {
      cloud_lut[i] = blend_rgb565(sky_color, cloud_color, CLOUD_COLORS[i]);
    }

    assert(H == 80);
    int stride = bg_clouds_len / H;

    for (int y = 0; y < H; ++y) {
      auto row_rle = &bg_clouds[CLOUD_ROW_OFFSETS[y]];

      int tex_x = 0;

      while (true) {
        u8 elem = *row_rle++;
        i32 len = elem >> 4;
        i32 value = elem & 0x0F;
        len = len < 0xF ? (len + 1) : *row_rle++;
        // i32 len = *row_rle++;
        // u8 value = *row_rle++;
        u16 color = cloud_lut[value];

        i32 run_start = tex_x, run_end = tex_x + len;
        tex_x = run_end;
        i32 sx = run_start - x_offset;
        if (sx < 0)
          sx += TEX_W;

        if (sx < W) {
          int span = std::min(len, W - sx);
          if (span > 0) {
            hal::gpu::fill(fb.subsurface(sx, y, span, 1), color);
          }
        }

        if (sx + len > TEX_W) {
          int span = (sx + len) - TEX_W;
          if (span > 0 && span < W) {
            hal::gpu::fill(fb.subsurface(0, y, span, 1), color);
          }
        }

        if (tex_x >= TEX_W)
          break;
      }
    }
  }

private:
  Texture sun_texture;
  u16 sky_color = hsv_to_rgb565(150, 200, 255),
      cloud_color = hsv_to_rgb565(0, 0, 255);
  u16 cloud_lut[sizeof(CLOUD_COLORS) / sizeof(CLOUD_COLORS[0])];
  int x_offset = 0;
};
} // namespace ge
