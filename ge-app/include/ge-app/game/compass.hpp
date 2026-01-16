#pragma once

#include "ge-app/texture.hpp"

#include <cmath>
#include <compass-base.h>
#include <compass-needle.h>

namespace ge {
class Compass {
public:
  void render(FramebufferRegion &region, float angle) {
    static const float needle_angle_offset = M_PI_4f;
    base.blit(region);
    // TODO: this is not pixel-perfect center alignment
    needle.blit_rotated(region, compass_base_width / 2, compass_base_height / 2,
                        needle_angle_offset - angle);
  }

private:
  Texture base{compass_base_color, compass_base_alpha, compass_base_width,
               compass_base_height};
  Texture needle{compass_needle_color, compass_needle_alpha,
                 compass_needle_width, compass_needle_height};
};
} // namespace ge
