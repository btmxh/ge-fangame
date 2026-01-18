#pragma once

#include "ge-app/gfx/color.hpp"
#include "ge-hal/fb.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace std {
template <class T>
constexpr const T &clamp(const T &v, const T &lo, const T &hi) {
  return clamp(v, lo, hi, std::less<T>{});
}

template <class T, class Compare>
constexpr const T &clamp(const T &v, const T &lo, const T &hi, Compare comp) {
  return comp(v, lo) ? lo : comp(hi, v) ? hi : v;
}
} // namespace std

namespace ge {
class Water {
public:
  void set_water_color(std::uint16_t color) { water_color = color; }
  void set_sky_color(std::uint16_t color) { sky_color = color; }

  static inline uint32_t hash2(uint32_t x, uint32_t y) {
    uint32_t h = x * 0x8da6b343u ^ y * 0xd8163841u;
    h ^= h >> 13;
    h *= 0x85ebca6bu;
    return h;
  }

  void render(Surface region, float time) {
    (void)time;
    for (int y = 0; y < region.get_height(); ++y) {
      for (int x = 0; x < region.get_width(); ++x) {
        region.set_pixel(x, y, water_color); // base water color
      }
    }
  }

private:
  std::uint16_t water_color = 0x03E0; // initial water color (greenish)
  std::uint16_t sky_color = 0xFFFF;
};
} // namespace ge
