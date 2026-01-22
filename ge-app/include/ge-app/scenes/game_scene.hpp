#pragma once

#include "ge-app/assets/bgm.hpp"
#include "ge-app/game/boat.hpp"
#include "ge-app/game/clock.hpp"
#include "ge-app/game/compass.hpp"
#include "ge-app/game/dock.hpp"
#include "ge-app/game/fishing.hpp"
#include "ge-app/game/inventory.hpp"
#include "ge-app/game/player_stats.hpp"
#include "ge-app/game/sky.hpp"
#include "ge-app/game/tutorial_system.hpp"
#include "ge-app/game/water.hpp"
#include "ge-app/gfx/color.hpp"
#include "ge-app/scenes/dialog_scene.hpp"
#include "ge-app/scenes/inventory_scene.hpp"
#include "ge-app/scenes/management_menu_scene.hpp"
#include "ge-app/scenes/scene.hpp"
#include "ge-app/scenes/status_scene.hpp"

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
private:
  // Forward declare nested classes for management scenes
  class ManagementMenuImpl;
  class StatusSceneImpl;
  class InventorySceneImpl;

public:
  GameScene(App &app)
      : Scene{app}, dialog_scene(app), management_menu_scene(*this),
        status_scene(*this), inventory_scene(*this) {
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

    // Connect inventory to fishing system
    fishing.set_inventory(&inventory);

    // Setup management sub-scenes (management_menu owns status and inventory)
    management_sub_scenes[0] = &status_scene;
    management_sub_scenes[1] = &inventory_scene;
    management_menu_scene.set_sub_scenes(management_sub_scenes, 2);

    // Status and inventory start inactive
    status_scene.set_active(false);
    inventory_scene.set_active(false);

    // Setup game sub-scenes (DialogScene and ManagementMenu)
    sub_scene_array[0] = &dialog_scene;
    sub_scene_array[1] = &management_menu_scene;
    set_sub_scenes(sub_scene_array, 2);

    // Management starts inactive
    management_menu_scene.set_active(false);

    // Initialize tutorial
    tutorial.initialize(dialog_scene, tutorial_messages, 3);
  }

  void tick(float dt) override {
    // Tick sub-scenes first (includes DialogScene and ManagementMenu)
    Scene::tick(dt);

    auto current_frame_world_time = clock.get_day_timer().get(app);
    float world_dt = 0.0f;
    if (last_frame_world_time >= 0) {
      world_dt = (current_frame_world_time - last_frame_world_time) * 1e-3f;
    }
    last_frame_world_time = current_frame_world_time;

    auto joystick = app.get_joystick_state();

    // If in Management mode, management UI is handled by sub-scenes
    if (mode_indicator.get_current_mode() == GameMode::Management) {
      return; // Management menu and sub-scenes handle everything
    }

    // Dialog captures input, so we don't update game state when it's showing
    if (dialog_scene.get_active()) {
      return;
    }

    if (mode_indicator.get_current_mode() == GameMode::Steering) {
      boat.update_angle(joystick.x, joystick.y, world_dt);

      // Update player stats (stamina drains when steering)
      player_stats.update(world_dt, boat.get_angle(), true);

      // Check if Button A is held for acceleration
      float cargo_weight = inventory.get_total_weight();
      boat.update_position(app, world_dt, is_accelerating, cargo_weight);
    } else if (mode_indicator.get_current_mode() == GameMode::Fishing) {
      // Update fishing system
      fishing.update(app, dialog_scene, world_dt, joystick);

      // Keep boat drifting slowly in fishing mode
      float cargo_weight = inventory.get_total_weight();
      boat.update_position(app, world_dt, false, cargo_weight);

      // Update player stats (no stamina drain in fishing mode)
      player_stats.update(world_dt, boat.get_angle(), false);
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

    // Render sub-scenes (includes DialogScene)
    Scene::render(fb_region);

    auto end_time = app.now();
    std::int64_t frame_time = end_time - start_time;
    // app.log("Frame time: %ld ms", frame_time);
  }
  bool on_button_clicked(Button btn) override {
    // Check sub-scenes first (DialogScene and ManagementMenu will capture if
    // active)
    if (Scene::on_button_clicked(btn)) {
      // If dialog was dismissed and tutorial not completed, show next message
      if (!dialog_scene.get_active() && !tutorial.is_completed()) {
        tutorial.next_message(dialog_scene);
      }
      return true;
    }

    // Handle game-specific input
    if (btn == Button::Button2) {
      auto new_mode = mode_indicator.switch_mode();
      clock.set_multiplier(app, new_mode);

      // Activate/deactivate management menu based on mode
      if (new_mode == GameMode::Management) {
        management_menu_scene.set_active(true);
      } else {
        management_menu_scene.set_active(false);
        // Also deactivate management sub-scenes
        status_scene.set_active(false);
        inventory_scene.set_active(false);
      }
      return true;
    } else if (btn == Button::Button1) {
      // Handle fishing actions
      if (mode_indicator.get_current_mode() == GameMode::Fishing) {
        fishing.on_button_clicked(app, dialog_scene, btn);
        return true;
      }
    }

    return false;
  }

  // Getters for management scenes
  const Clock &get_clock() const { return clock; }
  const Inventory &get_inventory() const { return inventory; }
  const GameModeIndicator &get_mode_indicator() const { return mode_indicator; }
  const PlayerStats &get_player_stats() const { return player_stats; }
  Inventory &get_inventory() { return inventory; }
  PlayerStats &get_player_stats() { return player_stats; }

  // Management scene navigation (called by management menu sub-scene)
  void show_status_screen() {
    status_scene.set_active(true);
    inventory_scene.set_active(false);
  }

  void show_inventory_screen() {
    status_scene.set_active(false);
    inventory_scene.set_active(true);
  }

  void hide_management_screens() {
    status_scene.set_active(false);
    inventory_scene.set_active(false);
  }

  bool on_button_held(Button btn) override {
    // Check sub-scenes first
    if (Scene::on_button_held(btn)) {
      return true;
    }

    if (btn == Button::Button1 &&
        mode_indicator.get_current_mode() == GameMode::Steering) {
      // Hold Button A to accelerate in Steering mode
      is_accelerating = true;
      return true;
    } else if (btn == Button::Button2) {
      clock.begin_sped_up();
      clock.set_multiplier(app, mode_indicator.get_current_mode());
      return true;
    }

    return false;
  }

  bool on_button_finished_hold(Button btn) override {
    // Check sub-scenes first
    if (Scene::on_button_finished_hold(btn)) {
      return true;
    }

    if (btn == Button::Button1) {
      // Release acceleration
      is_accelerating = false;
      return true;
    } else if (btn == Button::Button2) {
      clock.end_sped_up();
      clock.set_multiplier(app, mode_indicator.get_current_mode());
      return true;
    }

    return false;
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
  Inventory inventory;      // Player inventory
  PlayerStats player_stats; // Player food and stamina

  // Sub-scenes
  DialogScene dialog_scene;
  ManagementMenuScene management_menu_scene;
  StatusScene status_scene;
  InventoryScene inventory_scene;

  Scene *sub_scene_array[2]; // Array for GameScene sub-scenes (DialogScene,
                             // ManagementMenu)
  Scene *management_sub_scenes[2]; // Array for ManagementMenu sub-scenes
                                   // (Status, Inventory)

  // Tutorial system
  TutorialSystem tutorial;
  DialogMessage tutorial_messages[3] = {
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

  GameModeIndicator mode_indicator;
  Dock dock;
  const ge::Font &font = ge::Font::bold_font();

  i64 last_frame_world_time = -1;
  bool is_accelerating = false; // Track if Button A is held for acceleration
};
} // namespace ge
