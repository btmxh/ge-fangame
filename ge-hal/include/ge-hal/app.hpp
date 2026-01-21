
#pragma once

#include "ge-hal/core.hpp"
#include "ge-hal/surface.hpp"

namespace ge {

struct JoystickState {
  float x, y;
};

enum class Button { Button1, Button2 };

class App {
public:
  App();
  ~App();

#ifdef GE_HAL_STM32
  static void system_init();
#endif

  static constexpr int WIDTH = 240, HEIGHT = 320, AUDIO_FREQ = 8000;
#ifdef GE_HAL_PC
  // On PC, double buffering is automatically handled by SDL,
  // so we only need one buffer.
  static constexpr int NUM_BUFFERS = 1;
#else
  // On STM32, we use double buffering to avoid tearing.
  static constexpr int NUM_BUFFERS = 2;
#endif
  operator bool();

  std::int64_t now();
  void log(const char *fmt, ...);
  void sleep(std::int64_t ms);

  JoystickState get_joystick_state();

  void audio_bgm_play(const std::uint8_t *data, std::size_t length, bool loop);

  void audio_bgm_stop();
  bool audio_bgm_is_playing();

  // -------- SFX --------
  void audio_sfx_play(const std::uint8_t *data, std::size_t length,
                      std::size_t sample_rate);

  void audio_sfx_stop_all();

  // -------- global --------
  void audio_set_master_volume(std::uint8_t vol); // 0..255

  bool begin_render(Surface &out_surface);
  void end_render();

  bool button_clicked(Button btn);

  void wait_for_event(); // WFI on STM32
};
}; // namespace ge
