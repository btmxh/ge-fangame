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
    static const float needle_angle_offset = M_PI_4;
    base.blit(region);
    // TODO: this is not pixel-perfect center alignment
    needle.blit_rotated(region, base.get_width() / 2, base.get_height() / 2,
                        needle_angle_offset - angle);
  }

private:
  Texture base{compass_base, compass_base_WIDTH, compass_base_HEIGHT};
  Texture needle{compass_needle, compass_needle_WIDTH, compass_needle_HEIGHT};
};
} // namespace ge
