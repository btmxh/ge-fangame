#include "ge-app/text.hpp"

#include "font_proggy.h"

namespace ge {

bool Font::get_glyph(char c, const std::uint8_t **out_bitmap, int *out_width,
                     int *out_height) const {
  static constexpr int FONT_GLYPH_BYTES = Font::GLYPH_W * Font::GLYPH_H / 8;
  // TODO: this needs to match the font used in font_proggy.h
  static constexpr int FONT_GLYPH_START = 32;
  static constexpr int FONT_GLYPH_END = 126;

  if (c < FONT_GLYPH_START || c > FONT_GLYPH_END)
    return false;

  int index = c - FONT_GLYPH_START;
  if (out_bitmap)
    *out_bitmap = &font_proggy[index * FONT_GLYPH_BYTES];
  if (out_width)
    *out_width = GLYPH_W;
  if (out_height)
    *out_height = GLYPH_H;
  return true;
}

} // namespace ge
