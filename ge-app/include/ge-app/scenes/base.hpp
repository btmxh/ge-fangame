#pragma once

#include "ge-hal/app.hpp"
#include "ge-hal/surface.hpp"

namespace ge {
namespace scenes {

class Scene {
public:
  explicit Scene(App &app) : app(app) {}
  virtual ~Scene() = default;

  // --- lifecycle -------------------------------------------------

  virtual bool is_active() const { return true; }

  // Update logic
  virtual void tick(float /*dt*/) {}

  // Render into framebuffer region
  virtual void render(Surface & /*fb_region*/) {}

  // --- input -----------------------------------------------------

  // Return true if event is handled / captured
  // joystick state can be accessed through App,
  // but handling it via a dedicated method is more idiomatic
  // (allow for capturing input)
  virtual bool on_joystick_moved(float /*dt*/, float /*x*/, float /*y*/) {
    return false;
  }
  virtual bool on_button_clicked(Button /*btn*/) { return false; }

  // on_button_held and on_button_finished_hold is a VERY weird pair of event
  // handlers: on_button_held: called once when a button is held down for a
  // certain threshold (1s) on_button_finished_hold: called once when a held
  // button is released HOWEVER, on_button_finished_hold WILL NOT BE CALLED if
  // the button was released AFTER is_handling_input() returned false.
  //
  // e.g. RootScene contains 2 scenes: A and B
  // A: override on_button_held and on_button_finished_hold for Button2
  // B: toggle A's active state when Button1 is called
  // Input sequence:
  // Hold Button2 -> on_button_held called in A
  // Click Button1 -> B toggles A to inactive
  // Release Button2 -> on_button_finished_hold called in B instead of A
  //
  // In this example, we must make sure:
  // - on_button_held in A should have some mechanism to "release" Button2
  //   when it loses focus.
  // - on_button_finished_hold in B should be no-op if on_button_held was
  //   not called in B.
  //
  // It is advised to use Scene::on_enter and Scene::on_exit to manage such
  // state.
  virtual bool on_button_held(Button btn) { return false; }

  virtual bool on_button_finished_hold(Button btn) { return false; }

  App &get_app() { return app; }

protected:
  App &app;
};

class ContainerScene : public Scene {
public:
  explicit ContainerScene(App &app)
      : Scene(app), scenes(nullptr), scene_count(0) {}

  // Memory owned externally
  void set_scenes(Scene **new_scenes, u32 count) {
    assert(count < 32 && "ContainerScene only supports up to 32 sub-scenes");
    scenes = new_scenes;
    scene_count = count;
  }

  template <class SceneContainer> void set_scenes(SceneContainer &container) {
    scenes = container.data();
    scene_count = static_cast<u32>(container.size());
  }

  // --- lifecycle -------------------------------------------------

  void tick(float dt) override {
    // Bottom -> top
    for (u32 i = 0; i < scene_count; ++i) {
      Scene *s = scenes[i];
      if (s && s->is_active()) {
        s->tick(dt);
      }
    }
  }

  void render(Surface &fb_region) override {
    // Bottom -> top
    for (u32 i = 0; i < scene_count; ++i) {
      Scene *s = scenes[i];
      if (s && s->is_active()) {
        s->render(fb_region);
      }
    }
  }

  // --- input -----------------------------------------------------

  bool on_joystick_moved(float dt, float x, float y) override {
    (void)Scene::on_joystick_moved(dt, x, y);
    return dispatch_input(
        [&](Scene *s) { return s->on_joystick_moved(dt, x, y); });
  }

  bool on_button_clicked(Button btn) override {
    (void)Scene::on_button_clicked(btn);
    return dispatch_input([&](Scene *s) { return s->on_button_clicked(btn); });
  }

  bool on_button_held(Button btn) override {
    (void)Scene::on_button_held(btn);
    return dispatch_input([&](Scene *s) { return s->on_button_held(btn); });
  }

  bool on_button_finished_hold(Button btn) override {
    (void)Scene::on_button_finished_hold(btn);
    return dispatch_input(
        [&](Scene *s) { return s->on_button_finished_hold(btn); });
  }

protected:
  // Centralized top -> bottom dispatch
  template <typename Fn> bool dispatch_input(Fn &&fn) {
    for (i32 i = static_cast<i32>(scene_count) - 1; i >= 0; --i) {
      Scene *s = scenes[i];
      if (s && s->is_active()) {
        if (fn(s))
          return true;
      }
    }
    return false;
  }

private:
  Scene **scenes;
  u32 scene_count;
};
} // namespace scenes
} // namespace ge
