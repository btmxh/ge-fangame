
#pragma once

#include "ge-app/game/water.hpp"
#include "ge-app/scenes/base.hpp"

namespace ge {
namespace scenes {
namespace game {
class WorldScene;
namespace world {
class WaterScene : public Scene {
public:
  WaterScene(WorldScene &parent);

  void render(Surface &fb_region) override;

private:
  WorldScene &parent;
  Water water;
};
} // namespace world
} // namespace game
} // namespace scenes
} // namespace ge
