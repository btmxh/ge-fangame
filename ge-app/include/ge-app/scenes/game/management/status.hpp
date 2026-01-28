#pragma once

#include "ge-app/scenes/base.hpp"
#include "ge-hal/surface.hpp"

namespace ge {
namespace scenes {
namespace game {

class ManagementUIScene;

namespace mgmt {

class StatusScene : public Scene {
public:
  explicit StatusScene(ManagementUIScene &parent);

  void render(Surface &fb_region) override;
  bool on_button_clicked(Button btn) override;

  void on_back_action();

  bool is_active() const override;

private:
  // Helper to draw a status bar
  void draw_status_bar(const Surface &region, float percent, u16 color);

  ManagementUIScene &parent;
};

} // namespace mgmt
} // namespace game
} // namespace scenes
} // namespace ge
