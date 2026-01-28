#include "ge-app/scenes/game/hud/compass.hpp"
#include "ge-app/scenes/game/hud/main.hpp"
#include "ge-app/scenes/game/world/main.hpp"

namespace ge {
namespace scenes {
namespace game {
namespace hud {

CompassScene::CompassScene(HUDScene &parent)
    : Scene(parent.get_app()), parent(parent) {}

void CompassScene::render(Surface &fb_region) {
  auto compass_region =
      fb_region.subsurface(ge::App::WIDTH - compass.get_width() - 10, 10,
                           compass.get_width(), compass.get_height());
  compass.render(compass_region,
                 parent.get_world_scene().get_boat().get_relative_angle());
}

} // namespace hud
} // namespace game
} // namespace scenes
} // namespace ge
