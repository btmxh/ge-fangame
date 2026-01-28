#include "ge-app/scenes/game/management/inventory.hpp"
#include "ge-app/scenes/game/management/main.hpp"

namespace ge {
namespace scenes {
namespace game {
namespace mgmt {

InventoryScene::InventoryScene(ManagementUIScene &parent)
    : Scene{parent.get_app()}, parent{parent}, scroll_offset(0),
      selected_index(0) {}

void InventoryScene::on_back_action() { parent.back_to_menu(); }

PlayerStats &InventoryScene::get_player_stats() {
  return parent.get_player_stats();
}

bool InventoryScene::is_active() const {
  return parent.is_screen_active(ManagementUIScreen::Inventory);
}
} // namespace mgmt
} // namespace game
} // namespace scenes
} // namespace ge
