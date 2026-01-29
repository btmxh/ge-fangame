#pragma once

#include "ge-app/game/mode_indicator.hpp"
#include "ge-app/scenes/base.hpp"
#include "ge-app/scenes/game/hud/mode_indicator.hpp"

namespace ge {
namespace scenes {
namespace game {
class HUDScene;
namespace hud {

class ModeIndicatorScene : public Scene {
public:
  ModeIndicatorScene(HUDScene &parent);

  void render(Surface &fb_region) override;

  bool on_button_clicked(Button btn) override {
    if (btn == Button::Button2) {
      switch_mode();
      return true;
    }

    return false;
  }

  GameMode switch_mode();
  GameMode get_current_mode() const { return indicator.get_current_mode(); }

  void start_new_game() { indicator.reset(); }

private:
  HUDScene &parent;
  GameModeIndicator indicator;
};

} // namespace hud
} // namespace game
} // namespace scenes
} // namespace ge
