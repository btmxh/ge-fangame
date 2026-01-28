#pragma once

#include "ge-app/font.hpp"
#include "ge-app/game/inventory.hpp"
#include "ge-app/game/player_stats.hpp"
#include "ge-app/scenes/base.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/gpu.hpp"
#include "ge-hal/surface.hpp"
#include <algorithm>
#include <cstdio>
#include <cstring>

namespace ge {
namespace scenes {
namespace game {
class ManagementUIScene;
namespace mgmt {

class InventoryScene : public Scene {
public:
  InventoryScene(ManagementUIScene &parent);

  Inventory &get_inventory() { return inventory; }

  void tick(float dt) override {
    auto joystick = app.get_joystick_state();

    // Handle navigation with joystick
    static constexpr float MOVE_THRESHOLD = 0.5f;
    static constexpr float CENTER_THRESHOLD = 0.3f;

    if (inventory.get_item_count() > 0) {
      if (joystick.y < -MOVE_THRESHOLD && !joy_moved) {
        // Move up
        if (selected_index > 0) {
          selected_index--;
          // Adjust scroll if needed
          if (selected_index < scroll_offset) {
            scroll_offset = selected_index;
          }
        }
        joy_moved = true;
      } else if (joystick.y > MOVE_THRESHOLD && !joy_moved) {
        // Move down
        if (selected_index < inventory.get_item_count() - 1) {
          selected_index++;
          // Adjust scroll if needed
          const u32 max_visible_items = 8;
          if (selected_index >= scroll_offset + max_visible_items) {
            scroll_offset = selected_index - max_visible_items + 1;
          }
        }
        joy_moved = true;
      } else if (joystick.y > -CENTER_THRESHOLD &&
                 joystick.y < CENTER_THRESHOLD) {
        joy_moved = false;
      }
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
    snprintf(title_buf, sizeof(title_buf),
             "=== Inventory (%u/%u) ===", inventory.get_item_count(),
             Inventory::MAX_ITEMS);
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

        // Highlight selected item
        bool is_selected = (i == selected_index);
        if (is_selected) {
          // Draw selection indicator
          font.render_colored(">", -1, fb_region, 2, y_pos, 0xFFFF);
        }

        // Format: "1. Tropical Fish (Common) 0.5kg"
        char item_buf[64];
        const char *rarity_str = get_rarity_string(item.rarity);
        snprintf(item_buf, sizeof(item_buf), "%u. %s (%s) %.1fkg", i + 1,
                 item.name, rarity_str, item.weight);

        font.render_colored(item_buf, -1, fb_region, is_selected ? 16 : 10,
                            y_pos, color);
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
    font.render_colored("A: Eat Fish  B: Return", -1, fb_region, 10,
                        fb_region.get_height() - line_height - 10, 0x7BEF);
  }

  bool on_button_clicked(Button btn) override {
    if (btn == Button::Button1) {
      // Eat selected fish
      if (inventory.get_item_count() > 0 &&
          selected_index < inventory.get_item_count()) {
        const auto &item = inventory.get_item(selected_index);

        // Check if it's a poisonous or non-food item
        bool is_edible = true;
        float food_value = 10.0f; // Base food value

        // Pufferfish is poisonous
        if (strstr(item.name, "Pufferfish") != nullptr) {
          is_edible = false;
        }
        // Boots and chests are not edible
        if (strstr(item.name, "Boot") != nullptr ||
            strstr(item.name, "Chest") != nullptr) {
          is_edible = false;
        }

        if (is_edible) {
          auto &player_stats = get_player_stats();
          // Consume the fish - food value based on weight
          food_value = item.weight * 20.0f; // 1kg = 20 food
          player_stats.consume_fish(food_value);

          // Remove from inventory
          inventory.remove_fish(selected_index);

          // Adjust selected_index if needed
          if (selected_index >= inventory.get_item_count() &&
              selected_index > 0) {
            selected_index--;
          }
          // Adjust scroll if needed
          if (scroll_offset >= inventory.get_item_count()) {
            scroll_offset = get_max_scroll();
          }
        }
      }
      return true; // Event captured
    } else if (btn == Button::Button2) {
      // Return to management menu
      on_back_action();
      return true; // Event captured
    }
    return Scene::on_button_clicked(btn); // Check sub-scenes
  }

  void on_back_action();

  bool is_active() const override;

private:
  ManagementUIScene &parent;
  Inventory inventory;
  PlayerStats &get_player_stats();
  u32 scroll_offset;
  u32 selected_index;
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

} // namespace mgmt
} // namespace game
} // namespace scenes
} // namespace ge
