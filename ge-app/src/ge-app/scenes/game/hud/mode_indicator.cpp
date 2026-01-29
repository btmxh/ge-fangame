#include "ge-app/scenes/game/hud/mode_indicator.hpp"

#include "assets/out/textures/sign.h"
#include "ge-app/scenes/game/hud/main.hpp"

namespace ge {
namespace scenes {
namespace game {
namespace hud {

ModeIndicatorScene::ModeIndicatorScene(HUDScene &parent)
    : Scene(parent.get_app()), parent(parent) {}

GameMode ModeIndicatorScene::switch_mode() {
  auto old_mode = get_current_mode();
  GameMode mode = indicator.switch_mode();
  parent.update_mode(old_mode, mode);
  return mode;
}

void ModeIndicatorScene::render(Surface &fbr) {
  // auto mode_indicator_region = fb_region.subsurface(10, 30, 120, 16);
  // indicator.render(mode_indicator_region);
  static TextureARGB8888 sign_texture{sign, sign_WIDTH, sign_HEIGHT,
                                      sign_FORMAT_CPP};
  auto fb_region = fbr.subsurface(sign_WIDTH, 0, sign_WIDTH, sign_HEIGHT);
  sign_texture.blit(fb_region);

  auto &clock = parent.get_clock();
  auto &font = Font::regular_font();
  auto width = font.text_width("Mode", -1);
  auto x = (sign_WIDTH - width) / 2;
  font.render_colored("Mode", -1, fb_region, x, 20, 0xFFFF);

  auto bold_font = Font::bold_font();
  auto hr = clock.get_hr(app);
  auto ampm = (hr >= 12) ? "PM" : "AM";
  hr = hr % 12;
  const char *mode_str = [&] {
    switch (get_current_mode()) {
    case GameMode::Steering:
      return "Steering";
    case GameMode::Fishing:
      return "Fishing";
    case GameMode::Management:
      return "Management";
    default:
      return "UNKNOWN";
    }
  }();
  width = font.text_width(mode_str, -1);
  x = (sign_WIDTH - width) / 2 - 1;
  font.render_colored(mode_str, -1, fb_region, x, 36, 0xFFFF);
}
} // namespace hud
} // namespace game
} // namespace scenes
} // namespace ge
