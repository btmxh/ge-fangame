#pragma once

#include "ge-app/game/compass.hpp"
#include "ge-app/scenes/base.hpp"

namespace ge {
namespace scenes {
namespace game {
class HUDScene;
namespace hud {

class CompassScene : public Scene {
public:
  CompassScene(HUDScene &parent);

  void render(Surface &fb_region) override;

private:
  HUDScene &parent;
  Compass compass;
};

} // namespace hud
} // namespace game
} // namespace scenes
} // namespace ge
