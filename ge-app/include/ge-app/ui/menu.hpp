#pragma once

#include "ge-app/font.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/core.hpp"
#include "ge-hal/gpu.hpp"
#include "ge-hal/surface.hpp"
#include <vector>

namespace ge {
namespace ui {

struct MenuItem {
  const char *label;
  int id;
  u32 text_width = 0; // Cached text width
};

class Menu {
public:
  Menu() = default;

  // Joystick navigation thresholds
  static constexpr float JOY_THRESHOLD_MOVE = 0.5f;
  static constexpr float JOY_THRESHOLD_CENTER = 0.3f;

  void add_item(const char *label, int id) {
    items.push_back({label, id, 0});
  }

  void render(Surface &region, const Font &font) {
    if (items.empty())
      return;

    // Cache text widths on first render if not already cached
    for (auto &item : items) {
      if (item.text_width == 0) {
        const char *c = item.label;
        while (*c) {
          u8 const *glyph_data;
          u8 glyph_w, glyph_h, advance;
          if (font.get_glyph(*c, glyph_data, glyph_w, glyph_h, advance)) {
            item.text_width += advance;
          } else {
            item.text_width += font.default_advance();
          }
          c++;
        }
      }
    }

    // Calculate item height and spacing
    u32 item_height = font.line_height() + 8; // 8px padding
    u32 total_height = static_cast<u32>(items.size()) * item_height;
    u32 start_y = (region.get_height() - total_height) / 2;

    for (u32 i = 0; i < static_cast<u32>(items.size()); i++) {
      u32 y_pos = start_y + i * item_height;
      
      // Draw selection indicator
      if (i == selected_index) {
        // Draw a simple border around selected item
        u32 padding = 4;
        auto item_region = region.subsurface(
            padding, y_pos - padding,
            region.get_width() - padding * 2, item_height);
        
        // Draw border by drawing 4 lines
        hal::gpu::fill(item_region.subsurface(0, 0, item_region.get_width(), 2),
                       0xFFFF);
        hal::gpu::fill(item_region.subsurface(0, item_region.get_height() - 2,
                                              item_region.get_width(), 2),
                       0xFFFF);
        hal::gpu::fill(item_region.subsurface(0, 0, 2, item_region.get_height()),
                       0xFFFF);
        hal::gpu::fill(item_region.subsurface(item_region.get_width() - 2, 0, 2,
                                              item_region.get_height()),
                       0xFFFF);
      }

      // Calculate centered x position for text using cached width
      u32 x_pos = (region.get_width() - items[i].text_width) / 2;

      // Draw text
      u16 color = (i == selected_index) ? 0xFFFF : 0xBDF7; // white or gray
      font.render_colored(items[i].label, -1, region, x_pos, y_pos, color);
    }
  }

  void move_selection(float joy_y) {
    if (items.empty())
      return;

    // Use joystick Y axis to move selection
    // Only trigger on significant movement to avoid jitter
    if (joy_y < -JOY_THRESHOLD_MOVE && !joy_moved) {
      selected_index = (selected_index > 0) ? selected_index - 1 
                                             : static_cast<u32>(items.size()) - 1;
      joy_moved = true;
    } else if (joy_y > JOY_THRESHOLD_MOVE && !joy_moved) {
      selected_index = (selected_index + 1) % static_cast<u32>(items.size());
      joy_moved = true;
    } else if (joy_y > -JOY_THRESHOLD_CENTER && joy_y < JOY_THRESHOLD_CENTER) {
      // Reset joy_moved when joystick returns to center
      joy_moved = false;
    }
  }

  int get_selected_id() const {
    if (selected_index < static_cast<u32>(items.size())) {
      return items[selected_index].id;
    }
    return -1;
  }

  u32 get_selected_index() const { return selected_index; }

private:
  std::vector<MenuItem> items;
  u32 selected_index = 0;
  bool joy_moved = false;
};

} // namespace ui
} // namespace ge
