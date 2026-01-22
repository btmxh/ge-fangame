#pragma once

#include "ge-app/font.hpp"
#include "ge-app/scenes/scene.hpp"
#include "ge-app/ui/menu.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/gpu.hpp"
#include "ge-hal/surface.hpp"

namespace ge {

enum class MenuAction { None = 0, StartGame = 1, ExitGame = 2 };

class MenuScene : public Scene {
public:
  MenuScene(App &app) : Scene{app} {
    // Initialize menu items array
    menu_items[0] = {"Start Game", static_cast<int>(MenuAction::StartGame), 0};
    menu_items[1] = {"Exit", static_cast<int>(MenuAction::ExitGame), 0};
    menu.set_items(menu_items, 2);
    
    // Pre-calculate text widths for title and subtitle
    const auto &bold_font = Font::bold_font();
    const auto &regular_font = Font::regular_font();
    title_width = calculate_text_width(title, bold_font);
    subtitle_width = calculate_text_width(subtitle, regular_font);
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
    u32 title_x = (fb_region.get_width() - title_width) / 2;
    bold_font.render_colored(title, -1, fb_region, title_x, 40, 0xFFFF);

    // Render subtitle
    const auto &regular_font = Font::regular_font();
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
  static u32 calculate_text_width(const char *text, const Font &font) {
    u32 width = 0;
    while (*text) {
      u8 const *glyph_data;
      u8 glyph_w, glyph_h, advance;
      if (font.get_glyph(*text, glyph_data, glyph_w, glyph_h, advance)) {
        width += advance;
      } else {
        width += font.default_advance();
      }
      text++;
    }
    return width;
  }

  ui::Menu menu;
  ui::MenuItem menu_items[2];
  const char *title = "Glow Embrace";
  const char *subtitle = "A Fangame";
  u32 title_width = 0;
  u32 subtitle_width = 0;
};

} // namespace ge
