#pragma once

#include "ge-app/game/clock.hpp"
#include "ge-app/game/player_stats.hpp"
#include "ge-app/scenes/base.hpp"
#include "ge-app/timer.hpp"

namespace ge {
namespace scenes {
namespace game {
class WorldScene;
namespace world {
class TimeUpdateScene : public Scene {
public:
  TimeUpdateScene(WorldScene &parent);

  void tick(float dt) override;

  Clock &get_clock() { return clock; }
  PlayerStats &get_player_stats() { return player_stats; }

private:
  WorldScene &parent;

  i64 last_frame_world_time = -1;

  Clock clock;
  PlayerStats player_stats;
};
} // namespace world
} // namespace game
} // namespace scenes
} // namespace ge
