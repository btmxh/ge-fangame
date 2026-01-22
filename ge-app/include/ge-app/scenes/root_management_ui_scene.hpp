#pragma once

#include "ge-app/scenes/inventory_scene.hpp"
#include "ge-app/scenes/management_menu_scene.hpp"
#include "ge-app/scenes/scene.hpp"
#include "ge-app/scenes/status_scene.hpp"

namespace ge {

// Forward declaration
class GameScene;

/**
 * RootManagementUIScene - Container for all management UI components
 * 
 * This scene wraps ManagementMenuScene, StatusScene, and InventoryScene,
 * providing a single cohesive management interface. It handles navigation
 * between the three screens and manages their lifecycle.
 * 
 * Structure:
 * RootManagementUIScene
 * ├─ ManagementMenuScene (navigation hub)
 * │  ├─ StatusScene (player stats, food, stamina)
 * │  └─ InventoryScene (fish list, consumption)
 */
class RootManagementUIScene : public Scene {
public:
  RootManagementUIScene(GameScene &parent_game_scene);

  // Scene lifecycle methods - delegate to sub-scenes
  void tick(float dt) override {
    Scene::tick(dt);  // Ticks active sub-scenes
  }

  void render(Surface &fb_region) override {
    Scene::render(fb_region);  // Renders active sub-scenes
  }

  // Navigation methods called by management menu
  void show_status_screen();
  void show_inventory_screen();
  void hide_management_screens();

  GameScene &game_scene; // Public access for scene implementations to get game state

private:
  // Management UI sub-scenes
  ManagementMenuScene management_menu;
  StatusScene status_scene;
  InventoryScene inventory_scene;

  Scene *management_sub_scenes[2];  // Status and Inventory as sub-scenes of ManagementMenu
};

} // namespace ge
