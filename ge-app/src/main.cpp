#include "ge-app/game/boat.hpp"
#include "ge-app/game/compass.hpp"
#include "ge-app/game/perspective.hpp"
#include "ge-app/game/sky.hpp"
#include "ge-app/game/water.hpp"
#include "ge-app/gfx/color.hpp"
#include "ge-app/text.hpp"
#include "ge-app/texture.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/fb.hpp"

#include <cassert>

#include <bg_clouds.h>
#include <bgm_ambient.h>
#include <bgm_menu.h>
#include <crate.h>
#include <default-boat.h>

int main() {
  ge::App app;
  ge::Font font;

  ge::Compass compass;
  ge::Boat boat;

  // TODO: flash audio when it is implemented
  // Currently we skip this step to speed up flashing
#ifndef GE_HAL_STM32
  app.audio_bgm_play(bgm_ambient, bgm_ambient_len, true);
#endif

  ge::Sky sky{};
  sky.set_sky_color(ge::hsv_to_rgb565(150, 200, 255));
  sky.set_cloud_color(ge::hsv_to_rgb565(0, 0, 255));

  ge::Water water{};
  water.set_sky_color(ge::hsv_to_rgb565(150, 200, 255));
  water.set_water_color(ge::hsv_to_rgb565(142, 255, 181));

  ge::Texture crate{crate_color, crate_alpha, crate_width, crate_height};
  float crate_x = 0.0f, crate_y = 10.0f;

  ge::PerspectiveMapping mapping;

  float dt = 0.0f;

  while (app) {
    auto fb_region = app.begin();
    auto start_time = app.now();

    auto joystick = app.get_joystick_state();
    boat.update_angle(joystick.x, joystick.y, dt);
    boat.update_position(app, dt);
    auto water_region =
        fb_region.subsurface(0, 80, ge::App::WIDTH, ge::App::HEIGHT - 80);

    sky.set_x_offset((int)(app.now() * 1e-3) % ge::Sky::max_x_offset());
    sky.render(fb_region.subsurface(0, 0, ge::App::WIDTH, 80));
    water.render(water_region, app.now() * 1e-3);

    font.render("Hello, World!", fb_region, 10, 10, 1,
                [](const ge::GlyphContext &g) {
                  uint8_t hue = (uint8_t)(g.x + g.gx);
                  return ge::hsv_to_rgb565(hue, 255, 255);
                });

    {
      float dx = crate_x - boat.get_x();
      float dy = crate_y - boat.get_y();
      float sx, sy;
      mapping.transform_xy(dx, dy, sx, sy);
      // app.log("Crate relative pos: (%.2f, %.2f) -> screen (%.2f, %.2f)", dx,
      // dy,
      //         sx, sy);
      float scale = mapping.scale_at(dy) * 3;
      // app.log("  scale: %.3f", scale);

      crate.blit_scaled(water_region, sx, sy, scale, scale);
    }

    {
      auto max_side = std::max(default_boat_width, default_boat_height);
      auto boat_region = water_region.subsurface(
          (water_region.get_width() - max_side) / 2,
          (water_region.get_height() - max_side) / 2, max_side, max_side);
      boat.render(boat_region);
    }

    {
      auto compass_region =
          fb_region.subsurface(ge::App::WIDTH - compass_base_width - 10, 10,
                               compass_base_width, compass_base_height);
      compass.render(compass_region, boat.get_relative_angle());
    }

    auto end_time = app.now();

    std::int64_t frame_time = end_time - start_time;
    app.log("Frame time: %ld ms", frame_time);

    app.end();
  }
  return 0;
}
