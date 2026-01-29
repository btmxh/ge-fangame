#include "ge-app/scenes/game/gameover.hpp"
#include "ge-app/scenes/game/main.hpp"
#include <cinttypes>

namespace ge {
namespace scenes {
namespace game {
GameOverScene::GameOverScene(GameScene &parent)
    : Scene{parent.get_app()}, parent(parent) {}

void GameOverScene::render(Surface &fb_region) {
  // HACK: reuse the same bg from management UI
  auto fb = parent.get_management_ui_scene().render_bg(fb_region);
  auto &font = Font::big_font();
  const char *game_over_text = "Game Over";
  auto text_width = font.text_width(game_over_text, -1);
  auto x = (fb.get_width() - text_width) / 2;
  auto y = (fb.get_height() - font.line_height()) / 2 - 40;
  font.render_colored(game_over_text, -1, fb, x, y, 0xF800);

  auto &small_font = Font::regular_font();

  // score (max Y)
  char buf[64];
  std::snprintf(buf, sizeof(buf), "Max Y Reached: %d m",
                parent.get_player_stats().get_max_y());
  text_width = small_font.text_width(buf, -1);
  x = (fb.get_width() - text_width) / 2;
  y += font.line_height() + 20;
  small_font.render_colored(buf, -1, fb, x, y, 0x0000);
  std::snprintf(buf, sizeof(buf), "Total time survived: %" PRIu32 " days",
                parent.get_clock().get_num_days(app));
  text_width = small_font.text_width(buf, -1);
  x = (fb.get_width() - text_width) / 2;
  y += font.line_height();
  small_font.render_colored(buf, -1, fb, x, y, 0x0000);

  const char *prompt_text = "Button A: Restart\nButton B: Main Menu";
  text_width = small_font.text_width("Press Button 1 to Restart", -1);
  x = (fb.get_width() - text_width) / 2;
  y += font.line_height() + 10;
  small_font.render_colored(prompt_text, -1, fb, x, y, 0x7BEF);
}

bool GameOverScene::on_button_clicked(Button btn) {
  if (btn == Button::Button1) {
    parent.start_new_game();
    return true;
  } else if (btn == Button::Button2) {
    parent.return_to_main_menu();
    return true;
  }

  return false;
}

bool GameOverScene::is_active() const {
  return parent.get_player_stats().is_dead();
}
} // namespace game
} // namespace scenes
} // namespace ge
