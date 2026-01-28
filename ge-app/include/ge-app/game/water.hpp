#pragma once

#include "ge-app/gfx/color.hpp"
#include "ge-app/texture.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/gpu.hpp"
#include "ge-hal/surface.hpp"
#include <algorithm>

#include "assets/out/textures/watertexture.h"

namespace std {
template <class T>
constexpr const T &clamp(const T &v, const T &lo, const T &hi) {
  return clamp(v, lo, hi);
}

template <class T, class Compare>
constexpr const T &clamp(const T &v, const T &lo, const T &hi, Compare comp) {
  return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}
} // namespace std

namespace ge {
class Water {
public:
  void set_water_color(std::uint16_t color) { water_color = color; }
  void set_sky_color(std::uint16_t color) { sky_color = color; }

  void render(Surface region, float time, u32 x_offset, u32 y_offset) {
    (void)time;

    const u32 pw = pattern_width();
    const u32 ph = pattern_height();

    x_offset = (pw - (x_offset % pw)) % pw;
    y_offset %= ph;

    const u32 rw = region.get_width();
    const u32 rh = region.get_height();

    assert(rw == App::WIDTH);
    Surface row_region{row_memory, rw, rw, ph,
                       PixelFormat::RGB565}; // temporary row buffer
    // 1. Render a full row (rw x ph) to the temporary region at (0, y_offset)
    // First, render the part from (x_offset, y_offset)
    for (u32 x = x_offset; x < rw; x += pw) {
      hal::gpu::blit(row_region.subsurface(x, 0, rw - x, ph), water_pattern);
    }
    // Secondly, render the remaining part from (0, y_offset) to (x_offset,
    // y_offset + ph)
    if (x_offset > 0) {
      hal::gpu::blit(row_region.subsurface(0, 0, x_offset, ph),
                     water_pattern.subsurface(pw - x_offset, 0, x_offset, ph));
    }
    // After this, we have a complete row rendered in row_region

    // 2. Repeat the row to fill the entire region
    for (u32 y = y_offset; y < rh; y += ph) {
      hal::gpu::blit(region.subsurface(0, y, rw, std::min(ph, rh - y)),
                     row_region.as_const());
    }

    // 3. Fill in the upper region from (0, 0) to (rw, y_offset) too
    if (y_offset < ph) {
      hal::gpu::blit(
          region.subsurface(0, 0, rw, y_offset),
          row_region.subsurface(0, ph - y_offset, rw, y_offset).as_const());
    }
  }

  u32 pattern_width() const { return water_pattern.get_width(); }
  u32 pattern_height() const { return water_pattern.get_height(); }

private:
  static u16 row_memory[App::WIDTH * water_texture_HEIGHT];

  u16 water_color =
      hsv_to_rgb565(142, 255, 181); // initial water color (greenish)
  u16 sky_color = hsv_to_rgb565(150, 200, 255);
  Texture water_pattern{water_texture, water_texture_WIDTH,
                        water_texture_HEIGHT, PixelFormat::RGB565};
};
} // namespace ge
