#include "ge-app/scenes/game_scene.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/surface.hpp"

#include <cassert>

namespace ge {
class MainApp : public ge::App {
public:
  MainApp() : ge::App(), game_scene{*this} {}

  void tick(float dt) override {
    App::tick(dt);
    game_scene.tick(dt);
  }

  void render(Surface &fb) override { game_scene.render(fb); }

  void on_button_clicked(Button btn) override {
    game_scene.on_button_clicked(btn);
  }

  void on_button_held(Button btn) override { game_scene.on_button_held(btn); }

  void on_button_finished_hold(Button btn) override {
    game_scene.on_button_finished_hold(btn);
  }

private:
  GameScene game_scene;
};
} // namespace ge

int main() {
  ge::MainApp app;
  app.loop();

  return 0;

  // ge::Texture crate_tex{crate, crate_WIDTH, crate_HEIGHT};
  // float crate_x = 0.0f, crate_y = 10.0f;
}
