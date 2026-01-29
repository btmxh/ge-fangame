#pragma once
#include "ge-app/scenes/base.hpp"

namespace ge {
namespace scenes {
class GameScene;
namespace game {
class GameOverScene : public Scene {
public:
  GameOverScene(GameScene &parent);

  void render(Surface &fb_region) override;
  bool on_button_clicked(Button btn) override;

  bool is_active() const override;

private:
  GameScene &parent;
};
} // namespace game
} // namespace scenes
} // namespace ge
