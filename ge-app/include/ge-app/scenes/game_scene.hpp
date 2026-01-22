#pragma once

#include "ge-app/assets/bgm.hpp"
#include "ge-app/game/boat.hpp"
#include "ge-app/game/clock.hpp"
#include "ge-app/game/compass.hpp"
#include "ge-app/game/dock.hpp"
#include "ge-app/game/fishing.hpp"
#include "ge-app/game/inventory.hpp"
#include "ge-app/game/sky.hpp"
#include "ge-app/game/water.hpp"
#include "ge-app/gfx/color.hpp"
#include "ge-app/gfx/dialog_box.hpp"
#include "ge-app/scenes/scene.hpp"

namespace ge {
struct HSV {
  float h, s, v;
};

constexpr uint16_t SKY_NIGHT_RGB = 0x0821; // very dark blue
constexpr uint16_t SKY_DAY_RGB = 0x5DBF;   // clean sky blue
constexpr uint16_t SKY_WARM_RGB = 0xF2C0;  // orange / red glow

inline float smooth01(float x) {
  x = std::max(0.0f, std::min(1.0f, x));
  return x * x * (3.0f - 2.0f * x);
}

inline float lerp(float a, float b, float t) { return a + (b - a) * t; }

// Bell curve: 0 → 1 → 0
inline float bell(float x) {
  x = std::max(0.0f, std::min(1.0f, x));
  return smooth01(1.0f - std::fabs(2.0f * x - 1.0f));
}

// ------------------------------------------------------------
// Sky color function
// ------------------------------------------------------------

inline uint16_t sky_color(float t) {
  auto blend_rgb565 = [](uint16_t a, uint16_t b, float t) {
    t = std::max(0.0f, std::min(1.0f, t));
    uint8_t ti = static_cast<uint8_t>(t * 255.0f);
    return ge::blend_rgb565(a, b, ti);
  };

  t = fmodf(t, 1.0f);
  if (t < 0)
    t += 1.0f;

  constexpr float SUNRISE_START = 0.23f;
  constexpr float SUNRISE_END = 0.27f;
  constexpr float SUNSET_START = 0.73f;
  constexpr float SUNSET_END = 0.77f;

  // --- Sunrise: night → warm → day
  if (t >= SUNRISE_START && t < SUNRISE_END) {
    float k = (t - SUNRISE_START) / (SUNRISE_END - SUNRISE_START);
    k = smooth01(k);

    if (k < 0.5f)
      return blend_rgb565(SKY_NIGHT_RGB, SKY_WARM_RGB, k * 2.0f);
    else
      return blend_rgb565(SKY_WARM_RGB, SKY_DAY_RGB, (k - 0.5f) * 2.0f);
  }

  // --- Sunset: day → warm → night
  if (t >= SUNSET_START && t < SUNSET_END) {
    float k = (t - SUNSET_START) / (SUNSET_END - SUNSET_START);
    k = smooth01(k);

    if (k < 0.5f)
      return blend_rgb565(SKY_DAY_RGB, SKY_WARM_RGB, k * 2.0f);
    else
      return blend_rgb565(SKY_WARM_RGB, SKY_NIGHT_RGB, (k - 0.5f) * 2.0f);
  }

  // --- Day
  if (t >= SUNRISE_END && t < SUNSET_START)
    return SKY_DAY_RGB;

  // --- Night
  return SKY_NIGHT_RGB;
}

class GameScene : public Scene {
public:
  GameScene(App &app) : Scene{app} {
    // TODO: flash audio when it is implemented
    // Currently we skip this step to speed up flashing
#ifndef GE_HAL_STM32
    auto ambient_bgm = assets::Bgm::ambient();
    app.audio_bgm_play(ambient_bgm.data, ambient_bgm.length, true);
#endif
    sky.set_sky_color(ge::hsv_to_rgb565(150, 200, 255));
    sky.set_cloud_color(ge::hsv_to_rgb565(0, 0, 255));
    water.set_sky_color(ge::hsv_to_rgb565(150, 200, 255));
    water.set_water_color(ge::hsv_to_rgb565(142, 255, 181));

    dialog_box.show_message(app, msg[0].title, msg[0].desc);

    // Connect inventory to fishing system
    fishing.set_inventory(&inventory);
  }

  void tick(float dt) override {
    auto current_frame_world_time = clock.get_day_timer().get(app);
    float world_dt = 0.0f;
    if (last_frame_world_time >= 0) {
      world_dt = (current_frame_world_time - last_frame_world_time) * 1e-3f;
    }
    last_frame_world_time = current_frame_world_time;

    auto joystick = app.get_joystick_state();

    // If in Management mode, show management UI (handled elsewhere)
    if (mode_indicator.get_current_mode() == GameMode::Management) {
      return; // Management mode handled by separate scenes
    }

    if (mode_indicator.get_current_mode() == GameMode::Steering) {
      if (!dialog_box.has_input_focus()) {
        boat.update_angle(joystick.x, joystick.y, world_dt);
      }
      boat.update_position(app, world_dt);
    } else if (mode_indicator.get_current_mode() == GameMode::Fishing) {
      // Update fishing system
      if (!dialog_box.has_input_focus()) {
        fishing.update(app, dialog_box, world_dt, joystick);
      }
      // Keep boat drifting slowly in fishing mode
      boat.update_position(app, world_dt);
    }
  }

