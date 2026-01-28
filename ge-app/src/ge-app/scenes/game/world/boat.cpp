#include "ge-app/scenes/game/world/boat.hpp"
#include "ge-app/scenes/game/world/main.hpp"

namespace ge {
namespace scenes {
namespace game {
namespace world {

BoatUpdateScene::BoatUpdateScene(BoatScene &parent)
    : Scene(parent.get_app()), parent(parent) {}

void BoatUpdateScene::render(Surface &fb_region) {
  auto &world = parent.parent;
  auto max_side = std::max(parent.boat.get_width(), parent.boat.get_height());
  auto water_region = world.water_region(fb_region);
  auto boat_region = water_region.subsurface(
      (water_region.get_width() - max_side) / 2,
      (water_region.get_height() - max_side) / 2, max_side, max_side);
  parent.boat.render(boat_region);
}

void BoatUpdateScene::tick(float dt) {
  auto &world = parent.parent;
  parent.boat.update_position(app, world.get_world_dt(), parent.is_accelerating,
                              world.get_inventory().get_total_weight());
}

BoatSteeringScene::BoatSteeringScene(BoatScene &parent)
    : Scene(parent.get_app()), parent(parent) {}

bool BoatSteeringScene::on_joystick_moved(float dt, float x, float y) {
  (void)dt;
  auto &world = parent.parent;
  parent.boat.update_angle(x, y, world.get_world_dt());
  return true;
}

bool BoatSteeringScene::on_button_held(Button btn) {
  if (btn == Button::Button1) {
    parent.is_accelerating = true;
    return true;
  }

  return false;
}

bool BoatSteeringScene::on_button_finished_hold(Button btn) {
  if (btn == Button::Button1) {
    parent.is_accelerating = false;
    return true;
  }
  return false;
}

void BoatSteeringScene::on_exit() { parent.is_accelerating = false; }

bool BoatSteeringScene::is_active() const {
  return parent.parent.get_current_mode() == GameMode::Steering;
}

BoatScene::BoatScene(WorldScene &parent)
    : ContainerScene(parent.get_app()), parent(parent),
      boat_update_scene(*this), boat_steering_scene(*this) {
  set_scenes(subscenes);
}

} // namespace world
} // namespace game
} // namespace scenes
} // namespace ge
