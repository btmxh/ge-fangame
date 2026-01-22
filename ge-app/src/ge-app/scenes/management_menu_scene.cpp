#include "ge-app/scenes/management_menu_scene.hpp"
#include "ge-app/scenes/root_management_ui_scene.hpp"

namespace ge {

ManagementMenuScene::ManagementMenuScene(RootManagementUIScene &parent)
    : Scene{parent.app}, parent{parent} {
  // Initialize menu items array
  menu_items[0] = {"View Status",
                   static_cast<int>(ManagementAction::ViewStatus)};
  menu_items[1] = {"View Inventory",
                   static_cast<int>(ManagementAction::ViewInventory)};
  menu_items[2] = {"Back to Game",
                   static_cast<int>(ManagementAction::BackToGame)};
  menu.set_items(menu_items, 3);
}

void ManagementMenuScene::on_menu_action(ManagementAction action) {
  if (action == ManagementAction::ViewStatus) {
    parent.show_status_screen();
  } else if (action == ManagementAction::ViewInventory) {
    parent.show_inventory_screen();
  } else if (action == ManagementAction::BackToGame) {
    // Cycle back to gameplay mode (Button B will switch mode)
    parent.hide_management_screens();
  }
}
} // namespace ge
