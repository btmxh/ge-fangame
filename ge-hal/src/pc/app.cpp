#include "ge-hal/app.hpp"
#include "ge-hal/surface.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <utility>

namespace ge {

class AppImpl {
public:
  void audio_callback(SDL_AudioStream *stream, int);

  AppImpl(App *app) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
      SDL_Log("SDL_Init Error: %s", SDL_GetError());
      exit();
    }

    if (!SDL_CreateWindowAndRenderer("Glow Embrace Fangame", App::WIDTH,
                                     App::HEIGHT, SDL_WINDOW_RESIZABLE, &window,
                                     &renderer)) {
      SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
      exit();
    }

    SDL_SetRenderVSync(renderer, 1);

    frame_texture =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565,
                          SDL_TEXTUREACCESS_STREAMING, App::WIDTH, App::HEIGHT);

    SDL_AudioSpec spec{};
    spec.format = SDL_AUDIO_U8;
    spec.channels = 1;
    spec.freq = 8000;
    audio_dev = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    audio_stream = SDL_CreateAudioStream(&spec, &spec);
    SDL_SetAudioStreamGetCallback(
        audio_stream,
        [](void *userdata, SDL_AudioStream *stream, int len, int) {
          static_cast<AppImpl *>(userdata)->audio_callback(stream, len);
        },
        this);

    SDL_BindAudioStream(audio_dev, audio_stream);
    SDL_ResumeAudioDevice(audio_dev);
  }

  ~AppImpl() {
    SDL_DestroyTexture(frame_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  static void exit() { std::exit(1); }

  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  SDL_Texture *frame_texture = nullptr;
  SDL_AudioDeviceID audio_dev = 0;
  SDL_AudioStream *audio_stream;
  SDL_Gamepad *pad = nullptr;
  bool quit = false;

  struct AudioStream {
    const std::uint8_t *data;
    std::size_t length;
    std::size_t pos;
    bool loop;
    bool active;
  };

  static constexpr int MAX_SFX = 4;

  AudioStream bgm;
  AudioStream sfx[MAX_SFX];
  std::uint8_t master_volume = 255;
  u16 framebuffer[App::WIDTH * App::HEIGHT];

  friend class App;
};

void AppImpl::audio_callback(SDL_AudioStream *stream, int) {
  constexpr int BUF = 512;
  std::uint8_t out[BUF];

  for (int i = 0; i < BUF; ++i) {
    int mixed = 0;

    // ---- BGM ----
    if (bgm.active) {
      if (bgm.pos >= bgm.length) {
        if (bgm.loop)
          bgm.pos = 0;
        else
          bgm.active = false;
      }
      if (bgm.active) {
        mixed += int(bgm.data[bgm.pos++]) - 128;
      }
    }

    // ---- SFX ----
    for (int c = 0; c < MAX_SFX; ++c) {
      auto &s = sfx[c];
      if (!s.active)
        continue;

      if (s.pos >= s.length) {
        s.active = false;
        continue;
      }
      mixed += int(s.data[s.pos++]) - 128;
    }

    // volume + clamp
    mixed = (mixed * master_volume) / 255;
    if (mixed < -128)
      mixed = -128;
    if (mixed > 127)
      mixed = 127;

    out[i] = std::uint8_t(mixed + 128);
  }

  SDL_PutAudioStreamData(stream, out, BUF);
}

std::unique_ptr<AppImpl> app_impl_instance = nullptr;

static constexpr i64 BUTTON_HOLD_THRESHOLD_MS = 1000;
struct ButtonState {
  i64 last_up = -1, last_down = -1;
  bool handled_hold = false;
} button_states[2];

App::App() { app_impl_instance = std::make_unique<AppImpl>(this); }
App::~App() { app_impl_instance.reset(); }

App::operator bool() { return app_impl_instance && !app_impl_instance->quit; }

static auto gamepad_button_to_button = [](u32 gb) -> int {
  switch (gb) {
  case SDL_GAMEPAD_BUTTON_LABEL_A:
    return 0;
  case SDL_GAMEPAD_BUTTON_LABEL_B:
    return 1;
  default:
    return -1;
  }
};

static void handle_event(App &app) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_EVENT_QUIT:
      app_impl_instance->quit = true;
      break;
    case SDL_EVENT_GAMEPAD_ADDED:
      if (app_impl_instance->pad == nullptr) {
        app_impl_instance->pad = SDL_OpenGamepad(event.gdevice.which);
        if (!app_impl_instance->pad) {
          app.log("SDL_OpenGamepad Error: %s", SDL_GetError());
        } else {
          app.log("Gamepad connected: %s",
                  SDL_GetGamepadName(app_impl_instance->pad));
        }
      }
      break;
    case SDL_EVENT_GAMEPAD_REMOVED:
      if (app_impl_instance->pad &&
          SDL_GetGamepadID(app_impl_instance->pad) == event.gdevice.which) {
        SDL_CloseGamepad(app_impl_instance->pad);
        app_impl_instance->pad = nullptr;
        app.log("Gamepad disconnected");
      }
      break;
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN: {
      int btn = gamepad_button_to_button(event.gbutton.button);
      if (btn >= 0) {
        button_states[btn].last_down = app.now();
        button_states[btn].handled_hold = false;
      }
      break;
    }
    case SDL_EVENT_GAMEPAD_BUTTON_UP: {
      int btn = gamepad_button_to_button(event.gbutton.button);
      if (btn >= 0) {
        button_states[btn].last_up = app.now();
        if (button_states[btn].last_down < 0)
          break;
        i64 held_time =
            button_states[btn].last_up - button_states[btn].last_down;
        if (held_time < BUTTON_HOLD_THRESHOLD_MS) {
          app.on_button_clicked(static_cast<Button>(btn));
        } else {
          app.on_button_finished_hold(static_cast<Button>(btn));
        }
        button_states[btn].handled_hold = false;
      }
      break;
    }
    }
  }
}

