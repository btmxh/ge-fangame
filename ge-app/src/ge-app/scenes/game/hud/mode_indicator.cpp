#include "ge-app/scenes/game/hud/mode_indicator.hpp"

#include "ge-app/scenes/game/hud/main.hpp"

namespace ge {
namespace scenes {
namespace game {
namespace hud {

ModeIndicatorScene::ModeIndicatorScene(HUDScene &parent)
    : Scene(parent.get_app()), parent(parent) {}

GameMode ModeIndicatorScene::switch_mode() {
  GameMode mode = indicator.switch_mode();
  parent.update_mode(mode);
  return mode;
}

} // namespace hud
} // namespace game
} // namespace scenes
} // namespace ge
