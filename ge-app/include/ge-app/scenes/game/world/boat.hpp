#pragma once

#include "ge-app/game/boat.hpp"
#include "ge-app/scenes/base.hpp"
#include <array>

namespace ge {
namespace scenes {
namespace game {
class WorldScene;
namespace world {

class BoatScene;

class BoatUpdateScene : public Scene {
public:
  BoatUpdateScene(BoatScene &parent);

  void render(Surface &fb_region) override;
  void tick(float dt) override;

private:
  BoatScene &parent;
};

class BoatSteeringScene : public Scene {
public:
  BoatSteeringScene(BoatScene &parent);

  bool on_joystick_moved(float dt, float x, float y) override;
  bool on_button_held(Button btn) override;
  bool on_button_finished_hold(Button btn) override;

  void on_exit();
  bool is_active() const override;

private:
  BoatScene &parent;
};

class BoatScene : public ContainerScene {
public:
  BoatScene(WorldScene &parent);

  void on_exit() { boat_steering_scene.on_exit(); }

  Boat &get_boat() { return boat; }

private:
  WorldScene &parent;
  Boat boat;
  bool is_accelerating = false;

  BoatUpdateScene boat_update_scene;
  BoatSteeringScene boat_steering_scene;
  std::array<Scene *, 2> subscenes = {
      &boat_update_scene,
      &boat_steering_scene,
  };

  friend class BoatUpdateScene;
  friend class BoatSteeringScene;
};
} // namespace world
} // namespace game
} // namespace scenes
} // namespace ge
