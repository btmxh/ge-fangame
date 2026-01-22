#include "ge-app/font.hpp"

#include "assets/src/fonts/Pixeloid/TTF/PixeloidSans-Bold_9px.h"
#include "assets/src/fonts/Pixeloid/TTF/PixeloidSans_18px.h"
#include "assets/src/fonts/Pixeloid/TTF/PixeloidSans_9px.h"

namespace ge {
const Font &Font::regular_font() {
  static Font font{font_pixeloid_9px,
                   font_pixeloid_9px_CELL_WIDTH,
                   font_pixeloid_9px_CELL_HEIGHT,
                   font_pixeloid_9px_FIRST_CHAR,
                   font_pixeloid_9px_LAST_CHAR,
                   font_pixeloid_9px_BYTES_PER_ROW,
                   font_pixeloid_9px_ADVANCES};
  return font;
}

const Font &Font::bold_font() {
  static Font font{font_pixeloid_9px_bold,
                   font_pixeloid_9px_bold_CELL_WIDTH,
                   font_pixeloid_9px_bold_CELL_HEIGHT,
                   font_pixeloid_9px_bold_FIRST_CHAR,
                   font_pixeloid_9px_bold_LAST_CHAR,
                   font_pixeloid_9px_bold_BYTES_PER_ROW,
                   font_pixeloid_9px_bold_ADVANCES};
  return font;
}

const Font &Font::big_font() {
  static Font font{font_pixeloid_18px,
                   font_pixeloid_18px_CELL_WIDTH,
                   font_pixeloid_18px_CELL_HEIGHT,
                   font_pixeloid_18px_FIRST_CHAR,
                   font_pixeloid_18px_LAST_CHAR,
                   font_pixeloid_18px_BYTES_PER_ROW,
                   font_pixeloid_18px_ADVANCES};
  return font;
}

bool Font::get_glyph(char c, u8 const *&glyph_data, u8 &glyph_w, u8 &glyph_h,
                     u8 &advance) const {
  if (c < first_char || c > last_char)
    return false;

  u32 idx = c - first_char;
  glyph_data = &this->glyph_data[idx * glyph_height * glyph_bytes_per_row];
  glyph_w = this->glyph_width;
  glyph_h = this->glyph_height;
  advance = this->glyph_advances[idx];
  return true;
}

u32 Font::line_height() const { return static_cast<u32>(glyph_height); }

u32 Font::default_advance() const {
  assert(first_char <= ' ' && ' ' <= last_char);
  return glyph_advances[' ' - first_char];
}

u32 Font::get_glyph_width(char c) const {
  if (c < first_char || c > last_char)
    return default_advance();
  return glyph_advances[c - first_char];
}

} // namespace ge
