#pragma once

#include "ge-app/font.hpp"
#include "ge-app/game/inventory.hpp"
#include "ge-app/scenes/scene.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/gpu.hpp"
#include "ge-hal/surface.hpp"
#include <cstdio>

namespace ge {

class InventoryScene : public Scene {
public:
  InventoryScene(App &app, const Inventory &inventory)
      : Scene{app}, inventory(inventory), scroll_offset(0) {}

  void tick(float dt) override {
    auto joystick = app.get_joystick_state();

    // Handle scrolling with joystick
    static constexpr float MOVE_THRESHOLD = 0.5f;
    static constexpr float CENTER_THRESHOLD = 0.3f;

    if (joystick.y < -MOVE_THRESHOLD && !joy_moved) {
      // Move up
      if (scroll_offset > 0) {
        scroll_offset--;
      }
      joy_moved = true;
    } else if (joystick.y > MOVE_THRESHOLD && !joy_moved) {
      // Move down
      u32 max_scroll = get_max_scroll();
      if (scroll_offset < max_scroll) {
        scroll_offset++;
      }
      joy_moved = true;
    } else if (joystick.y > -CENTER_THRESHOLD &&
               joystick.y < CENTER_THRESHOLD) {
      joy_moved = false;
    }
  }

  void render(Surface &fb_region) override {
    // Clear screen with dark background
    hal::gpu::fill(fb_region, 0x0000);

    const auto &font = Font::regular_font();
    u32 y_pos = 10;
    const u32 line_height = font.line_height() + 2;

    // Title
    char title_buf[64];
    snprintf(title_buf, sizeof(title_buf), "=== Inventory (%u/%u) ===",
             inventory.get_item_count(), Inventory::MAX_ITEMS);
    font.render_colored(title_buf, -1, fb_region, 10, y_pos, 0xFFFF);
    y_pos += line_height + 8;

    // Display items
    if (inventory.get_item_count() == 0) {
      font.render_colored("No fish caught yet!", -1, fb_region, 10, y_pos,
                          0x7BEF);
      y_pos += line_height;
      font.render_colored("Go fishing to add items.", -1, fb_region, 10, y_pos,
                          0x7BEF);
    } else {
      // Calculate visible items
      const u32 max_visible_items = 8;
      u32 start_index = scroll_offset;
      u32 end_index =
          std::min(start_index + max_visible_items, inventory.get_item_count());

      for (u32 i = start_index; i < end_index; i++) {
        const auto &item = inventory.get_item(i);
        if (item.is_empty())
          continue;

        // Get color based on rarity
        u16 color = get_rarity_color(item.rarity);

        // Format: "1. Tropical Fish (Common)"
        char item_buf[64];
        const char *rarity_str = get_rarity_string(item.rarity);
        snprintf(item_buf, sizeof(item_buf), "%u. %s (%s)", i + 1, item.name,
                 rarity_str);

        font.render_colored(item_buf, -1, fb_region, 10, y_pos, color);
        y_pos += line_height;
      }

      // Scroll indicators
      if (scroll_offset > 0) {
        font.render_colored("^ More above", -1, fb_region, 10, 30, 0x7BEF);
      }
      if (scroll_offset < get_max_scroll()) {
        font.render_colored("v More below", -1, fb_region, 10,
                            fb_region.get_height() - line_height - 30, 0x7BEF);
      }
    }

    // Instructions
    font.render_colored("Press B to return", -1, fb_region, 10,
                        fb_region.get_height() - line_height - 10, 0x7BEF);
  }

  void on_button_clicked(Button btn) override {
    if (btn == Button::Button2) {
      // Return to management menu
      on_back_action();
    }
  }

  // Virtual method to be overridden to handle back action
  virtual void on_back_action() {
    // This will be handled by MainApp
  }

private:
  const Inventory &inventory;
  u32 scroll_offset;
  bool joy_moved = false;

  u32 get_max_scroll() const {
    const u32 max_visible_items = 8;
    if (inventory.get_item_count() <= max_visible_items) {
      return 0;
    }
    return inventory.get_item_count() - max_visible_items;
  }

  u16 get_rarity_color(FishRarity rarity) const {
    switch (rarity) {
    case FishRarity::Common:
      return 0xFFFF; // White
    case FishRarity::Uncommon:
      return 0x07E0; // Green
    case FishRarity::Rare:
      return 0x001F; // Blue
    case FishRarity::Legendary:
      return 0xF81F; // Magenta
    default:
      return 0xFFFF;
    }
  }

  const char *get_rarity_string(FishRarity rarity) const {
    switch (rarity) {
    case FishRarity::Common:
      return "Common";
    case FishRarity::Uncommon:
      return "Uncommon";
    case FishRarity::Rare:
      return "Rare";
    case FishRarity::Legendary:
      return "Legendary";
    default:
      return "Unknown";
    }
  }
};

} // namespace ge
