#include "ge-app/scenes/dialog.hpp"
#include "ge-app/font.hpp"
#include "ge-app/scenes/main.hpp"
#include "ge-hal/gpu.hpp"

#include <cstring>

namespace ge {
namespace scenes {

DialogScene::DialogScene(RootScene &parent)
    : Scene(parent.get_app()), parent{parent}, start_time(0), ms_per_char(50) {}

void DialogScene::render(Surface &fb_region) {
  // Dialog box is positioned at the bottom
  static constexpr auto dialog_height = 64;
  static constexpr auto dialog_padding = 4;

  auto region = fb_region.subsurface(
      dialog_padding, fb_region.get_height() - dialog_height - dialog_padding,
      fb_region.get_width() - dialog_padding * 2, dialog_height);

  static constexpr u32 PADDING = 4;
  hal::gpu::fill(region, 0x0000);

  region = region.subsurface(PADDING, PADDING, region.get_width() - PADDING * 2,
                             region.get_height() - PADDING * 2);

  const auto &bold_font = Font::bold_font();
  bold_font.render_colored(msg.title, -1, region, 0, 0, 0xFFFF);

  region = region.subsurface(0, bold_font.line_height(), region.get_width(),
                             region.get_height() - bold_font.line_height());

  u32 num_chars = (app.now() - start_time) / ms_per_char;
  Font::regular_font().render_colored(msg.desc, num_chars, region, 0, 0,
                                      0xFFFF);
}

bool DialogScene::on_button_clicked(Button btn) {
  // Dialog captures input when active
  if (btn == Button::Button2) {
    dismiss();
    return true;
  } else if (btn == Button::Button1) {
    if (message_complete()) {
      dismiss();
    } else {
      set_start_time();
    }
    return true;
  }

  return false;
}

void DialogScene::set_start_time(i64 time) { start_time = time; }

bool DialogScene::message_complete() {
  return !has_msg ||
         (app.now() - start_time) / ms_per_char >= std::strlen(msg.desc);
}

void DialogScene::show_message(const char *title, const char *desc) {
  start_time = app.now();
  msg = {title, desc};
  has_msg = true;
}

void DialogScene::dismiss() { has_msg = false; }

const DialogMessage *DialogScene::get_pending_message() {
  return has_msg ? &msg : nullptr;
}

} // namespace scenes
} // namespace ge
