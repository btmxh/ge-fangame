#pragma once

#include "ge-app/game/clock.hpp"
#include "ge-app/scenes/base.hpp"
#include <array>

#include "ge-app/scenes/game/hud/clock.hpp"
#include "ge-app/scenes/game/hud/compass.hpp"
#include "ge-app/scenes/game/hud/mode_indicator.hpp"

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

  void update_mode(GameMode new_mode);

  Clock &get_clock();

private:
  GameScene &parent;

  hud::ModeIndicatorScene mode_indicator;
  hud::CompassScene compass;
  hud::ClockScene clock;
  std::array<Scene *, 3> hud_sub_scenes{&mode_indicator, &compass, &clock};
};

} // namespace game
} // namespace scenes
} // namespace ge
