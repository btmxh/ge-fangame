#pragma once

#include "ge-hal/app.hpp"
#include "ge-hal/surface.hpp"

namespace ge {
class Scene {
public:
  Scene(App &app) : app{app} {}
  virtual void tick(float dt) {}
  virtual void render(Surface &fb_region) {}
  virtual void on_button_clicked(Button btn) {}
  virtual void on_button_held(Button btn) {}
  virtual void on_button_finished_hold(Button btn) {}

protected:
  App &app;
};
} // namespace ge
