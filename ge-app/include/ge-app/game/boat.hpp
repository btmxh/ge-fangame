#pragma once

#include "default-boat.h"
#include "ge-app/texture.hpp"
#include "ge-hal/app.hpp"
#include <cmath>
#include <iostream>
namespace ge {
class Boat {
public:
  void render(Surface &region) {
    auto angle_to_8dir = [](float angle) {
      // normalize to [0, 2π)
      angle = std::fmod(angle, 2.0f * M_PI);
      if (angle < 0)
        angle += 2.0f * M_PI;

      // add half-sector to round correctly
      angle += M_PI / 8.0f;

      // map to 0..7
      int dir = int(angle / (M_PI / 4.0f)) & 7;
      return dir;
    };

    float angle = -angle_to_8dir(get_relative_angle()) * M_PI_4;
    boat.blit_rotated(region, region.get_width() / 2,
                      region.get_height() / 2, angle);
  }

  float get_angle() const { return angle; }
  float get_relative_angle() const { return angle - default_angle; }

  void update_angle(float joystick_x, float joystick_y, float delta_time) {
    auto angle_diff = [](float from, float to) {
      float d = std::fmod(to - from + M_PI, 2.0f * M_PI);
      if (d < 0)
        d += 2.0f * M_PI;
      return d - M_PI;
    };

    float x = joystick_x, y = joystick_y;
    float mag = std::sqrt(x * x + y * y);

    if (mag > 0.0f) {
      float target = std::atan2(y, x);
      float diff = angle_diff(angle, target);
      float max_step = turn_rate * delta_time;
      diff = std::max(std::min(diff, max_step), -max_step);
      angle += diff;
    }

    angle = std::fmod(angle, 2.0f * M_PI);
  }

  void update_position(App &app, float delta_time) {
    const auto angle = get_angle();
    x += boat_speed * delta_time * std::cos(angle);
    y += boat_speed * delta_time * std::sin(angle);
    app.log("Boat position: (%.2f, %.2f)", x, y);
  }

  float get_x() const { return x; }
  float get_y() const { return y; }

private:
  Texture boat{
      default_boat_color,
      default_boat_alpha,
      default_boat_width,
      default_boat_height,
  };

  static constexpr float default_angle = M_PI_2, turn_rate = 1.5f,
                         boat_speed = 1.0; // 1m/s
  float angle = default_angle;

  float x = 0.0, y = 0.0;
};
} // namespace ge
