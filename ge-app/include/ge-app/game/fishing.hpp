#pragma once

#include "ge-app/texture.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/gpu.hpp"
#include <cmath>
#include <cstdlib>

namespace ge {

enum class FishingState {
  Idle,       // Not fishing
  Casting,    // Joystick flick animation
  Fishing,    // Line in water, waiting for fish
  FishBiting, // Fish is biting, wiggle intensifies
  Caught      // Fish caught, ready to reel in
};

class Fishing {
public:
  Fishing() : state(FishingState::Idle) {}

  void update(App &app, float dt, const JoystickState &joystick) {
    switch (state) {
    case FishingState::Idle:
      update_idle(joystick, dt);
      break;
    case FishingState::Casting:
      update_casting(dt);
      break;
    case FishingState::Fishing:
      update_fishing(app, dt);
      break;
    case FishingState::FishBiting:
      update_fish_biting(app, dt);
      break;
    case FishingState::Caught:
      // Stay in this state until player presses button
      break;
    }

    // Update wiggle animation
    wiggle_time += dt;
  }

  void render(Surface &region, i32 boat_center_x, i32 boat_center_y) {
    if (state == FishingState::Idle) {
      return; // Nothing to render
    }

    // Calculate bobber position with wiggle
    float wiggle_x = std::sin(wiggle_time * wiggle_freq) * wiggle_amplitude;
    float wiggle_y = std::cos(wiggle_time * wiggle_freq * 0.7f) * wiggle_amplitude;
    
    i32 bobber_x = cast_x + static_cast<i32>(wiggle_x);
    i32 bobber_y = cast_y + static_cast<i32>(wiggle_y);

    // Add floating effect (up and down motion)
    float float_offset = std::sin(wiggle_time * 2.0f) * 2.0f;
    bobber_y += static_cast<i32>(float_offset);

    // Draw fishing line from boat to bobber
    draw_line(region, boat_center_x, boat_center_y, bobber_x, bobber_y,
              0x07FF); // Cyan color for fishing line

    // Draw bobber (3x3 blob)
    draw_bobber(region, bobber_x, bobber_y);
  }

  void on_button_clicked(App &app, Button btn) {
    if (btn == Button::Button1 && state == FishingState::Caught) {
      // Catch the fish!
      catch_fish(app);
      reset();
    }
  }

  bool is_active() const { return state != FishingState::Idle; }

  FishingState get_state() const { return state; }

private:
  // Constants
  static constexpr float MIN_DELTA_TIME = 0.001f;
  static constexpr float FLICK_THRESHOLD = 3.0f;
  static constexpr float MIN_JOYSTICK_MAG = 0.4f;
  static constexpr float CAST_DURATION = 0.5f;
  static constexpr float BITE_CHANCE_PER_SECOND = 0.3f;
  static constexpr float MIN_FISHING_TIME = 2.0f;
  static constexpr float MAX_FISHING_TIME = 15.0f;
  static constexpr float BITE_WINDOW = 3.0f;
  static constexpr float CATCH_REACTION_TIME = 0.5f;

  void update_idle(const JoystickState &joystick, float dt) {
    // Detect joystick flick
    float mag = std::sqrt(joystick.x * joystick.x + joystick.y * joystick.y);
    
    // Track previous joystick state to detect velocity
    if (dt > MIN_DELTA_TIME) { // Avoid division by very small numbers
      float velocity = (mag - prev_joystick_mag) / dt;
      
      // If joystick moved fast enough, start casting
      if (velocity > FLICK_THRESHOLD && mag > MIN_JOYSTICK_MAG) {
        start_cast(joystick.x, joystick.y, mag);
      }
    }
    
    prev_joystick_mag = mag;
  }

  void start_cast(float joy_x, float joy_y, float strength) {
    state = FishingState::Casting;
    casting_timer = 0.0f;

    // Calculate cast direction and distance
    float angle = std::atan2(joy_y, joy_x);
    // Use joystick magnitude as strength (0-1 range typically)
    float distance = strength * 60.0f; // Scale to reasonable pixel range
    distance = std::min(distance, 80.0f); // Cap at 80 pixels

    // Position relative to boat (center of screen)
    cast_x = static_cast<i32>(std::cos(angle) * distance);
    cast_y = static_cast<i32>(std::sin(angle) * distance);
  }

  void update_casting(float dt) {
    casting_timer += dt;
    
    if (casting_timer >= CAST_DURATION) {
      state = FishingState::Fishing;
      fishing_timer = 0.0f;
      wiggle_amplitude = 1.0f; // Small wiggle while waiting
      wiggle_freq = 2.0f;
    }
  }

