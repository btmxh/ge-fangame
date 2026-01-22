#include "ge-app/scenes/credits_scene.hpp"
#include "ge-app/scenes/game_scene.hpp"
#include "ge-app/scenes/menu_scene.hpp"
#include "ge-app/scenes/root_scene.hpp"
#include "ge-app/scenes/settings_scene.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/surface.hpp"

namespace ge {

class MainApp : public ge::App {
public:
  MainApp()
      : ge::App(), menu_scene_impl(*this), game_scene_impl(*this),
        settings_scene_impl(*this), credits_scene_impl(*this),
        root_scene(*this) {
    // Setup root scene with main scenes as sub-scenes
    main_scenes[0] = &menu_scene_impl;
    main_scenes[1] = &game_scene_impl;
    main_scenes[2] = &settings_scene_impl;
    main_scenes[3] = &credits_scene_impl;
    root_scene.set_sub_scenes(main_scenes, 4);

    // Start with menu active, others inactive
    game_scene_impl.set_active(false);
    settings_scene_impl.set_active(false);
    credits_scene_impl.set_active(false);
  }

  void tick(float dt) override {
    App::tick(dt);
    root_scene.tick(dt);
  }

  void render(Surface &fb) override { root_scene.render(fb); }

  void on_button_clicked(Button btn) override {
    root_scene.on_button_clicked(btn);
  }

  void on_button_held(Button btn) override { root_scene.on_button_held(btn); }

  void on_button_finished_hold(Button btn) override {
    root_scene.on_button_finished_hold(btn);
  }

  void switch_to_menu() {
    menu_scene_impl.set_active(true);
    game_scene_impl.set_active(false);
    settings_scene_impl.set_active(false);
    credits_scene_impl.set_active(false);
  }

  void switch_to_game() {
    menu_scene_impl.set_active(false);
    game_scene_impl.set_active(true);
    settings_scene_impl.set_active(false);
    credits_scene_impl.set_active(false);
  }

  void switch_to_settings() {
    menu_scene_impl.set_active(false);
    game_scene_impl.set_active(false);
    settings_scene_impl.set_active(true);
    credits_scene_impl.set_active(false);
  }

  void switch_to_credits() {
    menu_scene_impl.set_active(false);
    game_scene_impl.set_active(false);
    settings_scene_impl.set_active(false);
    credits_scene_impl.set_active(true);
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

  RootScene root_scene;
  MenuSceneImpl menu_scene_impl;
  GameSceneImpl game_scene_impl;
  SettingsSceneImpl settings_scene_impl;
  CreditsSceneImpl credits_scene_impl;
  Scene *main_scenes[4];
};

} // namespace ge

int main() {
  ge::MainApp app;
  app.loop();
  return 0;
}
