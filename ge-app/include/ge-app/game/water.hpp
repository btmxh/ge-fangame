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
  Water(FramebufferRegion region) : region(region) {}

  void set_water_color(std::uint16_t color) { water_color = color; }
  void set_sky_color(std::uint16_t color) { sky_color = color; }

  static inline uint32_t hash2(uint32_t x, uint32_t y) {
    uint32_t h = x * 0x8da6b343u ^ y * 0xd8163841u;
    h ^= h >> 13;
    h *= 0x85ebca6bu;
    return h;
  }

  void render(float time) {
    int W = region.region_width(), H = region.region_height();
    float t = time;

    float f1 = 6.78549854395f, s1 = 1.548953589f;
    float f2 = 1.384593458f, s2 = -1.084594359f;

    uint16_t sun_color = hsv_to_rgb565(40, 30, 255); // warm yellow

    for (int y = 0; y < H; ++y) {
      float d = (float)y / H; // distance factor
      float d2 = d * d;       // nonlinear perspective

      float ty = 9000.0f / (y + 100);

      for (int x = 0; x < W; ++x) {

        // ---- waves (geometry only) ----
        float px = x * 0.0 * ty;
        float w = sinf(ty * f1 + px + t * s1) +
                  0.5f * sinf(ty * f2 + px + t * s2 + 13.0f);

        float nx = w * 2.5f;

        float micro = sinf(ty * 4.3434f + x * 0.3438495 + t * 0.5f);
        nx += micro * 0.15f;

        nx *= d2; // flatten toward horizon

        // ---- normalize ----
        float nz = 1.0f / sqrtf(nx * nx + 1.0f);
        nx *= nz;

        // ---- fresnel (EARLY quantized) ----
        float fresnel = 1.0f - nz;
        fresnel = fresnel * fresnel;

        // quantize to RGB565-friendly steps
        fresnel = floorf(fresnel * 32.0f) * (1.0f / 32.0f);

        // decorrelate rows
        float jitter = (hash2(x, y) & 0x3F) * (1.0f / 255.0f);
        fresnel = std::clamp(fresnel + jitter, 0.0f, 1.0f);

        uint16_t color =
            blend_rgb565(water_color, sky_color, (uint8_t)(fresnel * 255));

        // ---- sparkle ----
        float spec = nx * 0.3f + nz * 0.95f;
        spec = std::max(spec, 0.0f);
        spec = spec * spec * spec * spec;

        float sparkle = (spec - 0.92f) / (0.99f - 0.92f);
        sparkle = std::clamp(sparkle, 0.0f, 1.0f);
        sparkle *= d2;

        // quantize sparkle (temporal stability)
        uint8_t a = (uint8_t)(sparkle * 6) * 8;

        color = add_rgb565(color, sun_color, a);

        region.set_pixel(x, y, color);
      }
    }
  }

private:
  FramebufferRegion region;
  std::uint16_t water_color = 0x03E0; // initial water color (greenish)
  std::uint16_t sky_color = 0xFFFF;
};
} // namespace ge
