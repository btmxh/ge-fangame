#pragma once

#include "ge-app/font.hpp"
#include "ge-app/scenes/scene.hpp"
#include "ge-app/ui/menu.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/gpu.hpp"
#include "ge-hal/surface.hpp"

namespace ge {

enum class MenuAction { None = 0, StartGame = 1, Options = 2, Credits = 3, ExitGame = 4 };

class MenuScene : public Scene {
public:
  MenuScene(App &app) : Scene{app} {
    // Initialize menu items array
    menu_items[0] = {"Start Game", static_cast<int>(MenuAction::StartGame)};
    menu_items[1] = {"Options", static_cast<int>(MenuAction::Options)};
    menu_items[2] = {"Credits", static_cast<int>(MenuAction::Credits)};
    menu_items[3] = {"Exit", static_cast<int>(MenuAction::ExitGame)};
    menu.set_items(menu_items, 4);
  }

  void tick(float dt) override {
    auto joystick = app.get_joystick_state();
    menu.move_selection(joystick.y);
  }

  void render(Surface &fb_region) override {
    // Clear screen with dark background
    hal::gpu::fill(fb_region, 0x0000);

    // Render title (centered, just eyeball it)
    const auto &bold_font = Font::bold_font();
    bold_font.render_colored(title, -1, fb_region, 60, 40, 0xFFFF);

    // Render subtitle (centered, just eyeball it)
    const auto &regular_font = Font::regular_font();
    regular_font.render_colored(subtitle, -1, fb_region, 80, 65, 0xBDF7);

    // Render menu
    auto menu_region = fb_region.subsurface(0, 100, fb_region.get_width(),
                                           fb_region.get_height() - 100);
    menu.render(menu_region, regular_font);
  }

  void on_button_clicked(Button btn) override {
    if (btn == Button::Button1) {
      // Button 1 is used to select in UI mode
      int selected = menu.get_selected_id();
      on_menu_action(static_cast<MenuAction>(selected));
    }
  }

  // Virtual method that can be overridden to handle menu actions
  virtual void on_menu_action(MenuAction action) {
    // This will be handled by MainApp
  }

private:
  ui::Menu menu;
  ui::MenuItem menu_items[4];
  const char *title = "Glow Embrace";
  const char *subtitle = "A Fangame";
};

} // namespace ge
