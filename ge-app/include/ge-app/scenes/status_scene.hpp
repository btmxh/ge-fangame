#pragma once

#include "ge-app/font.hpp"
#include "ge-app/game/clock.hpp"
#include "ge-app/game/inventory.hpp"
#include "ge-app/game/mode_indicator.hpp"
#include "ge-app/game/player_stats.hpp"
#include "ge-app/scenes/scene.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/gpu.hpp"
#include "ge-hal/surface.hpp"
#include <cstdio>

namespace ge {

class StatusScene : public Scene {
public:
  StatusScene(App &app, const Clock &clock, const Inventory &inventory,
              const GameModeIndicator &mode_indicator, 
              const PlayerStats &player_stats)
      : Scene{app}, clock(clock), inventory(inventory),
        mode_indicator(mode_indicator), player_stats(player_stats) {}

  void tick(float dt) override {
    // No input handling needed for status screen
  }

  void render(Surface &fb_region) override {
    // Clear screen with dark background
    hal::gpu::fill(fb_region, 0x0000);

    const auto &font = Font::regular_font();
    u32 y_pos = 10;
    const u32 line_height = font.line_height() + 4;

    // Title
    font.render_colored("=== Player Status ===", -1, fb_region, 10, y_pos,
                        0xFFFF);
    y_pos += line_height + 10;

    // Game Time
    auto time_str = clock.get_display_string(app);
    font.render_colored(time_str, -1, fb_region, 10, y_pos, 0xF7BE);
    y_pos += line_height;

    // Current Mode
    char mode_buf[32];
    const char *mode_name = "";
    switch (mode_indicator.get_current_mode()) {
    case GameMode::Steering:
      mode_name = "Steering";
      break;
    case GameMode::Fishing:
      mode_name = "Fishing";
      break;
    case GameMode::Management:
      mode_name = "Management";
      break;
    }
    snprintf(mode_buf, sizeof(mode_buf), "Current Mode: %s", mode_name);
    font.render_colored(mode_buf, -1, fb_region, 10, y_pos, 0xF7BE);
    y_pos += line_height + 8;

    // Inventory Stats
    font.render_colored("Inventory:", -1, fb_region, 10, y_pos, 0xFFFF);
    y_pos += line_height;

    char inv_buf[64];
    snprintf(inv_buf, sizeof(inv_buf), "  Total Fish: %u / %u",
             inventory.get_item_count(), Inventory::MAX_ITEMS);
    font.render_colored(inv_buf, -1, fb_region, 10, y_pos, 0x07FF);
    y_pos += line_height;

    // Fish by rarity
    u32 common_count = inventory.get_count_by_rarity(FishRarity::Common);
    u32 uncommon_count = inventory.get_count_by_rarity(FishRarity::Uncommon);
    u32 rare_count = inventory.get_count_by_rarity(FishRarity::Rare);
    u32 legendary_count = inventory.get_count_by_rarity(FishRarity::Legendary);

    snprintf(inv_buf, sizeof(inv_buf), "  Common: %u", common_count);
    font.render_colored(inv_buf, -1, fb_region, 10, y_pos, 0xFFFF);
    y_pos += line_height;

    snprintf(inv_buf, sizeof(inv_buf), "  Uncommon: %u", uncommon_count);
    font.render_colored(inv_buf, -1, fb_region, 10, y_pos, 0x07E0);
    y_pos += line_height;

    snprintf(inv_buf, sizeof(inv_buf), "  Rare: %u", rare_count);
    font.render_colored(inv_buf, -1, fb_region, 10, y_pos, 0x001F);
    y_pos += line_height;

    snprintf(inv_buf, sizeof(inv_buf), "  Legendary: %u", legendary_count);
    font.render_colored(inv_buf, -1, fb_region, 10, y_pos, 0xF81F);
    y_pos += line_height + 10;

    // Food and Stamina Status Bars
    font.render_colored("Status:", -1, fb_region, 10, y_pos, 0xFFFF);
    y_pos += line_height;

    // Food bar
    float food_percent = player_stats.get_food_percent();
    snprintf(inv_buf, sizeof(inv_buf), "  Food: %.0f%%", food_percent);
    font.render_colored(inv_buf, -1, fb_region, 10, y_pos, 0xFBE0); // Yellow
    y_pos += line_height;

    // Draw food bar
    draw_status_bar(fb_region, 20, y_pos, 100, 8, food_percent, 0xFBE0);
    y_pos += 12;

    // Stamina bar
    float stamina_percent = player_stats.get_stamina_percent();
    snprintf(inv_buf, sizeof(inv_buf), "  Stamina: %.0f%%", stamina_percent);
    font.render_colored(inv_buf, -1, fb_region, 10, y_pos, 0x07FF); // Cyan
    y_pos += line_height;

    // Draw stamina bar
    draw_status_bar(fb_region, 20, y_pos, 100, 8, stamina_percent, 0x07FF);
    y_pos += 12;

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
  // Helper to draw a status bar
  void draw_status_bar(Surface &region, u32 x, u32 y, u32 width, u32 height,
                       float percent, u16 color) {
    // Border
    for (u32 i = 0; i < width; i++) {
      region.set_pixel<u16>(x + i, y, 0xFFFF);
      region.set_pixel<u16>(x + i, y + height - 1, 0xFFFF);
    }
    for (u32 i = 0; i < height; i++) {
      region.set_pixel<u16>(x, y + i, 0xFFFF);
      region.set_pixel<u16>(x + width - 1, y + i, 0xFFFF);
    }

    // Fill bar based on percentage
    u32 fill_width = static_cast<u32>((width - 2) * (percent / 100.0f));
    for (u32 j = 1; j < height - 1; j++) {
      for (u32 i = 1; i < width - 1; i++) {
        u16 pixel_color = (i - 1) < fill_width ? color : 0x0000;
        region.set_pixel<u16>(x + i, y + j, pixel_color);
      }
    }
  }

  const Clock &clock;
  const Inventory &inventory;
  const GameModeIndicator &mode_indicator;
  const PlayerStats &player_stats;
};

} // namespace ge
