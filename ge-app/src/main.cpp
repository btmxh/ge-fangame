#include "ge-app/scenes/game_scene.hpp"
#include "ge-app/scenes/menu_scene.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/surface.hpp"

#include <cassert>
#include <memory>

namespace ge {

enum class SceneType { Menu, Game };

class MainApp : public ge::App {
public:
  MainApp() : ge::App() { switch_to_menu(); }

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
    menu_scene = std::unique_ptr<MenuSceneImpl>(new MenuSceneImpl(*this));
    current_scene = menu_scene.get();
  }

  void switch_to_game() {
    current_scene_type = SceneType::Game;
    game_scene = std::unique_ptr<GameScene>(new GameScene(*this));
    current_scene = game_scene.get();
  }

private:
  class MenuSceneImpl : public MenuScene {
  public:
    MenuSceneImpl(MainApp &app) : MenuScene(app), main_app(app) {}

    void on_menu_action(MenuAction action) override {
      if (action == MenuAction::StartGame) {
        main_app.switch_to_game();
      } else if (action == MenuAction::ExitGame) {
        main_app.request_quit();
      }
    }

  private:
    MainApp &main_app;
  };

  SceneType current_scene_type = SceneType::Menu;
  Scene *current_scene = nullptr;
  std::unique_ptr<MenuSceneImpl> menu_scene;
  std::unique_ptr<GameScene> game_scene;
};
} // namespace ge

int main() {
  ge::MainApp app;
  app.loop();

  return 0;

  // ge::Texture crate_tex{crate, crate_WIDTH, crate_HEIGHT};
  // float crate_x = 0.0f, crate_y = 10.0f;
}
