#pragma once

#include "ge-app/font.hpp"
#include "ge-app/scenes/scene.hpp"
#include "ge-app/texture.hpp"
#include "ge-app/ui/menu.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/gpu.hpp"
#include "ge-hal/surface.hpp"

#include "assets/out/textures/menu-bg.h"

namespace ge {

class CreditsScene : public Scene {
public:
  CreditsScene(App &app) : Scene{app} {
    // Initialize back button
    menu_items[0] = {"Back", 1};
    menu.set_items(menu_items, 1);
  }

  void tick(float dt) override {
    auto joystick = app.get_joystick_state();
    menu.move_selection(-joystick.y);
  }

  void render(Surface &fb_region) override {
    hal::gpu::blit(fb_region, menu_bg_texture);

    // Render title
    Font::regular_font().render_colored("Credits", -1, fb_region, 80, 20,
                                        0x0000);

    // Render credits content (placeholders)
    u32 y_offset = 60;
    u32 line_height = Font::regular_font().line_height() + 4;

    Font::regular_font().render_colored("Game Development", -1, fb_region, 20,
                                        y_offset, 0x0000);
    y_offset += line_height;
    Font::regular_font().render_colored("  CTB Girls' Dorm", -1, fb_region, 20,
                                        y_offset, 0xBDF7);
    y_offset += line_height + 10;

    Font::regular_font().render_colored("Programming", -1, fb_region, 20,
                                        y_offset, 0x0000);
    y_offset += line_height;
    Font::regular_font().render_colored("  Placeholder Developer", -1,
                                        fb_region, 20, y_offset, 0xBDF7);
    y_offset += line_height + 10;

    Font::regular_font().render_colored("Art & Graphics", -1, fb_region, 20,
                                        y_offset, 0x0000);
    y_offset += line_height;
    Font::regular_font().render_colored("  Placeholder Artist", -1, fb_region,
                                        20, y_offset, 0xBDF7);
    y_offset += line_height + 10;

    Font::regular_font().render_colored("Music", -1, fb_region, 20, y_offset,
                                        0x0000);
    y_offset += line_height;
    Font::regular_font().render_colored("  Placeholder Composer", -1,
                                        fb_region, 20, y_offset, 0xBDF7);
    y_offset += line_height + 10;

    Font::regular_font().render_colored("Special Thanks", -1, fb_region, 20,
                                        y_offset, 0x0000);
    y_offset += line_height;
    Font::regular_font().render_colored("  Placeholder Team", -1, fb_region,
                                        20, y_offset, 0xBDF7);

    // Render back button at bottom
    auto menu_region =
        fb_region.subsurface(0, fb_region.get_height() - 60,
                             fb_region.get_width(), 60);
    menu.render(menu_region, Font::regular_font());
  }

  void on_button_clicked(Button btn) override {
    if (btn == Button::Button1) {
      // Button 1 selects the back button
      int selected = menu.get_selected_id();
      if (selected == 1) {
        on_back_action();
      }
    }
  }

  // Virtual method to handle back action
  virtual void on_back_action() {
    // This will be handled by MainApp
  }

private:
  ui::Menu menu;
  ui::MenuItem menu_items[1];

  Texture menu_bg_texture{menu_bg, menu_bg_WIDTH, menu_bg_HEIGHT,
                          PixelFormat::RGB565};
};

} // namespace ge
