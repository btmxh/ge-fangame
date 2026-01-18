#pragma once

#include "ge-hal/fb.hpp"
#include <cstdint>

namespace ge {

struct GlyphContext {
  char ch;
  int gx, gy; // glyph-local pixel (render space, includes scale)
  int gw, gh; // glyph dimensions in render space
  int x, y;   // glyph origin in layout space (unscaled)
};

// TODO: currently only supporting Proggy (mono 8x8 font)
// In the future, we might ship our own font instead of relying on external
// ones.
// This API can also support variable-width fonts in the future to save space.
class Font {
public:
  Font() = default;

  bool get_glyph(char c, const std::uint8_t **out_bitmap, int *out_width,
                 int *out_height) const;
  int line_height(int scale) const { return GLYPH_H * scale; }
  int default_advance(int scale) const { return GLYPH_W * scale; }

  template <class ColorCallback>
  void render(const char *text, Surface region, int x, int y,
              int scale, ColorCallback cb) {
    auto set_pixel = [&, x, y](int dx, int dy, std::uint16_t color) {
      int px = x + dx;
      int py = y + dy;
      if ((unsigned)px >= (unsigned)region.get_width() ||
          (unsigned)py >= (unsigned)region.get_height())
        return;
      region.set_pixel(px, py, color);
    };

    x = y = 0;
    for (; *text; ++text) {
      const char ch = *text;
      if (ch == '\n') {
        x = 0;
        y += line_height(scale);
        continue;
      }

      const std::uint8_t *glyph_bitmap;
      int glyph_w, glyph_h;
      if (!get_glyph(ch, &glyph_bitmap, &glyph_w, &glyph_h)) {
        x += default_advance(scale);
        continue;
      }

      for (int gy = 0; gy < glyph_h; ++gy) {
        for (int gx = 0; gx < glyph_w; ++gx) {
          int bit = gy * glyph_w + gx;
          bool pixel_on = (glyph_bitmap[bit >> 3] >> (7 - (bit & 0x7))) & 1;
          if (pixel_on) {
            for (int sy = 0; sy < scale; ++sy) {
              for (int sx = 0; sx < scale; ++sx) {
                std::uint16_t color =
                    cb(GlyphContext{ch, gx * scale + sx, gy * scale + sy,
                                    glyph_w * scale, glyph_h * scale, x, y});
                set_pixel(x + gx * scale + sx, y + gy * scale + sy, color);
              }
            }
          }
        }
      }

      x += glyph_w * scale;
    }
  }

  void render_colored(const char *text, Surface region, int x, int y,
                      int scale, std::uint16_t color) {
    render(text, region, x, y, scale,
           [color](const GlyphContext &) { return color; });
  }

private:
  static constexpr int GLYPH_W = 8, GLYPH_H = 12;
};
} // namespace ge
