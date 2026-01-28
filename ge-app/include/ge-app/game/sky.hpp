#pragma once

#include "assets/out/textures/clouds.h"
#include "assets/out/textures/sun.h"
#include "ge-app/game/clock.hpp"
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

  static int max_x_offset() { return CLOUD_TEXTURE_WIDTH; }

  void render(App &app, Surface render_region, Clock &clock);

private:
  TextureARGB8888 sun_texture;
  u16 sky_color = hsv_to_rgb565(150, 200, 255),
      cloud_color = hsv_to_rgb565(0, 0, 255);
  u16 cloud_lut[sizeof(CLOUD_COLORS) / sizeof(CLOUD_COLORS[0])];
  int x_offset = 0;

  struct Rect {
    i32 x, y, w, h;
  };

  Rect render_sun(Surface render_region, float t);
};
} // namespace ge
