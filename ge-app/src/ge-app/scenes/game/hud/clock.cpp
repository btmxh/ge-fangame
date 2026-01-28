#include "ge-app/scenes/game/hud/clock.hpp"
#include "ge-app/scenes/game/hud/main.hpp"

namespace ge {
namespace scenes {
namespace game {
namespace hud {

ClockScene::ClockScene(HUDScene &parent)
    : Scene(parent.get_app()), parent(parent) {}

void ClockScene::render(Surface &fb_region) {
  auto clock_region = fb_region.subsurface(10, 10, 120, 16);
  auto &clock = parent.get_clock();
  clock.render(app, clock_region);
}

} // namespace hud
} // namespace game
} // namespace scenes
} // namespace ge
