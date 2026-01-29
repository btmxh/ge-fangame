#pragma once

#include "ge-app/game/clock.hpp"
#include "ge-app/scenes/base.hpp"
#include <array>

#include "ge-app/scenes/game/hud/clock.hpp"
#include "ge-app/scenes/game/hud/compass.hpp"
#include "ge-app/scenes/game/hud/mode_indicator.hpp"
#include "ge-app/scenes/game/hud/y_hud.hpp"

namespace ge {
namespace scenes {
class GameScene;
namespace game {
class WorldScene;
class HUDScene : public ContainerScene {
public:
  HUDScene(GameScene &parent);

  WorldScene &get_world_scene();

  GameMode get_current_mode() const {
    return mode_indicator.get_current_mode();
  }

  void update_mode(GameMode old_mode, GameMode new_mode);

  Clock &get_clock();

  void start_new_game() { mode_indicator.start_new_game(); }

private:
  GameScene &parent;

  hud::ModeIndicatorScene mode_indicator;
  hud::CompassScene compass;
  hud::ClockScene clock;
  hud::YHUDScene y_hud;
  std::array<Scene *, 4> hud_sub_scenes{&mode_indicator, &compass, &clock,
                                        &y_hud};
};

} // namespace game
} // namespace scenes
} // namespace ge
