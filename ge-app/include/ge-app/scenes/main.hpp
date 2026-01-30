#pragma once

#include <array>
#include <cassert>

#include "ge-app/scenes/base.hpp"
#include "ge-app/scenes/buzz.hpp"
#include "ge-app/scenes/dialog.hpp"
#include "ge-app/scenes/game/main.hpp"
#include "ge-app/scenes/menu/main.hpp"

namespace ge {
namespace scenes {

enum class RootSceneScreen {
  Game,
  Menu,
};

class RootScene : public ContainerScene {
public:
  RootScene(App &app);

  void start_game();
  void exit();

  DialogScene &get_dialog_scene() { return dialog_scene; }

  bool is_current_screen(RootSceneScreen screen) const {
    return current_screen == screen;
  }

  void switch_to_main_menu();

  BuzzScene &get_buzz_scene() { return buzz_scene; }

private:
  GameScene game_scene;
  MenuScene menu_scene;
  DialogScene dialog_scene;
  BuzzScene buzz_scene;
  std::array<Scene *, 4> sub_scenes = {&game_scene, &menu_scene, &dialog_scene,
                                       &buzz_scene};
  RootSceneScreen current_screen = RootSceneScreen::Menu;
};
} // namespace scenes
} // namespace ge
