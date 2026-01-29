#pragma once

#include "ge-app/scenes/base.hpp"

namespace ge {
namespace scenes {
namespace game {
class HUDScene;
namespace hud {

class YHUDScene : public Scene {
public:
  YHUDScene(HUDScene &parent);

  void render(Surface &fb_region) override;

private:
  HUDScene &parent;
};

} // namespace hud
} // namespace game
} // namespace scenes
} // namespace ge
