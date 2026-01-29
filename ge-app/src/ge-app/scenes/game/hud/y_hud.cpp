#include "ge-app/scenes/game/hud/y_hud.hpp"

#include "assets/out/textures/sign.h"
#include "ge-app/scenes/game/hud/main.hpp"
#include "ge-app/scenes/game/world/main.hpp"

namespace ge {
namespace scenes {
namespace game {
namespace hud {

YHUDScene::YHUDScene(HUDScene &parent)
    : Scene(parent.get_app()), parent(parent) {}

static void format_distance(u32 meters, char *out, size_t out_size) {
  if (meters < 1000) {
    std::snprintf(out, out_size, "%u m", meters);
  } else {
    u32 km = meters / 1000;
    u32 dec = (meters % 1000) / 100; // 1 decimal

    std::snprintf(out, out_size, "%u.%u km", km, dec);
  }
}

void YHUDScene::render(Surface &fbr) {
  // auto mode_indicator_region = fb_region.subsurface(10, 30, 120, 16);
  // indicator.render(mode_indicator_region);
  static TextureARGB8888 sign_texture{sign, sign_WIDTH, sign_HEIGHT,
                                      sign_FORMAT_CPP};
  auto fb_region = fbr.subsurface(sign_WIDTH * 2, 0, sign_WIDTH, sign_HEIGHT);
  sign_texture.blit(fb_region);

  auto &clock = parent.get_clock();
  auto &font = Font::regular_font();
  auto width = font.text_width("Current Y", -1);
  auto x = (sign_WIDTH - width) / 2;
  font.render_colored("Current Y", -1, fb_region, x, 20, 0xFFFF);

  auto bota_y = parent.get_world_scene().get_boat().get_y();
  char y_str[16];
  format_distance(bota_y, y_str, sizeof(y_str));

  auto bold_font = Font::bold_font();
  width = bold_font.text_width(y_str, -1);
  x = (sign_WIDTH - width) / 2 - 1;
  bold_font.render_colored(y_str, -1, fb_region, x, 36, 0xFFFF);
}
} // namespace hud
} // namespace game
} // namespace scenes
} // namespace ge
