#pragma once

#include "ge-app/scenes/base.hpp"

namespace ge {
namespace scenes {
class MenuScene;
namespace menu {
class BGMScene : public Scene {
public:
  BGMScene(MenuScene &parent);

  void on_enter();
  void on_exit();

private:
  MenuScene &parent;
};
} // namespace menu
} // namespace scenes
} // namespace ge
