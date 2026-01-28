#pragma once

#include "ge-app/scenes/base.hpp"
#include "ge-app/texture.hpp"
#include "ge-app/ui/menu.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/surface.hpp"

namespace ge {
namespace scenes {
class MenuScene;
namespace menu {

enum class MenuAction { StartGame, Options, Credits, ExitGame };

class MenuSelectScene : public Scene {
public:
  explicit MenuSelectScene(MenuScene &parent);

  void tick(float dt) override;
  void render(Surface &fb_region) override;
  bool on_button_clicked(Button btn) override;

  void on_menu_action(MenuAction action);

  bool is_active() const override;

private:
  MenuScene &parent;
  ui::Menu menu;
  ui::MenuItem menu_items[4];
  const char *subtitle;

  TextureRGB565 menu_bg_texture;
};

} // namespace menu
} // namespace scenes
} // namespace ge
