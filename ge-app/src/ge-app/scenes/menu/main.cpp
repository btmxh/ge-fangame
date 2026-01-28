#include "ge-app/scenes/menu/main.hpp"

#include "ge-app/scenes/main.hpp"

namespace ge {
namespace scenes {
MenuScene::MenuScene(RootScene &parent)
    : ContainerScene{parent.get_app()}, parent{parent}, bgm_scene(*this),
      menu_select_scene(*this), credits_scene(*this), settings_scene(*this) {
  set_scenes(sub_scenes);
  show_select();
}

void MenuScene::show_select() { current_screen = MenuSceneScreen::Select; }
void MenuScene::show_credits() { current_screen = MenuSceneScreen::Credits; }
void MenuScene::show_settings() { current_screen = MenuSceneScreen::Settings; }

void MenuScene::start_game() { parent.start_game(); }
void MenuScene::exit() { parent.exit(); }

void MenuScene::on_enter() { bgm_scene.on_enter(); }
void MenuScene::on_exit() { bgm_scene.on_exit(); }

bool MenuScene::is_active() const {
  return parent.is_current_screen(RootSceneScreen::Menu);
}
} // namespace scenes
} // namespace ge
