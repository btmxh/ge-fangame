#include "ge-app/scenes/game/world/sky.hpp"

namespace ge {

// ============================================================
// Sky color helpers (private to this TU)
// ============================================================
namespace {

constexpr u16 SKY_NIGHT_RGB = 0x0821;
constexpr u16 SKY_DAY_RGB = 0x5DBF;
constexpr u16 SKY_WARM_RGB = 0xF2C0;

inline float clamp01(float x) { return std::max(0.0f, std::min(1.0f, x)); }

inline float smooth01(float x) {
  x = clamp01(x);
  return x * x * (3.0f - 2.0f * x);
}

inline u16 blend_rgb565(uint16_t a, uint16_t b, float t) {
  t = clamp01(t);
  u8 ti = static_cast<u8>(t * 255.0f);
  return ge::blend_rgb565(a, b, ti);
}

} // namespace

inline u16 sky_color(float t) {
  t = std::fmod(t, 1.0f);
  if (t < 0)
    t += 1.0f;

  constexpr float SUNRISE_START = 0.23f;
  constexpr float SUNRISE_END = 0.27f;
  constexpr float SUNSET_START = 0.73f;
  constexpr float SUNSET_END = 0.77f;

  if (t >= SUNRISE_START && t < SUNRISE_END) {
    float k = smooth01((t - SUNRISE_START) / (SUNRISE_END - SUNRISE_START));
    return (k < 0.5f)
               ? blend_rgb565(SKY_NIGHT_RGB, SKY_WARM_RGB, k * 2.0f)
               : blend_rgb565(SKY_WARM_RGB, SKY_DAY_RGB, (k - 0.5f) * 2.0f);
  }

  if (t >= SUNSET_START && t < SUNSET_END) {
    float k = smooth01((t - SUNSET_START) / (SUNSET_END - SUNSET_START));
    return (k < 0.5f)
               ? blend_rgb565(SKY_DAY_RGB, SKY_WARM_RGB, k * 2.0f)
               : blend_rgb565(SKY_WARM_RGB, SKY_NIGHT_RGB, (k - 0.5f) * 2.0f);
  }

  if (t >= SUNRISE_END && t < SUNSET_START)
    return SKY_DAY_RGB;

  return SKY_NIGHT_RGB;
}

void Sky::render(App &app, Surface render_region, Clock &clock) {
  set_x_offset(clock.get_game_timer().get(app) / 1000 % max_x_offset());
  set_sky_color(::ge::sky_color(clock.time_in_day(app)));

  const int W = render_region.get_width();
  const int H = render_region.get_height();
  const int TEX_W = CLOUD_TEXTURE_WIDTH;
  auto fb = render_region;

  // Fill sky
  hal::gpu::fill(render_region, sky_color);

  // temporary: recalculate cloud LUT every frame
  for (usize i = 0; i < sizeof(CLOUD_COLORS) / sizeof(CLOUD_COLORS[0]); ++i) {
    cloud_lut[i] = blend_rgb565(sky_color, cloud_color, CLOUD_COLORS[i]);
  }

  assert(H == 80);
  int stride = bg_clouds_len / H;

  auto sun = render_sun(render_region, clock.time_in_day(app));
  bool sun_visible = (sun.w > 0 && sun.h > 0);

  for (i32 y = 0; y < H; ++y) {
    const u8 *row_rle = &bg_clouds[CLOUD_ROW_OFFSETS[y]];
    i32 tex_x = 0;

    const bool sun_row = sun_visible && (y >= sun.y && y < sun.y + sun.h);

    while (true) {
      u8 elem = *row_rle++;
      i32 len = elem >> 4;
      i32 value = elem & 0x0F;
      len = (len < 0xF) ? (len + 1) : *row_rle++;

      i32 run_start = tex_x;
      tex_x += len;

      // map texture X → screen X with wrap
      i32 sx = run_start - x_offset;
      if (sx < 0)
        sx += TEX_W;

      auto draw_span = [&](i32 dx, i32 span_len) {
        if (span_len <= 0 || dx >= W)
          return;

        span_len = std::min(span_len, W - dx);

        // fast path: no sun on this row or this span
        if (!sun_row || dx + span_len <= sun.x || dx >= sun.x + sun.w) {
          hal::gpu::fill(fb.subsurface(dx, y, span_len, 1), cloud_lut[value]);
          return;
        }

        // split span against sun
        i32 x0 = dx;
        i32 x1 = dx + span_len;

        i32 ox0 = std::max<i32>(x0, sun.x);
        i32 ox1 = std::min<i32>(x1, sun.x + sun.w);

        // left (no sun)
        if (x0 < ox0) {
          hal::gpu::fill(fb.subsurface(x0, y, ox0 - x0, 1), cloud_lut[value]);
        }

        // middle (sun overlap → manual blend)
        for (i32 x = ox0; x < ox1; ++x) {
          u16 bg = fb.get_pixel(x, y); // sun already drawn
          u16 out = blend_rgb565(bg, cloud_color, CLOUD_COLORS[value]);
          fb.set_pixel(x, y, out);
        }

        // right (no sun)
        if (ox1 < x1) {
          hal::gpu::fill(fb.subsurface(ox1, y, x1 - ox1, 1), cloud_lut[value]);
        }
      };

      // normal span
      if (sx < W)
        draw_span(sx, len);

      // wrapped span
      if (sx + len > TEX_W)
        draw_span(0, (sx + len) - TEX_W);

      if (tex_x >= TEX_W)
        break;
    }
  }
}

Sky::Rect Sky::render_sun(Surface fb, float t) {
  constexpr float SUNRISE = 0.25f;
  constexpr float SUNSET = 0.75f;

  if (t < SUNRISE || t > SUNSET)
    return {0, 0, 0, 0};

  float day_t = (t - SUNRISE) / (SUNSET - SUNRISE);
  day_t = clamp01(day_t);

  const i32 W = fb.get_width();
  const i32 H = fb.get_height();

  const i32 SUN_W = sun_texture.get_width();
  const i32 SUN_H = sun_texture.get_height();

  // --- center-based position ---
  i32 cx = i32(day_t * (W + SUN_W)) - SUN_W / 2;

  float h = std::sin(day_t * M_PI);
  i32 cy = i32((H - 12) - h * (H - 22));

  // --- convert to top-left ---
  i32 x0 = cx - SUN_W / 2;
  i32 y0 = cy - SUN_H / 2;

  // --- clip against framebuffer ---
  i32 src_x = 0;
  i32 src_y = 0;
  i32 dst_x = x0;
  i32 dst_y = y0;
  i32 draw_w = SUN_W;
  i32 draw_h = SUN_H;

  if (dst_x < 0) {
    src_x -= dst_x;
    draw_w += dst_x;
    dst_x = 0;
  }
  if (dst_y < 0) {
    src_y -= dst_y;
    draw_h += dst_y;
    dst_y = 0;
  }
  if (dst_x + draw_w > W)
    draw_w = W - dst_x;
  if (dst_y + draw_h > H)
    draw_h = H - dst_y;

  if (draw_w <= 0 || draw_h <= 0)
    return {0, 0, 0, 0};

  // --- now everything is valid u32 ---
  auto dst =
      sun_texture.subsurface(u32(src_x), u32(src_y), u32(draw_w), u32(draw_h));
  auto src = fb.subsurface(u32(dst_x), u32(dst_y), u32(draw_w), u32(draw_h));
  hal::gpu::blit_blend(src, dst, 0xFF);

  return Rect{dst_x, dst_y, draw_w, draw_h};
}
} // namespace ge
