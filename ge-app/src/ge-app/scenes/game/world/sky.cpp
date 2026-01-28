#include "ge-app/scenes/game/world/sky.hpp"
#include "ge-app/scenes/game/world/main.hpp"

namespace ge {
namespace scenes {
namespace game {
namespace world {

// ============================================================
// Sky color helpers (private to this TU)
// ============================================================
namespace {

constexpr u16 SKY_NIGHT_RGB = 0x0821;
constexpr u16 SKY_DAY_RGB = 0x5DBF;
constexpr u16 SKY_WARM_RGB = 0xF2C0;

inline float clamp01(float x) { return std::max(0.0f, std::min(1.0f, x)); }

inline float smooth01(float x) {
  x = clamp01(x);
  return x * x * (3.0f - 2.0f * x);
}

inline u16 blend_rgb565(uint16_t a, uint16_t b, float t) {
  t = clamp01(t);
  u8 ti = static_cast<u8>(t * 255.0f);
  return ge::blend_rgb565(a, b, ti);
}

} // namespace

inline u16 sky_color(float t) {
  t = std::fmod(t, 1.0f);
  if (t < 0)
    t += 1.0f;

  constexpr float SUNRISE_START = 0.23f;
  constexpr float SUNRISE_END = 0.27f;
  constexpr float SUNSET_START = 0.73f;
  constexpr float SUNSET_END = 0.77f;

  if (t >= SUNRISE_START && t < SUNRISE_END) {
    float k = smooth01((t - SUNRISE_START) / (SUNRISE_END - SUNRISE_START));
    return (k < 0.5f)
               ? blend_rgb565(SKY_NIGHT_RGB, SKY_WARM_RGB, k * 2.0f)
               : blend_rgb565(SKY_WARM_RGB, SKY_DAY_RGB, (k - 0.5f) * 2.0f);
  }

  if (t >= SUNSET_START && t < SUNSET_END) {
    float k = smooth01((t - SUNSET_START) / (SUNSET_END - SUNSET_START));
    return (k < 0.5f)
               ? blend_rgb565(SKY_DAY_RGB, SKY_WARM_RGB, k * 2.0f)
               : blend_rgb565(SKY_WARM_RGB, SKY_NIGHT_RGB, (k - 0.5f) * 2.0f);
  }

  if (t >= SUNRISE_END && t < SUNSET_START)
    return SKY_DAY_RGB;

  return SKY_NIGHT_RGB;
}

SkyScene::SkyScene(WorldScene &parent)
    : Scene(parent.get_app()), parent{parent} {}

void SkyScene::render(Surface &fb_region) {
  auto &clock = parent.get_clock();

  sky.set_x_offset(clock.get_game_timer().get(app) / 1000);
  sky.set_sky_color(sky_color(clock.time_in_day(app)));
  sky.render(parent.sky_region(fb_region));
}

} // namespace world
} // namespace game
} // namespace scenes
} // namespace ge
