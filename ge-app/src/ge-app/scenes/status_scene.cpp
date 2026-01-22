#include "ge-app/scenes/status_scene.hpp"
#include "ge-app/scenes/root_management_ui_scene.hpp"
#include "ge-app/scenes/game_scene.hpp"

namespace ge {
StatusScene::StatusScene(RootManagementUIScene &parent)
    : Scene{parent.app}, parent{parent}, clock(parent.game_scene.get_clock()),
      inventory(parent.game_scene.get_inventory()),
      mode_indicator(parent.game_scene.get_mode_indicator()),
      player_stats(parent.game_scene.get_player_stats()) {}

void StatusScene::on_back_action() { parent.hide_management_screens(); }
} // namespace ge
