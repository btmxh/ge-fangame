#include "ge-app/scenes/game/hud/clock.hpp"
#include "assets/out/textures/sign.h"
#include "ge-app/scenes/game/hud/main.hpp"
#include <cinttypes>

namespace ge {
namespace scenes {
namespace game {
namespace hud {

ClockScene::ClockScene(HUDScene &parent)
    : Scene(parent.get_app()), parent(parent) {}

void ClockScene::render(Surface &fb_region) {
  static TextureARGB8888 sign_texture{sign, sign_WIDTH, sign_HEIGHT,
                                      sign_FORMAT_CPP};
  sign_texture.blit(fb_region);

  auto &clock = parent.get_clock();
  char day[32];
  std::snprintf(day, sizeof day, "Day %" PRIu32, clock.get_num_days(app));
  auto &font = Font::regular_font();
  auto width = font.text_width(day, -1);
  auto x = (sign_WIDTH - width) / 2;
  font.render_colored(day, -1, fb_region, x, 20, 0xFFFF);

  auto &bold_font = Font::bold_font();
  auto hr = clock.get_hr(app);
  auto ampm = (hr >= 12) ? "PM" : "AM";
  hr = hr % 12;
  std::snprintf(day, sizeof day, "%02" PRIu32 " %s", hr == 0 ? 12 : hr, ampm);
  width = bold_font.text_width(day, -1);
  x = (sign_WIDTH - width) / 2;
  bold_font.render_colored(day, -1, fb_region, x, 36, 0xFFFF);
}

} // namespace hud
} // namespace game
} // namespace scenes
} // namespace ge
