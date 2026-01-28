#pragma once

#include "ge-app/texture.hpp"

#include "assets/out/textures/compass-base.h"
#include "assets/out/textures/compass-needle.h"

namespace ge {
class Compass {
public:
  u32 get_width() const { return base.get_width(); }
  u32 get_height() const { return base.get_height(); }
  void render(Surface &region, float angle) {
    static const float needle_angle_offset = 0;
    base.blit(region);
    // NOTE: these pixel offsets are chosen manually based on the texture design
    needle.blit_rotated(region, 24, 28, needle_angle_offset - angle, 23.5,
                        28.5);
  }

private:
  Texture<compass_base_FORMAT_CPP> base{compass_base, compass_base_WIDTH,
                                        compass_base_HEIGHT};
  Texture<compass_needle_FORMAT_CPP> needle{
      compass_needle, compass_needle_WIDTH, compass_needle_HEIGHT};
};
} // namespace ge
