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

private:
  i64 start_time = 0;
  i64 ms_per_char = 50;
};

} // namespace ge
