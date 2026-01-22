#include "ge-app/scenes/credits_scene.hpp"
#include "ge-app/scenes/game_scene.hpp"
#include "ge-app/scenes/inventory_scene.hpp"
#include "ge-app/scenes/management_menu_scene.hpp"
#include "ge-app/scenes/menu_scene.hpp"
#include "ge-app/scenes/settings_scene.hpp"
#include "ge-app/scenes/status_scene.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/surface.hpp"

#include <cassert>

namespace ge {

enum class SceneType {
  Menu,
  Game,
  Settings,
  Credits,
  ManagementMenu,
  Status,
  Inventory
};

class MainApp : public ge::App {
public:
  MainApp()
      : ge::App(), menu_scene_impl(*this), game_scene_impl(*this),
        settings_scene_impl(*this), credits_scene_impl(*this),
        management_menu_scene_impl(*this), status_scene_impl(*this),
        inventory_scene_impl(*this) {
    switch_to_menu();
  }

  void tick(float dt) override {
    App::tick(dt);
    if (current_scene) {
      current_scene->tick(dt);
    }
  }

  void render(Surface &fb) override {
    if (current_scene) {
      current_scene->render(fb);
    }
  }

  void on_button_clicked(Button btn) override {
    if (current_scene) {
      current_scene->on_button_clicked(btn);
    }
  }

  void on_button_held(Button btn) override {
    if (current_scene) {
      current_scene->on_button_held(btn);
    }
  }

  void on_button_finished_hold(Button btn) override {
    if (current_scene) {
      current_scene->on_button_finished_hold(btn);
    }
  }

  void switch_to_menu() {
    current_scene_type = SceneType::Menu;
    current_scene = &menu_scene_impl;
  }

  void switch_to_game() {
    current_scene_type = SceneType::Game;
    current_scene = &game_scene_impl;
  }

  void switch_to_settings() {
    current_scene_type = SceneType::Settings;
    current_scene = &settings_scene_impl;
  }

  void switch_to_credits() {
    current_scene_type = SceneType::Credits;
    current_scene = &credits_scene_impl;
  }

  void switch_to_management_menu() {
    current_scene_type = SceneType::ManagementMenu;
    current_scene = &management_menu_scene_impl;
  }

  void switch_to_status() {
    current_scene_type = SceneType::Status;
    current_scene = &status_scene_impl;
  }

  void switch_to_inventory() {
    current_scene_type = SceneType::Inventory;
    current_scene = &inventory_scene_impl;
  }

private:
  class MenuSceneImpl : public MenuScene {
  public:
    MenuSceneImpl(MainApp &app) : MenuScene(app), main_app(app) {}

    void on_menu_action(MenuAction action) override {
      if (action == MenuAction::StartGame) {
        main_app.switch_to_game();
      } else if (action == MenuAction::Options) {
        main_app.switch_to_settings();
      } else if (action == MenuAction::Credits) {
        main_app.switch_to_credits();
      } else if (action == MenuAction::ExitGame) {
        main_app.request_quit();
      }
    }

  private:
    MainApp &main_app;
  };

  class GameSceneImpl : public GameScene {
  public:
    GameSceneImpl(MainApp &app) : GameScene(app), main_app(app) {}

    void on_enter_management_mode() override {
      main_app.switch_to_management_menu();
    }

  private:
    MainApp &main_app;
  };

  class SettingsSceneImpl : public SettingsScene {
  public:
    SettingsSceneImpl(MainApp &app) : SettingsScene(app), main_app(app) {}

    void on_back_action() override { main_app.switch_to_menu(); }

  private:
    MainApp &main_app;
  };

  class CreditsSceneImpl : public CreditsScene {
  public:
    CreditsSceneImpl(MainApp &app) : CreditsScene(app), main_app(app) {}

    void on_back_action() override { main_app.switch_to_menu(); }

  private:
    MainApp &main_app;
  };

  class ManagementMenuSceneImpl : public ManagementMenuScene {
  public:
    ManagementMenuSceneImpl(MainApp &app)
        : ManagementMenuScene(app), main_app(app) {}

    void on_menu_action(ManagementAction action) override {
      if (action == ManagementAction::ViewStatus) {
        main_app.switch_to_status();
      } else if (action == ManagementAction::ViewInventory) {
        main_app.switch_to_inventory();
      } else if (action == ManagementAction::BackToGame) {
        main_app.switch_to_game();
      }
    }

  private:
    MainApp &main_app;
  };

  class StatusSceneImpl : public StatusScene {
  public:
    // Note: Directly accesses game_scene_impl for shared game state (Clock, Inventory, ModeIndicator, PlayerStats)
    // This is by design as these objects are owned by GameScene and need to be shared
    StatusSceneImpl(MainApp &app)
        : StatusScene(app, app.game_scene_impl.get_clock(),
                      app.game_scene_impl.get_inventory(),
                      app.game_scene_impl.get_mode_indicator(),
                      app.game_scene_impl.get_player_stats()),
          main_app(app) {}

    void on_back_action() override { main_app.switch_to_management_menu(); }

  private:
    MainApp &main_app;
  };

  class InventorySceneImpl : public InventoryScene {
  public:
    // Note: Directly accesses game_scene_impl for shared inventory and player stats
    // This is by design as these are owned by GameScene
    InventorySceneImpl(MainApp &app)
        : InventoryScene(app, app.game_scene_impl.get_inventory_mutable(),
                        app.game_scene_impl.get_player_stats_mutable()),
          main_app(app) {}

    void on_back_action() override { main_app.switch_to_management_menu(); }

  private:
    MainApp &main_app;
  };

  SceneType current_scene_type = SceneType::Menu;
  Scene *current_scene = nullptr;
  MenuSceneImpl menu_scene_impl;
  GameSceneImpl game_scene_impl;
  SettingsSceneImpl settings_scene_impl;
  CreditsSceneImpl credits_scene_impl;
  ManagementMenuSceneImpl management_menu_scene_impl;
  StatusSceneImpl status_scene_impl;
  InventorySceneImpl inventory_scene_impl;
};
} // namespace ge

int main() {
  ge::MainApp app;
  app.loop();
  return 0;
}