  void update_fishing(App &app, float dt) {
    fishing_timer += dt;

    // Random chance for fish to bite (check every frame)
    if (fishing_timer > MIN_FISHING_TIME) {
      float bite_chance = BITE_CHANCE_PER_SECOND * dt;
      float random_value = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
      
      if (random_value < bite_chance) {
        // Fish is biting!
        state = FishingState::FishBiting;
        fish_bite_timer = 0.0f;
        wiggle_amplitude = 5.0f; // Increased wiggle
        wiggle_freq = 8.0f;      // Faster wiggle
        app.log("Fish is biting! Press A to catch it!");
      }
    }

    // If fishing too long without bite, reset
    if (fishing_timer > MAX_FISHING_TIME) {
      app.log("The fish got away. Recast your line!");
      reset();
    }
  }

  void update_fish_biting(App &app, float dt) {
    fish_bite_timer += dt;

    // Player has limited time to catch
    if (fish_bite_timer > BITE_WINDOW) {
      app.log("The fish got away! You were too slow.");
      reset();
    } else if (fish_bite_timer > CATCH_REACTION_TIME) {
      // Allow catching after a small delay (reaction time needed)
      state = FishingState::Caught;
    }
  }

  void catch_fish(App &app) {
    // Random fish names
    const char *fish_names[] = {
        "Tropical Fish",
        "Golden Fish (RARE!)",
        "Sea Bass",
        "Tuna",
        "Old Boot (loot)",
        "Treasure Chest (RARE loot!)",
        "Salmon",
        "Pufferfish",
        "Clownfish",
        "Sardine"
    };
    
    constexpr int num_fish = sizeof(fish_names) / sizeof(fish_names[0]);
    int caught_index = rand() % num_fish;
    
    app.log("You caught: %s", fish_names[caught_index]);
  }

  void reset() {
    state = FishingState::Idle;
    casting_timer = 0.0f;
    fishing_timer = 0.0f;
    fish_bite_timer = 0.0f;
    wiggle_time = 0.0f;
    prev_joystick_mag = 0.0f;
  }

  void draw_line(Surface &region, i32 x0, i32 y0, i32 x1, i32 y1,
                 u16 color) {
    // Bresenham's line algorithm
    i32 dx = std::abs(x1 - x0);
    i32 dy = std::abs(y1 - y0);
    i32 sx = x0 < x1 ? 1 : -1;
    i32 sy = y0 < y1 ? 1 : -1;
    i32 err = dx - dy;

    while (true) {
      // Draw pixel (convert from center coordinates to screen coordinates)
      i32 screen_x = x0 + region.get_width() / 2;
      i32 screen_y = y0 + region.get_height() / 2;
      
      if (screen_x >= 0 && screen_x < static_cast<i32>(region.get_width()) &&
          screen_y >= 0 && screen_y < static_cast<i32>(region.get_height())) {
        region.set_pixel<u16>(screen_x, screen_y, color);
      }

      if (x0 == x1 && y0 == y1)
        break;

      i32 e2 = 2 * err;
      if (e2 > -dy) {
        err -= dy;
        x0 += sx;
      }
      if (e2 < dx) {
        err += dx;
        y0 += sy;
      }
    }
  }

  void draw_bobber(Surface &region, i32 x, i32 y) {
    // Draw a 3x3 bobber (red color)
    const u16 BOBBER_COLOR = 0xF800; // Red in RGB565
    
    // Convert to screen coordinates
    i32 screen_x = x + region.get_width() / 2;
    i32 screen_y = y + region.get_height() / 2;

    // Draw 3x3 square
    for (i32 dy = -1; dy <= 1; dy++) {
      for (i32 dx = -1; dx <= 1; dx++) {
        i32 px = screen_x + dx;
        i32 py = screen_y + dy;
        if (px >= 0 && px < static_cast<i32>(region.get_width()) &&
            py >= 0 && py < static_cast<i32>(region.get_height())) {
          region.set_pixel<u16>(px, py, BOBBER_COLOR);
        }
      }
    }
  }

  FishingState state;
  
  // Cast parameters
  i32 cast_x = 0;
  i32 cast_y = 0;
  
  // Timers
  float casting_timer = 0.0f;
  float fishing_timer = 0.0f;
  float fish_bite_timer = 0.0f;
  float wiggle_time = 0.0f;
  
  // Wiggle parameters
  float wiggle_amplitude = 1.0f;
  float wiggle_freq = 2.0f;
  
  // Joystick tracking
  float prev_joystick_mag = 0.0f;
};

} // namespace ge