  void render(Surface &fb_region) override {
    auto start_time = app.now();
    auto water_region =
        fb_region.subsurface(0, 80, ge::App::WIDTH, ge::App::HEIGHT - 80);
    sky.set_x_offset(clock.get_day_timer().get(app) / 1000);
    sky.set_sky_color(sky_color(clock.time_in_day(app)));
    sky.render(fb_region.subsurface(0, 0, ge::App::WIDTH, 80));
    water.render(water_region, app.now() * 1e-3, boat.get_x(), boat.get_y());
    dock.render(app, fb_region, boat.get_x(), boat.get_y());
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
    // Render fishing line and bobber if in fishing mode
    if (mode_indicator.get_current_mode() == GameMode::Fishing &&
        fishing.is_active()) {
      // Calculate boat center position in water region
      i32 boat_center_x = 0; // Center of water region (relative coordinates)
      i32 boat_center_y = 0;
      fishing.render(water_region, boat_center_x, boat_center_y);
    }
    {
      auto compass_region =
          fb_region.subsurface(ge::App::WIDTH - compass.get_width() - 10, 10,
                               compass.get_width(), compass.get_height());
      compass.render(compass_region, boat.get_relative_angle());
    }
    {
      static constexpr u32 PADDING = 4;
      auto clock_region = fb_region.subsurface(PADDING, PADDING, 120, 15);
      clock.render(app, clock_region);
    }
    {
      static constexpr u32 PADDING = 4;
      auto mode_indicator_region =
          fb_region.subsurface(PADDING, PADDING + 16, 120, 15);
      mode_indicator.render(mode_indicator_region);
    }

    {
      // bottom, padding 4px
      static constexpr auto dialog_height = 64, dialog_padding = 4;
      auto dialog_region = fb_region.subsurface(
          dialog_padding, ge::App::HEIGHT - dialog_height - dialog_padding,
          ge::App::WIDTH - dialog_padding * 2, dialog_height);
      dialog_box.render(app, dialog_region);
    }

    auto end_time = app.now();
    std::int64_t frame_time = end_time - start_time;
    // app.log("Frame time: %ld ms", frame_time);
  }
  void on_button_clicked(Button btn) override {
    if (dialog_box.has_input_focus()) {
      if (btn == Button::Button2) {
        dialog_box.dismiss();
        // tutorial messages
        if (++current_msg < sizeof(msg) / sizeof(msg[0])) {
          dialog_box.show_message(app, msg[current_msg].title,
                                  msg[current_msg].desc);
        } else {
          current_msg = sizeof(msg) / sizeof(msg[0]); // No more messages
        }
      } else if (btn == Button::Button1) {
        if (dialog_box.message_complete(app)) {
          dialog_box.dismiss();
          // tutorial messages
          if (++current_msg < sizeof(msg) / sizeof(msg[0])) {
            dialog_box.show_message(app, msg[current_msg].title,
                                    msg[current_msg].desc);
          } else {
            current_msg = sizeof(msg) / sizeof(msg[0]); // No more messages
          }
        } else {
          dialog_box.set_start_time();
        }

        return;
      }
    }

    if (btn == Button::Button2) {
      auto new_mode = mode_indicator.switch_mode();
      clock.set_multiplier(app, new_mode);
      
      // If switched to Management mode, notify parent to show management UI
      if (new_mode == GameMode::Management) {
        on_enter_management_mode();
      }
    } else if (btn == Button::Button1) {
      // Handle fishing dialog dismissal first (input focus)
      if (mode_indicator.get_current_mode() == GameMode::Fishing) {
        // No dialog focus, handle fishing actions
        fishing.on_button_clicked(app, dialog_box, btn);
      }
    }
  }

  // Virtual method to be called when entering management mode
  virtual void on_enter_management_mode() {
    // This will be handled by MainApp
  }

  // Getters for management scenes
  const Clock &get_clock() const { return clock; }
  const Inventory &get_inventory() const { return inventory; }
  const GameModeIndicator &get_mode_indicator() const {
    return mode_indicator;
  }

  void on_button_held(Button btn) override {
    if (dialog_box.has_input_focus()) {
      return; // Ignore held buttons when dialog has focus
    }

    if (btn == Button::Button2) {
      clock.begin_sped_up();
      clock.set_multiplier(app, mode_indicator.get_current_mode());
    }
  }

  void on_button_finished_hold(Button btn) override {
    if (dialog_box.has_input_focus()) {
      return; // Ignore held buttons when dialog has focus
    }

    if (btn == Button::Button2) {
      clock.end_sped_up();
      clock.set_multiplier(app, mode_indicator.get_current_mode());
    }
  }

private:
  // Gameplay objects
  Compass compass;
  Boat boat;
  Clock clock;
  Timer main_timer;
  Sky sky;
  Water water;
  Fishing fishing;
  Inventory inventory; // Player inventory

  DialogBox dialog_box;
  DialogMessage msg[3] = {
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
  GameModeIndicator mode_indicator;
  Dock dock;
  const ge::Font &font = ge::Font::bold_font();

  i64 last_frame_world_time = -1;
};
} // namespace ge
