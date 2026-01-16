#pragma once

#include <cmath>
#include <ge-hal/app.hpp>

namespace ge {
class PerspectiveMapping {
public:
  // x, y are relative to boat position
  void transform_xy(float x, float y, float &out_x, float &out_y) {
    float scale = scale_at(y);
    out_x = region_width / 2 + x * pixel_per_length_unit;
    out_y = region_height / 2 - y * pixel_per_length_unit * scale;
  }

  float scale_at(float y) {
    return 1.0 / (1 + K * std::fmax(0, y * pixel_per_length_unit -
                                           region_height / 4));
  }

private:
  float K = 0.008, pixel_per_length_unit = 32.0f;
  float region_width = ge::App::WIDTH, region_height = ge::App::WIDTH;
};
} // namespace ge
