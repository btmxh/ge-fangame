#pragma once

#include "ge-app/font.hpp"
#include "ge-app/scenes/scene.hpp"
#include "ge-app/texture.hpp"
#include "ge-app/ui/option_selector.hpp"
#include "ge-app/ui/slider.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/gpu.hpp"
#include "ge-hal/surface.hpp"

#include "assets/out/textures/menu-bg.h"

namespace ge {

// Settings item indices
enum SettingsItem : u32 {
  MUSIC_SLIDER = 0,
  SFX_SLIDER = 1,
  BUTTON_FLIP = 2,
  BACK_BUTTON = 3,
  NUM_SETTINGS_ITEMS = 4
};

class SettingsScene : public Scene {
public:
  SettingsScene(App &app) : Scene{app} {
    // Initialize sliders
    music_slider.set_label("Music Volume");
    music_slider.set_range(0.0f, 100.0f);
    music_slider.set_value(100.0f); // Default to max volume

    sfx_slider.set_label("SFX Volume");
    sfx_slider.set_range(0.0f, 100.0f);
    sfx_slider.set_value(100.0f); // Default to max volume

    // Initialize flip button option
    button_flip_options[0] = "Normal (A/B)";
    button_flip_options[1] = "Flipped (B/A)";
    button_flip_selector.set_label("Button Layout");
    button_flip_selector.set_options(button_flip_options, 2, 0);
  }

  void tick(float dt) override {
    auto joystick = app.get_joystick_state();

    // Navigate between settings items
    if (joystick.y < -0.5f && !joy_moved_y) {
      if (selected_item > 0) {
        selected_item--;
      }
      joy_moved_y = true;
    } else if (joystick.y > 0.5f && !joy_moved_y) {
      if (selected_item < BACK_BUTTON) {
        selected_item++;
      }
      joy_moved_y = true;
    } else if (joystick.y > -0.3f && joystick.y < 0.3f) {
      joy_moved_y = false;
    }

    // Adjust selected item with joystick X axis
    if (selected_item == MUSIC_SLIDER) {
      // Music slider
      music_slider.adjust_value(joystick.x, 2.0f);
      // Update app volume
      app.audio_set_master_volume(music_slider.get_value_as_volume());
    } else if (selected_item == SFX_SLIDER) {
      // SFX slider
      sfx_slider.adjust_value(joystick.x, 2.0f);
      // Note: SFX volume would need separate handling in the audio system
    } else if (selected_item == BUTTON_FLIP) {
      // Button flip option
      button_flip_selector.adjust_selection(joystick.x);
    }
  }

  void render(Surface &fb_region) override {
    hal::gpu::blit(fb_region, menu_bg_texture);

    // Render title
    Font::regular_font().render_colored("Settings", -1, fb_region, 80, 20,
                                        0x0000);

    u32 y_offset = 70;
    u32 item_height = 60;

    // Render Music Slider
    auto music_region =
        fb_region.subsurface(0, y_offset, fb_region.get_width(), item_height);
    music_slider.render(music_region, Font::regular_font(),
                        selected_item == MUSIC_SLIDER);
    y_offset += item_height;

    // Render SFX Slider
    auto sfx_region =
        fb_region.subsurface(0, y_offset, fb_region.get_width(), item_height);
    sfx_slider.render(sfx_region, Font::regular_font(),
                      selected_item == SFX_SLIDER);
    y_offset += item_height;

    // Render Button Flip Option
    auto flip_region =
        fb_region.subsurface(0, y_offset, fb_region.get_width(), item_height);
    button_flip_selector.render(flip_region, Font::regular_font(),
                                selected_item == BUTTON_FLIP);
    y_offset += item_height;

    // Render Back button
    auto back_region =
        fb_region.subsurface(0, y_offset, fb_region.get_width(), item_height);
    u16 back_color = (selected_item == BACK_BUTTON) ? 0x0000 : 0xBDF7;

    // Draw border for selected back button
    if (selected_item == BACK_BUTTON) {
      u32 padding = 4;
      auto border_region = back_region.subsurface(
          padding, padding, back_region.get_width() - padding * 2,
          back_region.get_height() - padding * 2);

      // Draw border
      hal::gpu::fill(
          border_region.subsurface(0, 0, border_region.get_width(), 2),
          0x0000);
      hal::gpu::fill(border_region.subsurface(0, border_region.get_height() - 2,
                                              border_region.get_width(), 2),
                     0x0000);
      hal::gpu::fill(
          border_region.subsurface(0, 0, 2, border_region.get_height()),
          0x0000);
      hal::gpu::fill(border_region.subsurface(border_region.get_width() - 2, 0,
                                              2, border_region.get_height()),
                     0x0000);
    }

    Font::regular_font().render_colored("Back to Menu", -1, back_region, 70, 8,
                                        back_color);
  }

  void on_button_clicked(Button btn) override {
    if (btn == Button::Button1 && selected_item == BACK_BUTTON) {
      // Back button selected
      on_back_action();
    }
  }

  // Virtual method to handle back action
  virtual void on_back_action() {
    // This will be handled by MainApp
  }

  // Getters for settings
  bool is_button_flipped() const {
    return button_flip_selector.get_selected_index() == 1;
  }

  float get_music_volume() const { return music_slider.get_value(); }

  float get_sfx_volume() const { return sfx_slider.get_value(); }

private:
  ui::Slider music_slider;
  ui::Slider sfx_slider;
  ui::OptionSelector button_flip_selector;
  const char *button_flip_options[2];

  u32 selected_item = MUSIC_SLIDER;
  bool joy_moved_y = false;

  Texture menu_bg_texture{menu_bg, menu_bg_WIDTH, menu_bg_HEIGHT,
                          PixelFormat::RGB565};
};

} // namespace ge
