#pragma once

#include "ge-app/scenes/scene.hpp"

namespace ge {

// Root scene that manages top-level scenes (Menu, Game, Settings, Credits)
// and the global DialogScene
class RootScene : public Scene {
public:
  RootScene(App &app) : Scene(app) {}

  // RootScene itself doesn't render anything, just manages sub-scenes
  void render(Surface &fb_region) override {
    // Only render active sub-scenes (handled by base Scene class)
    Scene::render(fb_region);
  }

  void tick(float dt) override {
    // Tick active sub-scenes (handled by base Scene class)
    Scene::tick(dt);
  }

  bool on_button_clicked(Button btn) override {
    // Delegate to sub-scenes (handled by base Scene class)
    return Scene::on_button_clicked(btn);
  }

  bool on_button_held(Button btn) override {
    return Scene::on_button_held(btn);
  }

  bool on_button_finished_hold(Button btn) override {
    return Scene::on_button_finished_hold(btn);
  }
};

} // namespace ge
