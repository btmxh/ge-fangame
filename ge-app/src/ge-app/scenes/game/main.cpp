#include "ge-app/scenes/game/main.hpp"

#include "ge-app/assets/bgm.hpp"
#include "ge-app/scenes/dialog.hpp"
#include "ge-app/scenes/main.hpp"

namespace ge {
namespace scenes {

// ============================================================
// GameScene
// ============================================================

static DialogMessage tutorial_messages[3] = {
    {
        "fbk",
        "Welcome to the GE-HAL and GE-App demo!\n"
        "This is a short tutorial on how to get startedaaaaa with the "
        "game.\n",
    },
    {
        "Controls",
        "Use the joystick to steer the boat.\n"
        "Try to explore the sea and find hidden treasures!\n",
    },
    {
        "Have fun!",
        "This demo showcases basic graphics rendering, "
        "input handling, and audio playback using GE-HAL and GE-App.\n"
        "Enjoy your time on the sea!\n",
    },
};

GameScene::GameScene(RootScene &parent)
    : ContainerScene{parent.get_app()}, parent{parent}, bgm{*this},
      world{*this}, hud{*this}, management_ui(*this), game_over{*this} {
  set_scenes(sub_scene_array);
  // tutorial.initialize(dialog_scene, tutorial_messages, 3);
}

DialogScene &GameScene::get_dialog_scene() { return parent.get_dialog_scene(); }

void GameScene::on_mode_changed(GameMode old_mode, GameMode new_mode) {
  if (old_mode == new_mode)
    return;
  // // DEBUG: trigger game over
  // get_player_stats().apply_damage(app, 10000);
  world.on_mode_changed(old_mode, new_mode);
}

// TODO: implement boat acceleration + time speed up
// bool GameScene::on_button_held(Button btn) {
//   if (Scene::on_button_held(btn))
//     return true;
//
//   if (btn == Button::Button1 &&
//       mode_indicator.get_current_mode() == GameMode::Steering) {
//     is_accelerating = true;
//     return true;
//   }
//
//   if (btn == Button::Button2) {
//     clock.begin_sped_up();
//     clock.set_multiplier(app, mode_indicator.get_current_mode());
//     return true;
//   }
//
//   return false;
// }

// bool GameScene::on_button_finished_hold(Button btn) {
//   if (Scene::on_button_finished_hold(btn))
//     return true;
//
//   if (btn == Button::Button1) {
//     is_accelerating = false;
//     return true;
//   }
//
//   if (btn == Button::Button2) {
//     clock.end_sped_up();
//     clock.set_multiplier(app, mode_indicator.get_current_mode());
//     return true;
//   }
//
//   return false;
// }

bool GameScene::is_active() const {
  return parent.is_current_screen(RootSceneScreen::Game);
}
void GameScene::return_to_main_menu() {
  world.end_game();
  parent.switch_to_main_menu();
}
} // namespace scenes
} // namespace ge
