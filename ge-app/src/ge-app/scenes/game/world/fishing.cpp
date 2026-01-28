#include "ge-app/scenes/game/world/fishing.hpp"

#include "ge-app/scenes/game/world/main.hpp"

namespace ge {
namespace scenes {
namespace game {
namespace world {

FishingUpdateScene::FishingUpdateScene(FishingScene &parent)
    : Scene(parent.get_app()), parent(parent) {}

void FishingUpdateScene::tick(float dt) {
  auto &world = parent.parent;
  parent.fishing.update(app, world.get_inventory(), world.get_dialog_scene(),
                        world.get_world_dt());
}

void FishingUpdateScene::render(Surface &fb_region) {
  auto &world = parent.parent;
  auto &boat = world.get_boat();
  auto water_region = world.water_region(fb_region);
  parent.fishing.render(water_region, 0, 0);
}

FishingInputScene::FishingInputScene(FishingScene &parent)
    : Scene(parent.get_app()), parent(parent) {}

bool FishingInputScene::on_button_clicked(Button btn) {
  auto &world = parent.parent;
  return parent.fishing.on_button_clicked(app, world.get_dialog_scene(), btn);
}

bool FishingInputScene::on_joystick_moved(float dt, float x, float y) {
  parent.fishing.on_joystick_moved(dt, x, y);
  return true;
}

bool FishingInputScene::is_active() const {
  return parent.parent.get_current_mode() == GameMode::Fishing;
}

FishingScene::FishingScene(WorldScene &parent)
    : ContainerScene(parent.get_app()), parent(parent),
      fishing_update_scene(*this), fishing_input_scene(*this) {
  set_scenes(subscenes);
}

void FishingScene::on_mode_changed(GameMode old_mode, GameMode new_mode) {
  if (old_mode == new_mode)
    return;
  if (new_mode != GameMode::Fishing) {
    fishing.reel_if_fishing(app, parent.get_dialog_scene());
    fishing.lose_focus();
  }
}
} // namespace world
} // namespace game
} // namespace scenes
} // namespace ge
