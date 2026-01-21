#include "ge-app/assets/bgm.hpp"
#include "ge-app/font.hpp"
#include "ge-app/game/boat.hpp"
#include "ge-app/game/compass.hpp"
#include "ge-app/game/sky.hpp"
#include "ge-app/game/water.hpp"
#include "ge-app/gfx/color.hpp"
#include "ge-app/gfx/dialog_box.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/surface.hpp"

#include <cassert>

#include "assets/out/textures/crate.h"

int main() {
  using namespace ge;
  ge::App app;
  ge::Font font = ge::Font::bold_font();

  ge::Compass compass;
  ge::Boat boat;

  // TODO: flash audio when it is implemented
  // Currently we skip this step to speed up flashing
#ifndef GE_HAL_STM32
  auto ambient_bgm = assets::Bgm::ambient();
  app.audio_bgm_play(ambient_bgm.data, ambient_bgm.length, true);
#endif

  ge::Sky sky{};
  sky.set_sky_color(ge::hsv_to_rgb565(150, 200, 255));
  sky.set_cloud_color(ge::hsv_to_rgb565(0, 0, 255));

  ge::Water water{};
  water.set_sky_color(ge::hsv_to_rgb565(150, 200, 255));
  water.set_water_color(ge::hsv_to_rgb565(142, 255, 181));

  ge::Texture crate_tex{crate, crate_WIDTH, crate_HEIGHT};
  // float crate_x = 0.0f, crate_y = 10.0f;

  ge::DialogBox dialog_box;
  ge::DialogMessage msg[3] = {
      {
          "fbk",
          "Welcome to the GE-HAL and GE-App demo!\nThis "
          "is a short tutorial on how to get startedaaaaa with the game.\n",
      },
      {
          "Controls",
          "Use the joystick to steer the boat.\n"
          "Try to explore the sea and find hidden treasures!\n",
      },
      {
          "Have fun!",
          "This demo showcases basic graphics rendering, "
          "input handling, and audio playback using GE-HAL and GE-App.\n"
          "Enjoy your time on the sea!\n",
      },
  };

  u32 current_msg = 0;

  while (app) {
    // Render
    {
      Surface fb_region;
      if (app.begin_render(fb_region)) {
        auto start_time = app.now();
        auto water_region =
            fb_region.subsurface(0, 80, ge::App::WIDTH, ge::App::HEIGHT - 80);
        sky.set_x_offset((ge::u32)(app.now() / 1000) % ge::Sky::max_x_offset());
        sky.render(fb_region.subsurface(0, 0, ge::App::WIDTH, 80));
        water.render(water_region, app.now() * 1e-3);
        font.render("Hello, World!", -1, fb_region, 10, 10,
                    [](const ge::GlyphContext &g) {
                      uint8_t hue = (uint8_t)(g.x + g.gx);
                      return ge::hsv_to_rgb565(hue, 255, 255);
                    });
        {
          auto max_side = std::max(boat.get_width(), boat.get_height());
          auto boat_region = water_region.subsurface(
              (water_region.get_width() - max_side) / 2,
              (water_region.get_height() - max_side) / 2, max_side, max_side);
          boat.render(boat_region);
        }
        {
          auto compass_region = fb_region.subsurface(
              ge::App::WIDTH - compass.get_width() - 10, 10,
              compass.get_width(), compass.get_height());
          compass.render(compass_region, boat.get_relative_angle());
        }
        if (current_msg < sizeof(msg) / sizeof(msg[0])) {
          // bottom, padding 4px
          static constexpr auto dialog_height = 64, dialog_padding = 4;
          auto dialog_region = fb_region.subsurface(
              dialog_padding, ge::App::HEIGHT - dialog_height - dialog_padding,
              ge::App::WIDTH - dialog_padding * 2, dialog_height);
          dialog_box.render(app, dialog_region, msg[current_msg]);
        }

        auto end_time = app.now();
        std::int64_t frame_time = end_time - start_time;
        // app.log("Frame time: %ld ms", frame_time);
        app.end_render();
      }
    }

    // Update
    {
      static i64 last_update = 0;
      i64 now = app.now(), delta_time = now - last_update;
      float dt = delta_time * 1e-3f;
      auto joystick = app.get_joystick_state();
      boat.update_angle(joystick.x, joystick.y, dt);
      boat.update_position(app, dt);
      last_update = now;
    }

    if (app.button_clicked(Button::Button1)) {
      if (dialog_box.message_complete(app, msg[current_msg])) {
        // advance to next message
        current_msg++;
        dialog_box.set_start_time(app.now());
      } else {
        // complete current message immediately
        dialog_box.set_start_time();
      }
    }

    app.wait_for_event();
  }
  return 0;
}
