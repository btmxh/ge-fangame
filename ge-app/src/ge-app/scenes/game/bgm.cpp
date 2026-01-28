#include "ge-app/scenes/game/bgm.hpp"
#include "ge-app/assets/bgm.hpp"
#include "ge-app/scenes/game/main.hpp"

namespace ge {
namespace scenes {
namespace game {
BGMScene::BGMScene(GameScene &parent)
    : Scene(parent.get_app()), parent(parent) {}

void BGMScene::on_enter() {
  app.audio_bgm_stop();

  const auto &ambient_bgm = assets::Bgm::ambient();
  app.audio_bgm_play(ambient_bgm.data, ambient_bgm.length, true);
}

void BGMScene::on_exit() { app.audio_bgm_stop(); }
} // namespace game
} // namespace scenes
} // namespace ge
