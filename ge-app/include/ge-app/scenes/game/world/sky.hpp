
#pragma once

#include "ge-app/game/sky.hpp"
#include "ge-app/scenes/base.hpp"

namespace ge {
namespace scenes {
namespace game {
class WorldScene;
namespace world {
class SkyScene : public Scene {
public:
  SkyScene(WorldScene &parent);

  void render(Surface &fb_region) override;

private:
  WorldScene &parent;
  Sky sky;
};
} // namespace world
} // namespace game
} // namespace scenes
} // namespace ge
