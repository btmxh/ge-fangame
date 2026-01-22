#pragma once

#include "ge-app/font.hpp"
#include "ge-hal/gpu.hpp"
#include <utility>
namespace ge {
enum class GameMode { Fishing, Steering, Management };

inline std::pair<u32, u32> game_mode_speed_multiplier(GameMode mode) {
  switch (mode) {
  case GameMode::Management:
    return {0, 1}; // paused
  default:
    return {1, 1};
  }
}

class GameModeIndicator {
public:
  void render(const Surface &region) {
    hal::gpu::fill(region, 0x0000);
    const char *mode_str = "";
    switch (current_mode) {
    case GameMode::Fishing:
      mode_str = "Mode: Fishing";
      break;
    case GameMode::Steering:
      mode_str = "Mode: Steering";
      break;
    case GameMode::Management:
      mode_str = "Mode: Management";
      break;
    }
    Font::regular_font().render_colored(mode_str, -1, region, 1, 1, 0xFFFF);
  }

  GameMode switch_mode() {
    return current_mode =
               static_cast<GameMode>((static_cast<int>(current_mode) + 1) % 3);
  }

  GameMode get_current_mode() const { return current_mode; }

private:
  GameMode current_mode = GameMode::Steering;
};

} // namespace ge
