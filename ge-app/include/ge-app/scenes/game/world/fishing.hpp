#pragma once

#include <array>

#include "ge-app/game/fishing.hpp"
#include "ge-app/scenes/base.hpp"

namespace ge {
namespace scenes {
namespace game {
class WorldScene;
namespace world {

class FishingScene;

class FishingUpdateScene : public Scene {
public:
  FishingUpdateScene(FishingScene &parent);

  void tick(float dt) override;
  void render(Surface &fb_region) override;

private:
  FishingScene &parent;
};

class FishingInputScene : public Scene {
public:
  FishingInputScene(FishingScene &parent);

  bool on_button_clicked(Button btn) override;
  bool on_joystick_moved(float dt, float x, float y) override;

  bool is_active() const override;

private:
  FishingScene &parent;
};

class FishingScene : public ContainerScene {
public:
  FishingScene(WorldScene &parent);

private:
  WorldScene &parent;

  Fishing fishing;
  FishingUpdateScene fishing_update_scene;
  FishingInputScene fishing_input_scene;
  std::array<Scene *, 2> subscenes = {
      &fishing_update_scene,
      &fishing_input_scene,
  };

  friend class FishingUpdateScene;
  friend class FishingInputScene;
};

} // namespace world
} // namespace game
} // namespace scenes
} // namespace ge
