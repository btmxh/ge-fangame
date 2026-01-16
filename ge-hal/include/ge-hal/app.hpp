
#pragma once

#include "ge-hal/fb.hpp"
#include <cstdint>

namespace ge {

struct JoystickState {
  float x, y;
};

class App {
public:
  App();
  ~App();

  static constexpr int WIDTH = 240, HEIGHT = 320, AUDIO_FREQ = 8000;
  operator bool();

  void begin();
  void end();

  std::int64_t now();
  void log(const char *fmt, ...);

  JoystickState get_joystick_state();

  FramebufferRegion framebuffer_region() {
    return FramebufferRegion{framebuffer, WIDTH, WIDTH, HEIGHT};
  }

  void audio_bgm_play(const std::uint8_t *data, std::size_t length, bool loop);

  void audio_bgm_stop();
  bool audio_bgm_is_playing();

  // -------- SFX --------
  void audio_sfx_play(const std::uint8_t *data, std::size_t length,
                      std::size_t sample_rate);

  void audio_sfx_stop_all();

  // -------- global --------
  void audio_set_master_volume(std::uint8_t vol); // 0..255

  // private:
  std::uint16_t framebuffer[WIDTH * HEIGHT] = {0};
};
}; // namespace ge
