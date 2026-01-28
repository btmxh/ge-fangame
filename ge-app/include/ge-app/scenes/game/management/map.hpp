#pragma once

#include "ge-app/font.hpp"
#include "ge-app/game/boat.hpp"
#include "ge-app/game/clock.hpp"
#include "ge-app/scenes/base.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/gpu.hpp"
#include "ge-hal/surface.hpp"
#include <cstdio>
#include <cstring>

namespace ge {
namespace scenes {
namespace game {
class ManagementUIScene;
namespace mgmt {

struct Bookmark {
  char name[32];
  i32 x;
  i32 y;
  char icon;
  bool active;

  Bookmark() : x(0), y(0), icon('*'), active(false) { name[0] = '\0'; }
};

class MapScene : public Scene {
public:
  MapScene(ManagementUIScene &parent);

  void tick(float dt) override {
    auto joystick = app.get_joystick_state();

    // Handle navigation with joystick
    static constexpr float MOVE_THRESHOLD = 0.5f;
    static constexpr float CENTER_THRESHOLD = 0.3f;
    static constexpr float MAP_MOVE_SPEED = 5.0f; // pixels per tick

    // Move the map view (crosshair stays at center)
    if (joystick.x < -MOVE_THRESHOLD) {
      map_offset_x -= MAP_MOVE_SPEED;
    } else if (joystick.x > MOVE_THRESHOLD) {
      map_offset_x += MAP_MOVE_SPEED;
    }

    if (joystick.y < -MOVE_THRESHOLD) {
      map_offset_y -= MAP_MOVE_SPEED;
    } else if (joystick.y > MOVE_THRESHOLD) {
      map_offset_y += MAP_MOVE_SPEED;
    }

    // Apply Y clamping
    apply_y_clamp();
  }

  void render(Surface &fb_region) override {
    // Clear screen with ocean color (dark blue)
    hal::gpu::fill(fb_region, 0x001F);

    const auto &font = Font::regular_font();
    const u32 line_height = font.line_height() + 2;

    // Draw title
    font.render_colored("=== World Map ===", -1, fb_region, 70, 5, 0xFFFF);

    // Get current boat position
    i32 boat_x = get_boat_x();
    i32 boat_y = get_boat_y();

    // Calculate crosshair position (center of screen for map view)
    u32 screen_center_x = fb_region.get_width() / 2;
    u32 screen_center_y = fb_region.get_height() / 2;

    // Calculate crosshair world coordinates
    i32 crosshair_x = static_cast<i32>(map_offset_x);
    i32 crosshair_y = static_cast<i32>(map_offset_y);

    // Draw crosshair at center of screen
    font.render_colored("+", -1, fb_region, screen_center_x - 3,
                        screen_center_y - 4, 0xFFFF);

    // Draw bookmarks
    for (u32 i = 0; i < MAX_BOOKMARKS; i++) {
      if (bookmarks[i].active) {
        // Calculate screen position for bookmark
        i32 rel_x = bookmarks[i].x - crosshair_x;
        i32 rel_y = bookmarks[i].y - crosshair_y;

        i32 screen_x = screen_center_x + rel_x;
        i32 screen_y = screen_center_y + rel_y;

        // Only draw if on screen
        if (screen_x >= 0 && screen_x < (i32)fb_region.get_width() &&
            screen_y >= 0 && screen_y < (i32)fb_region.get_height()) {
          char icon_str[2] = {bookmarks[i].icon, '\0'};
          font.render_colored(icon_str, -1, fb_region, screen_x, screen_y,
                              0xFFE0); // Yellow
        }
      }
    }

    // Display coordinates at top
    char coord_buf[64];
    snprintf(coord_buf, sizeof(coord_buf), "Boat: (%d, %d)", boat_x, boat_y);
    font.render_colored(coord_buf, -1, fb_region, 10, 25, 0x7BEF);

    snprintf(coord_buf, sizeof(coord_buf), "Cross: (%d, %d)", crosshair_x,
             crosshair_y);
    font.render_colored(coord_buf, -1, fb_region, 10, 25 + line_height,
                        0x7BEF);

    // Display bookmarks list at bottom
    u32 y_pos = fb_region.get_height() - (MAX_BOOKMARKS + 2) * line_height - 5;
    font.render_colored("Bookmarks:", -1, fb_region, 10, y_pos, 0xFFFF);
    y_pos += line_height;

    u32 active_count = 0;
    for (u32 i = 0; i < MAX_BOOKMARKS; i++) {
      if (bookmarks[i].active) {
        active_count++;
        char bookmark_buf[64];
        snprintf(bookmark_buf, sizeof(bookmark_buf), "%c %s", bookmarks[i].icon,
                 bookmarks[i].name);
        font.render_colored(bookmark_buf, -1, fb_region, 15, y_pos, 0x7BEF);
        y_pos += line_height;
      }
    }

    if (active_count == 0) {
      font.render_colored("None yet", -1, fb_region, 15, y_pos, 0x7BEF);
    }

    // Instructions
    font.render_colored("A: Add Bookmark  B: Return", -1, fb_region, 10,
                        fb_region.get_height() - line_height - 5, 0x7BEF);
  }

  bool on_button_clicked(Button btn) override {
    if (btn == Button::Button1) {
      // Add bookmark at crosshair position
      add_bookmark();
      return true;
    } else if (btn == Button::Button2) {
      // Return to management menu
      on_back_action();
      return true;
    }
    return Scene::on_button_clicked(btn);
  }

  void on_back_action();

  bool is_active() const override;

private:
  ManagementUIScene &parent;

  static constexpr u32 MAX_BOOKMARKS = 5;
  static constexpr i32 MIN_Y = -100; // Minimum Y coordinate

  Bookmark bookmarks[MAX_BOOKMARKS];
  float map_offset_x = 0.0f;
  float map_offset_y = 0.0f;

  i32 get_boat_x();
  i32 get_boat_y();
  Clock &get_clock();

  void add_bookmark() {
    // Find first empty slot
    for (u32 i = 0; i < MAX_BOOKMARKS; i++) {
      if (!bookmarks[i].active) {
        bookmarks[i].active = true;
        bookmarks[i].x = static_cast<i32>(map_offset_x);
        bookmarks[i].y = static_cast<i32>(map_offset_y);
        bookmarks[i].icon = '0' + (i % 10); // Use 0-9 as icons

        // Generate bookmark name based on current time
        auto &clock = get_clock();
        u32 day = clock.get_num_days(app) + 1;
        u32 hr = clock.get_hr(app);
        const char *am_pm = (hr >= 12) ? "PM" : "AM";
        u32 display_hr = (hr % 12 == 0) ? 12 : (hr % 12);

        snprintf(bookmarks[i].name, sizeof(bookmarks[i].name),
                 "day %u %u %s", day, display_hr, am_pm);
        return;
      }
    }
  }

  void apply_y_clamp() {
    // Clamp Y to minimum value
    if (map_offset_y < MIN_Y) {
      map_offset_y = MIN_Y;
    }
    // X is unlimited (no clamping)
  }
};

} // namespace mgmt
} // namespace game
} // namespace scenes
} // namespace ge
