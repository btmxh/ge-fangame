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
  void show_status_screen() {
    status_scene.set_active(true);
    inventory_scene.set_active(false);
  }

  void show_inventory_screen() {
    status_scene.set_active(false);
    inventory_scene.set_active(true);
  }

  void hide_management_screens() {
    status_scene.set_active(false);
    inventory_scene.set_active(false);
  }

private:
  // Forward declare nested implementation classes
  class ManagementMenuImpl;
  class StatusSceneImpl;
  class InventorySceneImpl;

  GameScene &game_scene;

  // Management UI sub-scenes
  ManagementMenuImpl management_menu;
  StatusSceneImpl status_scene;
  InventorySceneImpl inventory_scene;

  Scene *management_sub_scenes[2];  // Status and Inventory as sub-scenes of ManagementMenu

  // Nested implementation classes
  class ManagementMenuImpl : public ManagementMenuScene {
  public:
    ManagementMenuImpl(RootManagementUIScene &parent)
        : ManagementMenuScene(parent.app), root_mgmt(parent) {}

    void on_menu_action(ManagementAction action) override {
      if (action == ManagementAction::ViewStatus) {
        root_mgmt.show_status_screen();
      } else if (action == ManagementAction::ViewInventory) {
        root_mgmt.show_inventory_screen();
      } else if (action == ManagementAction::BackToGame) {
        root_mgmt.hide_management_screens();
      }
    }

  private:
    RootManagementUIScene &root_mgmt;
  };

  class StatusSceneImpl : public StatusScene {
  public:
    StatusSceneImpl(RootManagementUIScene &parent, const Clock &clock,
                    const Inventory &inventory,
                    const GameModeIndicator &mode_indicator,
                    const PlayerStats &player_stats)
        : StatusScene(parent.app, clock, inventory, mode_indicator,
                      player_stats),
          root_mgmt(parent) {}

    void on_back_action() override { root_mgmt.hide_management_screens(); }

  private:
    RootManagementUIScene &root_mgmt;
  };

  class InventorySceneImpl : public InventoryScene {
  public:
    InventorySceneImpl(RootManagementUIScene &parent, Inventory &inventory,
                       PlayerStats &player_stats)
        : InventoryScene(parent.app, inventory, player_stats),
          root_mgmt(parent) {}

    void on_back_action() override { root_mgmt.hide_management_screens(); }

  private:
    RootManagementUIScene &root_mgmt;
  };
};

} // namespace ge
