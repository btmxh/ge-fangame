#include "ge-app/scenes/menu/menu_bgm.hpp"
#include "ge-app/assets/bgm.hpp"
#include "ge-app/scenes/menu/main.hpp"

namespace ge {
namespace scenes {
namespace menu {

BGMScene::BGMScene(MenuScene &parent)
    : Scene(parent.get_app()), parent(parent) {}

void BGMScene::on_enter() {
  app.audio_bgm_stop();

  const auto &menu_bgm = assets::Bgm::menu();
  app.audio_bgm_play(menu_bgm.data, menu_bgm.length, true);
}

void BGMScene::on_exit() { app.audio_bgm_stop(); }

} // namespace menu
} // namespace scenes
} // namespace ge
