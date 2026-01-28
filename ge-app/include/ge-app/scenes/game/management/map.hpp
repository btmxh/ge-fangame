#pragma once

#include "ge-app/font.hpp"
#include "ge-app/game/boat.hpp"
#include "ge-app/game/clock.hpp"
#include "ge-app/game/water.hpp"
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
    static constexpr float MAP_MOVE_SPEED = 100.0f; // pixels per second

    // Move the map view (crosshair stays at center) - frame-rate independent
    if (joystick.x < -MOVE_THRESHOLD && !joy_moved_x) {
      map_offset_x -= MAP_MOVE_SPEED * dt;
      joy_moved_x = true;
    } else if (joystick.x > MOVE_THRESHOLD && !joy_moved_x) {
      map_offset_x += MAP_MOVE_SPEED * dt;
      joy_moved_x = true;
    } else if (joystick.x > -CENTER_THRESHOLD &&
               joystick.x < CENTER_THRESHOLD) {
      joy_moved_x = false;
    }

    if (joystick.y < -MOVE_THRESHOLD && !joy_moved_y) {
      map_offset_y -= MAP_MOVE_SPEED * dt;
      joy_moved_y = true;
    } else if (joystick.y > MOVE_THRESHOLD && !joy_moved_y) {
      map_offset_y += MAP_MOVE_SPEED * dt;
      joy_moved_y = true;
    } else if (joystick.y > -CENTER_THRESHOLD &&
               joystick.y < CENTER_THRESHOLD) {
      joy_moved_y = false;
    }

    // Apply Y clamping
    apply_y_clamp();
  }

  void render(Surface &fb_region) override {
    // Render ocean texture
    water.render(fb_region, 0.0f,
                 static_cast<u32>(static_cast<i32>(map_offset_x)),
                 static_cast<u32>(static_cast<i32>(-map_offset_y)));

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

    // Draw boat position on map
    i32 boat_rel_x = boat_x - crosshair_x;
    i32 boat_rel_y = boat_y - crosshair_y;
    i32 boat_screen_x = screen_center_x + boat_rel_x;
    i32 boat_screen_y = screen_center_y + boat_rel_y;

    // Only draw boat marker if on screen
    if (boat_screen_x >= 0 && boat_screen_x < (i32)fb_region.get_width() &&
        boat_screen_y >= 0 && boat_screen_y < (i32)fb_region.get_height()) {
      // Temp marker (red square)
      hal::gpu::fill(fb_region.subsurface(boat_screen_x, boat_screen_y, 4, 4),
                     0xF800);
    }

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
    font.render_colored(coord_buf, -1, fb_region, 10, 25 + line_height, 0x7BEF);

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
    } else if (active_count >= MAX_BOOKMARKS) {
      font.render_colored("(Full - new adds remove oldest)", -1, fb_region, 15,
                          y_pos, 0xFFE0); // Yellow
    }

    // Instructions
    char instr_buf[64];
    snprintf(instr_buf, sizeof(instr_buf), "A: Add Bookmark  B: Return");
    font.render_colored(instr_buf, -1, fb_region, 10,
                        fb_region.get_height() - line_height - 5, 0x7BEF);
  }

  bool on_button_clicked(Button btn) override {
    if (btn == Button::Button1) {
      // Add bookmark at crosshair position
      bool added = add_bookmark();
      if (!added) {
        // All slots full - remove oldest and add new one
        remove_first_bookmark();
        add_bookmark();
      }
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
  Water water;

  static constexpr u32 MAX_BOOKMARKS = 5;
  static constexpr i32 MIN_Y = -100; // Minimum Y coordinate

  Bookmark bookmarks[MAX_BOOKMARKS];
  float map_offset_x = 0.0f;
  float map_offset_y = 0.0f;
  bool joy_moved_x = false;
  bool joy_moved_y = false;

  i32 get_boat_x();
  i32 get_boat_y();
  Clock &get_clock();

  bool add_bookmark() {
    // Find first empty slot
    for (u32 i = 0; i < MAX_BOOKMARKS; i++) {
      if (!bookmarks[i].active) {
        bookmarks[i].active = true;
        bookmarks[i].x = static_cast<i32>(map_offset_x);
        bookmarks[i].y = static_cast<i32>(map_offset_y);
        bookmarks[i].icon = '0' + i; // Use 0-4 as icons

        // Generate bookmark name based on current time
        auto &clock = get_clock();
        u32 day = clock.get_num_days(app) + 1;
        u32 hr = clock.get_hr(app);
        const char *am_pm = (hr >= 12) ? "PM" : "AM";
        u32 display_hr = (hr % 12 == 0) ? 12 : (hr % 12);

        snprintf(bookmarks[i].name, sizeof(bookmarks[i].name), "day %u %u %s",
                 day, display_hr, am_pm);
        return true;
      }
    }
    return false; // All slots full
  }

  void remove_first_bookmark() {
    // Remove the first active bookmark (oldest)
    for (u32 i = 0; i < MAX_BOOKMARKS; i++) {
      if (bookmarks[i].active) {
        bookmarks[i].active = false;
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
