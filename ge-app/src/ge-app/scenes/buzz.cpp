#include "ge-app/scenes/buzz.hpp"
#include "ge-app/scenes/main.hpp"

namespace ge {
namespace scenes {
BuzzScene::BuzzScene(RootScene &parent)
    : Scene(parent.get_app()), parent{parent} {

#ifdef GE_HAL_STM32
  buzzer_pin.set_mode(hal::stm::GPIOMode::Output);
#endif
}

void BuzzScene::buzz_for(i64 time) {
#ifdef GE_HAL_STM32
  buzzer_pin.write(true);
#endif
  buzz_until = app.now() + time;
}

void BuzzScene::tick(float dt) {
  if (buzz_until >= 0 && app.now() >= buzz_until) {
#ifdef GE_HAL_STM32
    buzzer_pin.write(false);
#endif
  }
}

} // namespace scenes
} // namespace ge
