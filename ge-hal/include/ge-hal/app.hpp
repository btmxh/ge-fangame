
#pragma once

#include <cstdint>

namespace ge {
class App {
public:
  App();
  ~App();

#ifdef GE_HAL_STM32
  static void system_init();
#endif

  static constexpr int WIDTH = 320, HEIGHT = 240, AUDIO_FREQ = 8000;
  operator bool();

  void begin();
  void end();

  std::int64_t now();
  void log(const char *fmt, ...);

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
