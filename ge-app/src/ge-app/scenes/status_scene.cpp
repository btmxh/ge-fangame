#include "ge-app/scenes/status_scene.hpp"
#include "ge-app/scenes/game_scene.hpp"

namespace ge {
StatusScene::StatusScene(GameScene &parent)
    : Scene{parent.get_app()}, parent{parent}, clock(parent.get_clock()),
      inventory(parent.get_inventory()),
      mode_indicator(parent.get_mode_indicator()),
      player_stats(parent.get_player_stats()) {}

void StatusScene::on_back_action() { parent.hide_management_screens(); }
} // namespace ge
