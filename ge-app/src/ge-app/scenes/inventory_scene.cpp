#include "ge-app/scenes/inventory_scene.hpp"
#include "ge-app/scenes/game_scene.hpp"

namespace ge {

InventoryScene::InventoryScene(GameScene &parent)
    : Scene{parent.get_app()}, parent{parent},
      inventory(parent.get_inventory()),
      player_stats(parent.get_player_stats()), scroll_offset(0),
      selected_index(0) {}

void InventoryScene::on_back_action() { parent.hide_management_screens(); }

} // namespace ge
