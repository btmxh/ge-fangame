#pragma once

#include <cstdint>
namespace ge {
static inline uint16_t hsv_to_rgb565(uint8_t h, uint8_t s, uint8_t v) {
  uint8_t region = h / 43; // 0..5
  uint8_t remainder = (h - region * 43) * 6;

  uint8_t p = (v * (255 - s)) >> 8;
  uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;
  uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

  uint8_t r, g, b;

  switch (region) {
  case 0:
    r = v;
    g = t;
    b = p;
    break;
  case 1:
    r = q;
    g = v;
    b = p;
    break;
  case 2:
    r = p;
    g = v;
    b = t;
    break;
  case 3:
    r = p;
    g = q;
    b = v;
    break;
  case 4:
    r = t;
    g = p;
    b = v;
    break;
  default:
    r = v;
    g = p;
    b = q;
    break;
  }

  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

static inline uint16_t blend_rgb565(uint16_t a, uint16_t b, uint8_t t) {
  // t: 0..255, returns a*(1-t) + b*t
  uint16_t ar = (a >> 11) & 0x1F;
  uint16_t ag = (a >> 5) & 0x3F;
  uint16_t ab = a & 0x1F;

  uint16_t br = (b >> 11) & 0x1F;
  uint16_t bg = (b >> 5) & 0x3F;
  uint16_t bb = b & 0x1F;

  uint16_t r = (ar * (255 - t) + br * t) >> 8;
  uint16_t g = (ag * (255 - t) + bg * t) >> 8;
  uint16_t b2 = (ab * (255 - t) + bb * t) >> 8;

  return (r << 11) | (g << 5) | b2;
}

static inline uint16_t gray8_to_rgb565(uint8_t g) {
  uint16_t r = (g >> 3) & 0x1F;
  uint16_t g6 = (g >> 2) & 0x3F;
  uint16_t b = (g >> 3) & 0x1F;

  return (r << 11) | (g6 << 5) | b;
}

static inline uint16_t add_rgb565(uint16_t base, uint16_t light,
                                  uint8_t alpha /* 0..255 */) {
  if (alpha == 0)
    return base;
  if (alpha == 255) {
    // pure add with saturation
    uint32_t r = ((base >> 11) & 0x1F) + ((light >> 11) & 0x1F);
    uint32_t g = ((base >> 5) & 0x3F) + ((light >> 5) & 0x3F);
    uint32_t b = (base & 0x1F) + (light & 0x1F);

    if (r > 31)
      r = 31;
    if (g > 63)
      g = 63;
    if (b > 31)
      b = 31;

    return (r << 11) | (g << 5) | b;
  }

  // Extract channels
  uint32_t br = (base >> 11) & 0x1F;
  uint32_t bg = (base >> 5) & 0x3F;
  uint32_t bb = base & 0x1F;

  uint32_t lr = (light >> 11) & 0x1F;
  uint32_t lg = (light >> 5) & 0x3F;
  uint32_t lb = light & 0x1F;

  // Scale light by alpha (8.8 fixed)
  br += (lr * alpha) >> 8;
  bg += (lg * alpha) >> 8;
  bb += (lb * alpha) >> 8;

  // Saturate
  if (br > 31)
    br = 31;
  if (bg > 63)
    bg = 63;
  if (bb > 31)
    bb = 31;

  return (br << 11) | (bg << 5) | bb;
}

static inline uint16_t scale_rgb565(uint16_t color,
                                    uint8_t scale /* 0..255 */) {
  if (scale == 255)
    return color;
  if (scale == 0)
    return 0;

  // Extract channels
  uint32_t r = (color >> 11) & 0x1F;
  uint32_t g = (color >> 5) & 0x3F;
  uint32_t b = color & 0x1F;

  // Scale (8.8 fixed-point)
  r = (r * scale) >> 8;
  g = (g * scale) >> 8;
  b = (b * scale) >> 8;

  // No need to clamp (scaling down only)
  return (r << 11) | (g << 5) | b;
}

} // namespace ge
