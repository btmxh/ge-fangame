#pragma once

#include "ge-app/scenes/dialog_scene.hpp"

namespace ge {

// Tutorial system that manages sequential dialog messages
class TutorialSystem {
public:
  TutorialSystem() : current_index(0), completed(false) {}

  // Initialize tutorial messages
  void initialize(DialogScene &dialog, const DialogMessage *messages,
                  u32 count) {
    this->messages = messages;
    this->message_count = count;
    current_index = 0;
    completed = false;

    // Show first message
    if (message_count > 0) {
      dialog.show_message(messages[0].title, messages[0].desc);
    }
  }

  // Advance to next message, returns true if more messages available
  bool next_message(DialogScene &dialog) {
    if (completed)
      return false;

    current_index++;
    if (current_index >= message_count) {
      completed = true;
      return false;
    }

    dialog.show_message(messages[current_index].title,
                        messages[current_index].desc);
    return true;
  }

  // Check if tutorial is completed
  bool is_completed() const { return completed; }

  // Get current message index
  u32 get_current_index() const { return current_index; }

private:
  const DialogMessage *messages;
  u32 message_count;
  u32 current_index;
  bool completed;
};

} // namespace ge
