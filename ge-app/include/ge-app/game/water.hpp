#pragma once

#include "ge-app/gfx/color.hpp"
#include "ge-hal/gpu.hpp"
#include "ge-hal/surface.hpp"
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

  void render(Surface region, float time) {
    (void)time;

    hal::gpu::fill(region, sky_color);
  }

private:
  std::uint16_t water_color = 0x03E0; // initial water color (greenish)
  std::uint16_t sky_color = 0xFFFF;
};
} // namespace ge
