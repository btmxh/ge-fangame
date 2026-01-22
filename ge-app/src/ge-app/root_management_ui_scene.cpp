#include "ge-app/scenes/root_management_ui_scene.hpp"
#include "ge-app/scenes/game_scene.hpp"

namespace ge {

RootManagementUIScene::RootManagementUIScene(GameScene &parent_game_scene)
    : Scene(parent_game_scene.app),
      game_scene(parent_game_scene),
      management_menu(*this),
      status_scene(*this),
      inventory_scene(*this) {
  // Setup management sub-scenes (status and inventory are sub-scenes of
  // management menu)
  management_sub_scenes[0] = &status_scene;
  management_sub_scenes[1] = &inventory_scene;
  management_menu.set_sub_scenes(management_sub_scenes, 2);

  // Status and inventory start inactive (management menu shows first)
  status_scene.set_active(false);
  inventory_scene.set_active(false);

  // Set management menu as the only direct sub-scene of RootManagementUIScene
  // (It will delegate to status/inventory)
  // Note: We don't set sub_scenes here because ManagementMenu already contains them
}

void RootManagementUIScene::show_status_screen() {
  status_scene.set_active(true);
  inventory_scene.set_active(false);
}

void RootManagementUIScene::show_inventory_screen() {
  status_scene.set_active(false);
  inventory_scene.set_active(true);
}

void RootManagementUIScene::hide_management_screens() {
  status_scene.set_active(false);
  inventory_scene.set_active(false);
}

} // namespace ge
