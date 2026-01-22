#pragma once

#include "ge-app/font.hpp"
#include "ge-hal/app.hpp"
#include "ge-hal/gpu.hpp"
#include <cstring>
namespace ge {

struct DialogMessage {
  const char *title;
  const char *desc;
};

class DialogBox {
public:
  DialogBox() = default;

  void render(App &app, Surface region, const DialogMessage &msg) {
    static constexpr u32 PADDING = 4;
    hal::gpu::fill(region, 0x0000);
    region =
        region.subsurface(PADDING, PADDING, region.get_width() - PADDING * 2,
                          region.get_height() - PADDING * 2);
    const auto &bold_font = Font::bold_font();
    bold_font.render_colored(msg.title, -1, region, 0, 0, 0xFFFF);
    region = region.subsurface(0, bold_font.line_height(), region.get_width(),
                               region.get_height() - bold_font.line_height());
    u32 num_chars = (app.now() - start_time) / ms_per_char;
    Font::regular_font().render_colored(msg.desc, num_chars, region, 0, 0,
                                        0xFFFF);
  }

  // default: show everything
  void set_start_time(i64 time = -1e12) { start_time = time; }

  bool message_complete(App &app, const DialogMessage &msg) {
    return (app.now() - start_time) / ms_per_char >= strlen(msg.desc);
  }

  // Show a message with input focus (blocks other input)
  void show_message(const char *title, const char *desc, i64 current_time) {
    pending_message.title = title;
    pending_message.desc = desc;
    has_focus = true;
    start_time = current_time;
    is_complete = false;
  }

  // Check if dialog has input focus
  bool has_input_focus() const { return has_focus; }

  // Get the current pending message
  const DialogMessage* get_pending_message() const {
    return has_focus ? &pending_message : nullptr;
  }

  // Update dialog state - returns true if message was just completed
  bool update(App &app) {
    if (!has_focus) return false;
    
    if (!is_complete && message_complete(app, pending_message)) {
      is_complete = true;
      return true; // Message just completed
    }
    return false;
  }

  // Dismiss the dialog (removes focus)
  void dismiss() {
    has_focus = false;
    is_complete = false;
  }

  // Check if current message is complete
  bool is_message_complete() const { return is_complete; }

private:
  i64 start_time = 0;
  i64 ms_per_char = 50;
  bool has_focus = false;
  bool is_complete = false;
  DialogMessage pending_message = {"", ""};
};

} // namespace ge
