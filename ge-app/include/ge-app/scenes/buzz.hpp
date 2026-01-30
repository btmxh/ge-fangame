#pragma once

#include "ge-app/scenes/base.hpp"

#ifdef GE_HAL_STM32
#include "ge-hal/stm/gpio.hpp"
#endif

namespace ge {
namespace scenes {
class RootScene;
class BuzzScene : public Scene {
public:
  BuzzScene(RootScene &parent);

  void buzz_for(i64 time);

  void tick(float dt) override;
  bool on_button_clicked(Button btn) override {
    buzz_for(50);
    return false;
  }

private:
  RootScene &parent;
#ifdef GE_HAL_STM32
  hal::stm::Pin buzzer_pin{'B', 12};
#endif
  i64 buzz_until = -1;
};
} // namespace scenes
} // namespace ge
