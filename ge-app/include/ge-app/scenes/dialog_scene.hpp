#pragma once

#include "ge-app/font.hpp"
#include "ge-app/scenes/scene.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/gpu.hpp"
#include <cstring>

namespace ge {

struct DialogMessage {
  const char *title;
  const char *desc;
};

class DialogScene : public Scene {
public:
  DialogScene(App &app) : Scene(app), has_message(false), start_time(0), ms_per_char(50) {
    set_active(false); // Start inactive
  }

  void render(Surface &fb_region) override {
    if (!get_active() || !has_message) return;

    // Dialog box is positioned at the bottom
    static constexpr auto dialog_height = 64, dialog_padding = 4;
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
    Font::regular_font().render_colored(msg.desc, num_chars, region, 0, 0, 0xFFFF);
  }

  bool on_button_clicked(Button btn) override {
    if (!get_active() || !has_message) return false;

    // Dialog captures input when active
    if (btn == Button::Button2) {
      dismiss();
      return true; // Captured
    } else if (btn == Button::Button1) {
      if (message_complete()) {
        dismiss();
      } else {
        set_start_time();
      }
      return true; // Captured
    }
    return false;
  }

  // default: show everything
  void set_start_time(i64 time = -1e12) { start_time = time; }

  bool message_complete() {
    return !has_message || (app.now() - start_time) / ms_per_char >= strlen(msg.desc);
  }

  // Show a message with input focus (blocks other input)
  void show_message(const char *title, const char *desc) {
    msg = {title, desc};
    has_message = true;
    start_time = app.now();
    set_active(true);
  }

  // Check if dialog has input focus
  bool has_input_focus() const { return has_message && get_active(); }

  void dismiss() {
    msg = {"", ""};
    has_message = false;
    set_active(false);
  }

  // Get the current pending message
  const DialogMessage *get_pending_message() const {
    return has_message ? &msg : nullptr;
  }

private:
  i64 start_time;
  i64 ms_per_char;
  DialogMessage msg;
  bool has_message;
};

} // namespace ge
