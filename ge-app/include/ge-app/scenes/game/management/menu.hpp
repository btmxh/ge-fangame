#pragma once

#include "ge-app/font.hpp"
#include "ge-app/scenes/base.hpp"
#include "ge-app/ui/menu.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/gpu.hpp"
#include "ge-hal/surface.hpp"

namespace ge {
namespace scenes {
namespace game {
class ManagementUIScene;
namespace mgmt {

enum class Action {
  ViewStatus,
  ViewInventory,
  ViewMap,
};

class MenuScene : public Scene {
public:
  MenuScene(ManagementUIScene &parent);

  void tick(float dt) override {
    auto joystick = app.get_joystick_state();
    menu.move_selection(joystick.y);
  }

  void render(Surface &fb_region) override;

  bool on_button_clicked(Button btn) override {
    if (btn == Button::Button1) {
      // Button 1 is used to select in UI mode
      int selected = menu.get_selected_id();
      on_menu_action(static_cast<Action>(selected));
      return true; // Event captured
    }
    return Scene::on_button_clicked(btn); // Check sub-scenes
  }

  void on_menu_action(Action action);

  bool is_active() const override;

private:
  ManagementUIScene &parent;

  ui::Menu menu;
  ui::MenuItem menu_items[3];
};

} // namespace mgmt
} // namespace game
} // namespace scenes
} // namespace ge
