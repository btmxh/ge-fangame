#include "ge-app/scenes/game/world/sky.hpp"
#include "ge-app/scenes/game/world/main.hpp"

namespace ge {
namespace scenes {
namespace game {
namespace world {

SkyScene::SkyScene(WorldScene &parent)
    : Scene(parent.get_app()), parent{parent} {}

void SkyScene::render(Surface &fb_region) {
  auto &clock = parent.get_clock();

  sky.render(app, parent.sky_region(fb_region), parent.get_clock());
}

} // namespace world
} // namespace game
} // namespace scenes
} // namespace ge
