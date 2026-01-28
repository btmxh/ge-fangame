#pragma once

#include "ge-app/scenes/base.hpp"

namespace ge {
namespace scenes {
class GameScene;
namespace game {
class BGMScene : public Scene {
public:
  BGMScene(GameScene &parent);

  void on_enter();
  void on_exit();

private:
  GameScene &parent;
};
} // namespace game
} // namespace scenes
} // namespace ge
