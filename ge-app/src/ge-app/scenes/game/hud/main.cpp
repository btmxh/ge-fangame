#include "ge-app/scenes/game/hud/main.hpp"

#include "ge-app/scenes/game/main.hpp"

namespace ge {
namespace scenes {
namespace game {

HUDScene::HUDScene(GameScene &parent)
    : ContainerScene(parent.get_app()), parent{parent}, mode_indicator{*this},
      compass{*this}, clock{*this} {
  set_scenes(hud_sub_scenes);
}

WorldScene &HUDScene::get_world_scene() { return parent.get_world_scene(); }
Clock &HUDScene::get_clock() { return parent.get_world_scene().get_clock(); }

void HUDScene::update_mode(GameMode new_mode) {
  parent.on_mode_changed(new_mode);
}

} // namespace game
} // namespace scenes
} // namespace ge
