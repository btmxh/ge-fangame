#include "ge-app/scenes/menu/settings.hpp"

#include "assets/out/textures/menu-bg.h"
#include "ge-app/font.hpp"
#include "ge-app/scenes/menu/main.hpp"
#include "ge-app/ui/menu.hpp"
#include "ge-hal/gpu.hpp"

namespace ge {
namespace scenes {
namespace menu {

SettingsScene::SettingsScene(MenuScene &parent)
    : Scene{parent.get_app()}, parent{parent}, selected_item{MUSIC_SLIDER},
      joy_moved_y{false}, menu_bg_texture{menu_bg, menu_bg_WIDTH,
                                          menu_bg_HEIGHT, PixelFormat::RGB565} {
  // Initialize sliders
  music_slider.set_label("Music Volume");
  music_slider.set_range(0.0f, 100.0f);
  music_slider.set_value(100.0f);

  sfx_slider.set_label("SFX Volume");
  sfx_slider.set_range(0.0f, 100.0f);
  sfx_slider.set_value(100.0f);

  // Initialize flip button option
  button_flip_options[0] = "Normal (A/B)";
  button_flip_options[1] = "Flipped (B/A)";
  button_flip_selector.set_label("Button Layout");
  button_flip_selector.set_options(button_flip_options, 2, 0);
}

void SettingsScene::tick(float /*dt*/) {
  auto joystick = app.get_joystick_state();

  ui::Menu::joystick_move_logic(joystick.y, joy_moved_y, selected_item,
                                NUM_SETTINGS_ITEMS);

  if (selected_item == MUSIC_SLIDER) {
    music_slider.adjust_value(joystick.x, 2.0f);
    app.audio_set_master_volume(music_slider.get_value_as_volume());
  } else if (selected_item == SFX_SLIDER) {
    sfx_slider.adjust_value(joystick.x, 2.0f);
  } else if (selected_item == BUTTON_FLIP) {
    button_flip_selector.adjust_selection(joystick.x);
  }
}

void SettingsScene::render(Surface &fb_region) {
  hal::gpu::blit(fb_region, menu_bg_texture);

  Font::bold_font().render_colored("Options", -1, fb_region, 100, 20, 0x0000);

  u32 y_offset = 102;
  constexpr u32 item_height = 40;

  auto music_region =
      fb_region.subsurface(0, y_offset, fb_region.get_width(), item_height);
  music_slider.render(music_region, Font::regular_font(),
                      selected_item == MUSIC_SLIDER);
  y_offset += item_height;

  auto sfx_region =
      fb_region.subsurface(0, y_offset, fb_region.get_width(), item_height);
  sfx_slider.render(sfx_region, Font::regular_font(),
                    selected_item == SFX_SLIDER);
  y_offset += item_height;

  auto flip_region =
      fb_region.subsurface(0, y_offset, fb_region.get_width(), item_height);
  button_flip_selector.render(flip_region, Font::regular_font(),
                              selected_item == BUTTON_FLIP);

  // Back button
  {
    u16 back_color = (selected_item == BACK_BUTTON) ? 0x0000 : 0x39E7;
    auto back_btn_region = fb_region.subsurface(20, fb_region.get_height() - 60,
                                                fb_region.get_width() - 40, 20);

    if (selected_item == BACK_BUTTON) {
      draw_rect(back_btn_region, back_color);
    }

    Font::regular_font().render_colored("Back to menu", -1, back_btn_region, 24,
                                        4, back_color);
  }
}

bool SettingsScene::on_button_clicked(Button btn) {
  if (btn == Button::Button1 && selected_item == BACK_BUTTON) {
    parent.show_select();
    return true;
  }

  return false;
}

bool SettingsScene::is_button_flipped() const {
  return button_flip_selector.get_selected_index() == 1;
}

float SettingsScene::get_music_volume() const {
  return music_slider.get_value();
}

float SettingsScene::get_sfx_volume() const { return sfx_slider.get_value(); }

bool SettingsScene::is_active() const {
  return parent.is_current_screen(MenuSceneScreen::Settings);
}
} // namespace menu
} // namespace scenes
} // namespace ge
