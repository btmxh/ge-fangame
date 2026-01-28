#pragma once

#include "ge-app/scenes/base.hpp"
#include "ge-app/texture.hpp"
#include "ge-app/ui/option_selector.hpp"
#include "ge-app/ui/slider.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/surface.hpp"

namespace ge {
namespace scenes {
class MenuScene;
namespace menu {

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
  explicit SettingsScene(MenuScene &parent);

  void tick(float dt) override;
  void render(Surface &fb_region) override;
  bool on_button_clicked(Button btn) override;

  // Getters
  bool is_button_flipped() const;
  float get_music_volume() const;
  float get_sfx_volume() const;

  bool is_active() const override;

private:
  MenuScene &parent;
  ui::Slider music_slider;
  ui::Slider sfx_slider;
  ui::OptionSelector button_flip_selector;
  const char *button_flip_options[2];

  u32 selected_item;
  bool joy_moved_y;

  Texture menu_bg_texture;
};

} // namespace menu
} // namespace scenes
} // namespace ge
