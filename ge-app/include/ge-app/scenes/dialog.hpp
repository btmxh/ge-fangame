#pragma once

#include "ge-app/scenes/base.hpp"
#include "ge-hal/app.hpp"

namespace ge {
namespace scenes {
class RootScene;

struct DialogMessage {
  const char *title;
  const char *desc;
};

class DialogScene : public Scene {
public:
  explicit DialogScene(RootScene &parent);

  void render(Surface &fb_region) override;
  bool on_button_clicked(Button btn) override;

  // default: show everything
  void set_start_time(i64 time = -1e12);

  bool message_complete();

  // Show a message with input focus (blocks other input)
  void show_message(const char *title, const char *desc);

  void dismiss();

  // Get the current pending message
  const DialogMessage *get_pending_message();

  bool is_active() const override { return has_msg; }

private:
  RootScene &parent;
  i64 start_time;
  i64 ms_per_char;
  DialogMessage msg;
  bool has_msg = false;
};

} // namespace scenes
} // namespace ge
