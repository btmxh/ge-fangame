#pragma once

#include "ge-hal/app.hpp"
#include "ge-hal/surface.hpp"

namespace ge {
class Scene {
public:
  Scene(App &app) : app{app}, active_scenes(nullptr), num_active_scenes(0), is_active(true) {}
  
  // Set sub-scenes (memory managed externally by implementation)
  void set_sub_scenes(Scene **scenes, u32 count) {
    active_scenes = scenes;
    num_active_scenes = count;
  }

  // Activate/deactivate this scene
  void set_active(bool active) { is_active = active; }
  bool get_active() const { return is_active; }

  // Virtual methods with sub-scene support
  virtual void tick(float dt) {
    if (!is_active) return;
    // Tick sub-scenes first
    for (u32 i = 0; i < num_active_scenes; ++i) {
      if (active_scenes[i] && active_scenes[i]->get_active()) {
        active_scenes[i]->tick(dt);
      }
    }
  }

  virtual void render(Surface &fb_region) {
    if (!is_active) return;
    // Render sub-scenes in order (bottom to top)
    for (u32 i = 0; i < num_active_scenes; ++i) {
      if (active_scenes[i] && active_scenes[i]->get_active()) {
        active_scenes[i]->render(fb_region);
      }
    }
  }

  // Input events return true if captured/handled
  virtual bool on_button_clicked(Button btn) {
    if (!is_active) return false;
    // Check sub-scenes in reverse order (top to bottom)
    for (i32 i = static_cast<i32>(num_active_scenes) - 1; i >= 0; --i) {
      if (active_scenes[i] && active_scenes[i]->get_active()) {
        if (active_scenes[i]->on_button_clicked(btn)) {
          return true; // Event captured by sub-scene
        }
      }
    }
    return false; // Not captured, can be overridden
  }

  virtual bool on_button_held(Button btn) {
    if (!is_active) return false;
    for (i32 i = static_cast<i32>(num_active_scenes) - 1; i >= 0; --i) {
      if (active_scenes[i] && active_scenes[i]->get_active()) {
        if (active_scenes[i]->on_button_held(btn)) {
          return true;
        }
      }
    }
    return false;
  }

  virtual bool on_button_finished_hold(Button btn) {
    if (!is_active) return false;
    for (i32 i = static_cast<i32>(num_active_scenes) - 1; i >= 0; --i) {
      if (active_scenes[i] && active_scenes[i]->get_active()) {
        if (active_scenes[i]->on_button_finished_hold(btn)) {
          return true;
        }
      }
    }
    return false;
  }

protected:
  App &app;

private:
  Scene **active_scenes;
  u32 num_active_scenes;
  bool is_active;
};
} // namespace ge
