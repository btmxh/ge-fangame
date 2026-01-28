#pragma once

#include "ge-app/scenes/base.hpp"
#include "ge-app/texture.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/surface.hpp"

namespace ge {
namespace scenes {
class MenuScene;

namespace menu {

class CreditsScene : public Scene {
public:
  explicit CreditsScene(MenuScene &parent);

  void tick(float dt) override;
  void render(Surface &fb_region) override;
  bool on_button_clicked(Button btn) override;

  // Handle back action
  void on_back_action();

  bool is_active() const override;

private:
  MenuScene &parent;
  TextureRGB565 menu_bg_texture;
};

} // namespace menu
} // namespace scenes
} // namespace ge
