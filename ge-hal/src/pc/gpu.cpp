#include "ge-hal/gpu.hpp"
#include "ge-hal/surface.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>
#include <algorithm>
#include <iostream>
#include <vector>

namespace ge {
namespace hal {
namespace gpu {

// --- Internal Helper: Palette Storage ---
// STM32 DMA2D stores a CLUT (Color Look Up Table). We emulate this for
// L8/Indexed modes.
static std::vector<SDL_Color> g_clut(256);

// --- Internal Helper: Format Mapping ---
static SDL_PixelFormat to_sdl_format(PixelFormat fmt) {
  switch (fmt) {
  case PixelFormat::ARGB8888:
    return SDL_PIXELFORMAT_ARGB8888;
  case PixelFormat::RGB888:
    return SDL_PIXELFORMAT_RGB24;
  case PixelFormat::RGB565:
    return SDL_PIXELFORMAT_RGB565;
  case PixelFormat::ARGB1555:
    return SDL_PIXELFORMAT_ARGB1555;
  case PixelFormat::ARGB4444:
    return SDL_PIXELFORMAT_ARGB4444;
  case PixelFormat::L8:
    return SDL_PIXELFORMAT_INDEX8;
  // Fallbacks for formats SDL2 doesn't explicitly support in software blit
  // easily You might need custom conversion for A8/A4 if strictly required.
  default:
    std::cerr << "[DMA2D] Warning: Unsupported SDL format mapping for "
              << (int)fmt << ". Defaulting to ARGB8888.\n";
    return SDL_PIXELFORMAT_ARGB8888;
  }
}

// --- Internal Helper: Surface Wrapper ---
// Creates a temporary SDL_Surface that "views" the existing memory buffer.
// Does NOT copy data, does NOT free memory on destruction.
static SDL_Surface *create_sdl_wrapper(Surface &s) {
  int bpp = pixel_format_bpp(s.get_pixel_format());
  int pitch = (s.get_stride() * bpp) / 8; // SDL pitch is in bytes
  auto sdl_fmt = to_sdl_format(s.get_pixel_format());

  SDL_Surface *surf = SDL_CreateSurfaceFrom(s.get_width(), s.get_height(),
                                            sdl_fmt, s.data(), pitch);

  if (!surf) {
    std::cerr << "[DMA2D] SDL Surface creation failed: " << SDL_GetError()
              << "\n";
    return nullptr;
  }

  // Apply global palette if this is an indexed surface
  if (s.get_pixel_format() == PixelFormat::L8) {
    SDL_SetPaletteColors(SDL_GetSurfacePalette(surf), g_clut.data(), 0,
                         g_clut.size());
  }

  return surf;
}

// --- Internal Helper: Region Normalization ---
// Identical logic to the STM32 driver to ensure safety
static void normalize_regions(Surface &dst, Surface &src) {
  u32 width = std::min(src.get_width(), dst.get_width());
  u32 height = std::min(src.get_height(), dst.get_height());
  src = src.subsurface(0, 0, width, height);
  dst = dst.subsurface(0, 0, width, height);
}

void fill(Surface dst, u32 color) {
  SDL_Surface *s_dst = create_sdl_wrapper(dst);
  if (!s_dst)
    return;

  // NOTE: SDL_FillRect expects 'color' mapped to the format.
  // If 'color' is always ARGB8888 in your app logic, map it.
  // However, the STM32 Register-to-Memory (R2M) expects the raw value
  // for that format. We assume 'color' is already packed correctly
  // for the destination format (e.g., packed 565).

  SDL_FillSurfaceRect(s_dst, NULL, color);
  SDL_DestroySurface(s_dst);
}

void blit(Surface dst, Surface src) {
  normalize_regions(dst, src);

  SDL_Surface *s_src = create_sdl_wrapper(src);
  SDL_Surface *s_dst = create_sdl_wrapper(dst);

  if (s_src && s_dst) {
    // SDL_BlitSurface handles pixel format conversion (PFC) automatically
    SDL_BlitSurface(s_src, NULL, s_dst, NULL);
  }

  if (s_src)
    SDL_DestroySurface(s_src);
  if (s_dst)
    SDL_DestroySurface(s_dst);
}

void blit_blend(Surface dst, Surface src, u8 global_alpha) {
  normalize_regions(dst, src);

  SDL_Surface *s_src = create_sdl_wrapper(src);
  SDL_Surface *s_dst = create_sdl_wrapper(dst);

  if (s_src && s_dst) {
    // 1. Configure Blending Mode
    // STM32 hardware blends source onto destination.
    // SDL_BLENDMODE_BLEND = src_alpha * (src) + (1-src_alpha) * (dst)
    SDL_SetSurfaceBlendMode(s_src, SDL_BLENDMODE_BLEND);

    // 2. Apply Global Alpha
    // This corresponds to STM32's ALPHA register in the FGPFCCR
    SDL_SetSurfaceAlphaMod(s_src, global_alpha);

    SDL_BlitSurface(s_src, NULL, s_dst, NULL);
  }

  if (s_src)
    SDL_DestroySurface(s_src);
  if (s_dst)
    SDL_DestroySurface(s_dst);
}

void load_palette(const u32 *colors, usize num_colors) {
  // Store palette for future L8 surface creation
  // Assuming Palette struct has { u32* colors; u32 count; }
  // and colors are ARGB8888 (standard for STM32 CLUT)

  int count = std::min<int>(num_colors, 256);

  for (int i = 0; i < count; i++) {
    u32 c = colors[i];
    // Extract ARGB
    g_clut[i].a = (c >> 24) & 0xFF;
    g_clut[i].r = (c >> 16) & 0xFF;
    g_clut[i].g = (c >> 8) & 0xFF;
    g_clut[i].b = (c >> 0) & 0xFF;
  }
}

void blit_indexed(Surface dst, Surface src) { blit(dst, src); }

} // namespace gpu
} // namespace hal
} // namespace ge
