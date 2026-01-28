#include "ge-app/scenes/menu/credits.hpp"

#include "assets/out/textures/menu-bg.h"
#include "ge-app/font.hpp"
#include "ge-app/gfx/shape_utils.hpp"
#include "ge-hal/gpu.hpp"

#include "ge-app/scenes/menu/main.hpp"

namespace ge {
namespace scenes {
namespace menu {

CreditsScene::CreditsScene(MenuScene &parent)
    : Scene{parent.get_app()}, parent{parent},
      menu_bg_texture{menu_bg, menu_bg_WIDTH, menu_bg_HEIGHT,
                      menu_bg_FORMAT_CPP} {}

void CreditsScene::tick(float /*dt*/) {}

void CreditsScene::render(Surface &fb_region) {
  hal::gpu::blit(fb_region, menu_bg_texture);

  // Render title
  Font::bold_font().render_colored("Credits", -1, fb_region, 160, 80, 0x0000);

  u32 y_offset = 102;
  u32 line_height = Font::regular_font().line_height();

  Font::regular_font().render_colored("Original Work", -1, fb_region, 20,
                                      y_offset, 0x0000);
  y_offset += line_height;
  Font::regular_font().render_colored("  CTB Girls' Dorm", -1, fb_region, 20,
                                      y_offset, 0x39E7);
  y_offset += line_height * 3 / 2;

  Font::regular_font().render_colored("Programming", -1, fb_region, 20,
                                      y_offset, 0x0000);
  y_offset += line_height;
  Font::regular_font().render_colored("  Ministry of Education and Development",
                                      -1, fb_region, 20, y_offset, 0x39E7);
  y_offset += line_height * 3 / 2;

  Font::regular_font().render_colored("Art & Graphics", -1, fb_region, 20,
                                      y_offset, 0x0000);
  y_offset += line_height;
  Font::regular_font().render_colored("  Association of the Painters", -1,
                                      fb_region, 20, y_offset, 0x39E7);
  y_offset += line_height * 3 / 2;

  Font::regular_font().render_colored("Music", -1, fb_region, 20, y_offset,
                                      0x0000);
  y_offset += line_height;
  Font::regular_font().render_colored("  Association of the Painters", -1,
                                      fb_region, 20, y_offset, 0x39E7);
  y_offset += line_height * 3 / 2;

  Font::regular_font().render_colored("Special Thanks", -1, fb_region, 20,
                                      y_offset, 0x0000);
  y_offset += line_height;
  Font::regular_font().render_colored("  HoloEP", -1, fb_region, 20, y_offset,
                                      0x39E7);

  // Back button
  auto back_btn_region = fb_region.subsurface(20, fb_region.get_height() - 60,
                                              fb_region.get_width() - 40, 20);
  draw_rect(back_btn_region, 0x0000);
  Font::regular_font().render_colored("Back to menu", -1, back_btn_region, 24,
                                      4, 0x0000);
}

bool CreditsScene::on_button_clicked(Button btn) {
  if (btn == Button::Button1) {
    on_back_action();
    return true;
  }
  return Scene::on_button_clicked(btn);
}

void CreditsScene::on_back_action() { parent.show_select(); }

bool CreditsScene::is_active() const {
  return parent.is_current_screen(MenuSceneScreen::Credits);
}
} // namespace menu
} // namespace scenes
} // namespace ge
