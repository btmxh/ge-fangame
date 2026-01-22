#pragma once

#include "ge-app/font.hpp"
#include "ge-app/scenes/scene.hpp"
#include "ge-app/ui/menu.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/gpu.hpp"
#include "ge-hal/surface.hpp"

namespace ge {

enum class ManagementAction {
  None = 0,
  ViewStatus = 1,
  ViewInventory = 2,
  BackToGame = 3
};

class ManagementMenuScene : public Scene {
public:
  ManagementMenuScene(App &app) : Scene{app} {
    // Initialize menu items array
    menu_items[0] = {"View Status", static_cast<int>(ManagementAction::ViewStatus)};
    menu_items[1] = {"View Inventory", static_cast<int>(ManagementAction::ViewInventory)};
    menu_items[2] = {"Back to Game", static_cast<int>(ManagementAction::BackToGame)};
    menu.set_items(menu_items, 3);
  }

  void tick(float dt) override {
    auto joystick = app.get_joystick_state();
    menu.move_selection(joystick.y);
  }

  void render(Surface &fb_region) override {
    // Clear screen with dark background
    hal::gpu::fill(fb_region, 0x0000);

    // Title
    Font::regular_font().render_colored("=== Management ===", -1, fb_region, 80,
                                        30, 0xFFFF);

    // Subtitle
    Font::regular_font().render_colored("Select an option:", -1, fb_region, 70,
                                        50, 0x7BEF);

    // Render menu
    auto menu_region = fb_region.subsurface(0, 70, fb_region.get_width(),
                                            fb_region.get_height() - 70);
    menu.render(menu_region, Font::regular_font());
  }

  bool on_button_clicked(Button btn) override {
    if (btn == Button::Button1) {
      // Button 1 is used to select in UI mode
      int selected = menu.get_selected_id();
      on_menu_action(static_cast<ManagementAction>(selected));
      return true; // Event captured
    }
    return Scene::on_button_clicked(btn); // Check sub-scenes
  }

  // Virtual method that can be overridden to handle menu actions
  virtual void on_menu_action(ManagementAction action) {
    // This will be handled by MainApp
  }

private:
  ui::Menu menu;
  ui::MenuItem menu_items[3];
};

} // namespace ge
