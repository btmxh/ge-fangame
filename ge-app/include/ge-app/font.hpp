#pragma once

#include "ge-hal/surface.hpp"
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
  Font(const u8 *glyph_data, u8 glyph_width, u8 glyph_height, char first_char,
       char last_char, u8 glyph_byte_per_row, const u8 *glyph_advances)
      : glyph_data(glyph_data), glyph_width(glyph_width),
        glyph_height(glyph_height), first_char(first_char),
        last_char(last_char), glyph_bytes_per_row(glyph_byte_per_row),
        glyph_advances(glyph_advances) {}

  static const Font &regular_font();
  static const Font &bold_font();
  static const Font &big_font();

  bool get_glyph(char c, u8 const *&glyph_data, u8 &glyph_w, u8 &glyph_h,
                 u8 &advance) const;

  u32 line_height() const;
  u32 default_advance() const;

  template <class ColorCallback>
  void render(const char *text, Surface region, int x, int y,
              ColorCallback cb) {
    auto set_pixel = [&, x, y](int dx, int dy, std::uint16_t color) {
      region.set_pixel(x + dx, y + dy, color);
    };

    x = y = 0;
    for (auto ch = *text; ch; ch = *++text) {
      if (ch == '\n') {
        x = 0, y += line_height();
        continue;
      }

      u8 const *glyph_data;
      u8 glyph_w, glyph_h, advance;
      if (!get_glyph(ch, glyph_data, glyph_w, glyph_h, advance)) {
        x += default_advance();
        continue;
      }

      for (int gy = 0; gy < glyph_h; ++gy) {
        for (int gx = 0; gx < glyph_w; ++gx) {
          int bit = gy * glyph_w + gx;
          bool pixel_on = (glyph_data[bit >> 3] >> (7 - (bit & 0x7))) & 1;
          if (pixel_on) {
            auto color = cb(GlyphContext{ch, gx, gy, glyph_w, glyph_h, x, y});
            set_pixel(x + gx, y + gy, color);
          }
        }
      }

      x += advance;
    }
  }

  void render_colored(const char *text, Surface region, int x, int y,
                      std::uint16_t color) {
    render(text, region, x, y, [color](const GlyphContext &) { return color; });
  }

private:
  const u8 *glyph_data, *glyph_advances;
  u8 glyph_width, glyph_height, glyph_bytes_per_row;
  char first_char, last_char;
};
} // namespace ge