void App::tick(float /*dt*/) {
  for (auto btn : {Button::Button1, Button::Button2}) {
    auto &bs = button_states[static_cast<int>(btn)];
    if (bs.last_down < 0 || bs.last_up > bs.last_down || bs.handled_hold)
      continue;
    i64 held_time = now() - bs.last_down;
    if (held_time >= BUTTON_HOLD_THRESHOLD_MS) {
      on_button_held(btn);
    }
  }
}

void App::loop() {
  i64 last_tick = now();
  while (*this) {
    handle_event(*this);
    i64 current = now();
    float dt = (current - last_tick) * 1e-3f;
    tick(dt);
    last_tick = current;

    // Render to framebuffer
    Surface fb_region{app_impl_instance->framebuffer,
                      WIDTH,
                      WIDTH,
                      HEIGHT,
                      PixelFormat::RGB565,
                      0};
    render(fb_region);

    // Upload framebuffer to screen
    int win_w, win_h;
    SDL_GetWindowSize(app_impl_instance->window, &win_w, &win_h);

    float sx = (float)win_w / WIDTH;
    float sy = (float)win_h / HEIGHT;
    float scale = (sx < sy) ? sx : sy;

    int dst_w = (int)(WIDTH * scale);
    int dst_h = (int)(HEIGHT * scale);
    int dst_x = (win_w - dst_w) / 2;
    int dst_y = (win_h - dst_h) / 2;

    auto *impl = app_impl_instance.get();

    // upload framebuffer → texture
    SDL_UpdateTexture(impl->frame_texture, nullptr,
                      app_impl_instance->framebuffer,
                      WIDTH * sizeof(app_impl_instance->framebuffer[0]));

    // letterbox clear
    SDL_SetRenderDrawColor(impl->renderer, 0, 0, 0, 255);
    SDL_RenderClear(impl->renderer);

    // render scaled texture
    SDL_FRect dstf{(float)dst_x, (float)dst_y, (float)dst_w, (float)dst_h};

    SDL_RenderTexture(impl->renderer, impl->frame_texture, nullptr, &dstf);
    // TODO: remove VSync if that is problematic
    SDL_RenderPresent(impl->renderer);
  }
}

void App::request_quit() { app_impl_instance->quit = true; }

std::int64_t App::now() { return SDL_GetTicks(); }

JoystickState App::get_joystick_state() {
  if (!app_impl_instance->pad)
    return JoystickState{};

  auto norm_axis = [](int v) {
    return (v >= 0) ? (v / 32767.0f) : (v / 32768.0f);
  };

  float x = norm_axis(
      SDL_GetGamepadAxis(app_impl_instance->pad, SDL_GAMEPAD_AXIS_LEFTX));
  float y = norm_axis(
      -SDL_GetGamepadAxis(app_impl_instance->pad, SDL_GAMEPAD_AXIS_LEFTY));

  static const float dz = 0.12, dz_squared = dz * dz;

  float mag = std::sqrt(x * x + y * y);
  if (mag < dz) {
    x = y = 0.0f;
  } else {
    float scale = (mag - dz) / (1.0f - dz);
    x = (x / mag) * scale;
    y = (y / mag) * scale;
  }

  x = std::max(std::min(x, 1.0f), -1.0f);
  y = std::max(std::min(y, 1.0f), -1.0f);

  return {x, y};
}

static u32 button_to_gamepad_button(Button button) {
  switch (button) {
  case Button::Button1:
    return SDL_GAMEPAD_BUTTON_LABEL_A;
  case Button::Button2:
    return SDL_GAMEPAD_BUTTON_LABEL_B;
  default:
    return SDL_GAMEPAD_BUTTON_INVALID;
  }
}

void App::log(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt,
                  args);
  va_end(args);
}

void App::sleep(std::int64_t ms) { SDL_Delay((Uint32)ms); }

void App::audio_bgm_play(const std::uint8_t *data, std::size_t len, bool loop) {
  app_impl_instance->bgm = {data, len, 0, loop, true};
}

void App::audio_bgm_stop() {
  auto *impl = app_impl_instance.get();
  impl->bgm.active = false;
  SDL_ClearAudioStream(impl->audio_stream);
}

bool App::audio_bgm_is_playing() { return app_impl_instance->bgm.active; }

void App::audio_sfx_play(const std::uint8_t *data, std::size_t len,
                         std::size_t /*rate*/) {
  auto &sfx = app_impl_instance->sfx;
  for (int i = 0; i < AppImpl::MAX_SFX; ++i) {
    if (!sfx[i].active) {
      sfx[i] = {data, len, 0, false, true};
      return;
    }
  }

  // voice steal (overwrite oldest)
  sfx[0] = {data, len, 0, false, true};
}

void App::audio_sfx_stop_all() {
  for (int i = 0; i < AppImpl::MAX_SFX; ++i) {
    app_impl_instance->sfx[i].active = false;
  }
}

void App::audio_set_master_volume(std::uint8_t vol) {
  app_impl_instance->master_volume = vol;
}

} // namespace ge
