#pragma once

#include "ge-app/font.hpp"
#include "ge-app/scenes/scene.hpp"
#include "ge-app/ui/menu.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/gpu.hpp"
#include "ge-hal/surface.hpp"

namespace ge {

enum MenuAction { None = 0, StartGame = 1, ExitGame = 2 };

class MenuScene : public Scene {
public:
  MenuScene(App &app) : Scene{app} {
    menu.add_item("Start Game", MenuAction::StartGame);
    menu.add_item("Exit", MenuAction::ExitGame);
  }

  void tick(float dt) override {
    auto joystick = app.get_joystick_state();
    menu.move_selection(joystick.y);
  }

  void render(Surface &fb_region) override {
    // Clear screen with dark background
    hal::gpu::fill(fb_region, 0x0000);

    // Render title
    const auto &bold_font = Font::bold_font();
    const char *title = "Glow Embrace";
    u32 title_width = 0;
    const char *c = title;
    while (*c) {
      title_width += bold_font.get_glyph_width(*c);
      c++;
    }
    u32 title_x = (fb_region.get_width() - title_width) / 2;
    bold_font.render_colored(title, -1, fb_region, title_x, 40, 0xFFFF);

    // Render subtitle
    const auto &regular_font = Font::regular_font();
    const char *subtitle = "A Fangame";
    u32 subtitle_width = 0;
    c = subtitle;
    while (*c) {
      subtitle_width += regular_font.get_glyph_width(*c);
      c++;
    }
    u32 subtitle_x = (fb_region.get_width() - subtitle_width) / 2;
    regular_font.render_colored(subtitle, -1, fb_region, subtitle_x, 65,
                                0xBDF7);

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
};

} // namespace ge
