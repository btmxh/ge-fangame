#pragma once

#include "credits.hpp"
#include "ge-app/scenes/menu/menu_bgm.hpp"
#include "select.hpp"
#include "settings.hpp"

#include <array>

namespace ge {
namespace scenes {
class RootScene;

enum class MenuSceneScreen {
  Select,
  Credits,
  Settings,
};

class MenuScene : public ContainerScene {
public:
  MenuScene(RootScene &parent);

  void start_game();
  void exit();

  void show_select();
  void show_credits();
  void show_settings();

  void on_enter();
  void on_exit();

  bool is_current_screen(MenuSceneScreen screen) const {
    return current_screen == screen;
  }

  bool is_active() const override;

private:
  RootScene &parent;

  menu::BGMScene bgm_scene;
  menu::MenuSelectScene menu_select_scene;
  menu::CreditsScene credits_scene;
  menu::SettingsScene settings_scene;

  std::array<Scene *, 4> sub_scenes = {
      &bgm_scene,
      &menu_select_scene,
      &credits_scene,
      &settings_scene,
  };

  MenuSceneScreen current_screen = MenuSceneScreen::Select;
};
} // namespace scenes
} // namespace ge
