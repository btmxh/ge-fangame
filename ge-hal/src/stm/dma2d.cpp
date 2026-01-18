#include "ge-hal/stm/dma2d.hpp"

namespace ge {
namespace hal {
namespace stm {

void DMA2DDevice::fill(Surface dst, Rect rect, u32 color) {
  wait_idle();

  // Mode: Register-to-Memory (0x3)
  DMA2D->CR = 0x00030000UL;
  DMA2D->OCOLR = color;

  // Output Config
  DMA2D->OPFCCR = static_cast<u32>(dst.fmt);

  // Geometry
  u32 bpp = (dst.fmt == PixelFormat::RGB888) ? 3
            : (dst.fmt == PixelFormat::ARGB8888)
                ? 4
                : 2; // Rough assumption for common types

  // For RGB565 (standard), BytesPerPixel is 2.
  if (dst.fmt == PixelFormat::RGB565)
    bpp = 2;

  u32 offset = (rect.y * dst.width + rect.x) * bpp;
  DMA2D->OMAR = reinterpret_cast<u32>(dst.pixels) + offset;

  DMA2D->NLR = (rect.w << 16) | rect.h;
  DMA2D->OOR = dst.width - rect.w;

  // Fire
  DMA2D->CR |= DMA2D_CR_START;
}

void DMA2DDevice::blit(Surface dst, Surface src, int x, int y) {
  Rect full_src = {0, 0, src.width, src.height};
  blit_ex(dst, src, full_src, x, y);
}

void DMA2DDevice::blit_ex(Surface dst, Surface src, Rect src_rect, int x,
                          int y) {
  wait_idle();

  // Determine Mode: Pure Copy vs Conversion
  if (src.fmt == dst.fmt) {
    DMA2D->CR = 0x00000000UL; // M2M (Fastest)
  } else {
    DMA2D->CR = 0x00010000UL; // M2M_PFC (Convert)
  }

  // Output Setup
  DMA2D->OPFCCR = static_cast<u32>(dst.fmt);
  u32 dst_offset = (y * dst.width + x) * 2; // Assuming RGB565 dst
  DMA2D->OMAR = reinterpret_cast<u32>(dst.pixels) + dst_offset;
  DMA2D->OOR = dst.width - src_rect.w;
  DMA2D->NLR = (src_rect.w << 16) | src_rect.h;

  // Foreground Setup (Source)
  DMA2D->FGPFCCR = static_cast<u32>(src.fmt);

  // Calculate Source Offset based on BPP
  u32 src_bpp = 2; // Default 16-bit
  if (src.fmt == PixelFormat::ARGB8888)
    src_bpp = 4;
  else if (src.fmt == PixelFormat::L8)
    src_bpp = 1;

  u32 src_mem_offset = (src_rect.y * src.width + src_rect.x) * src_bpp;
  DMA2D->FGMAR = reinterpret_cast<u32>(src.pixels) + src_mem_offset;

  // Source Stride
  DMA2D->FGOR = src.width - src_rect.w;

  // Fire
  DMA2D->CR |= DMA2D_CR_START;
}

void DMA2DDevice::blit_blend(Surface dst, Surface src, int x, int y,
                             u8 global_alpha) {
  wait_idle();

  // Mode: M2M with Blending (0x2)
  DMA2D->CR = 0x00020000UL;

  // Output Setup
  DMA2D->OPFCCR = static_cast<u32>(dst.fmt);
  u32 dst_offset = (y * dst.width + x) * 2;
  DMA2D->OMAR = reinterpret_cast<u32>(dst.pixels) + dst_offset;
  DMA2D->OOR = dst.width - src.width;
  DMA2D->NLR = (src.width << 16) | src.height;

  // Foreground (Sprite) Setup
  // Alpha Mode: 0=NoMod, 1=Replace, 2=Combine (Multiply)
  u32 am = (global_alpha < 255) ? 2 : 0;
  DMA2D->FGPFCCR =
      static_cast<u32>(src.fmt) | (am << 16) | (global_alpha << 24);

  DMA2D->FGMAR = reinterpret_cast<u32>(src.pixels);
  DMA2D->FGOR = 0; // Assuming full sprite copy

  // Background (Current Screen) Setup
  // Must match Output Format usually
  DMA2D->BGPFCCR = static_cast<u32>(dst.fmt);
  DMA2D->BGMAR = DMA2D->OMAR; // Read from same place we write
  DMA2D->BGOR = DMA2D->OOR;

  // Fire
  DMA2D->CR |= DMA2D_CR_START;
}

void DMA2DDevice::load_palette(Palette pal) {
  wait_idle();

  // 1. Point to Colors
  DMA2D->FGCMAR = reinterpret_cast<u32>(pal.colors);

  // 2. Configure Load
  // Size = count - 1. Mode = ARGB8888 (1).
  DMA2D->FGPFCCR = ((pal.count - 1) << 8) | (0x01 << 4);

  // 3. Start Load
  DMA2D->FGPFCCR |= DMA2D_FGPFCCR_START;

  // 4. Wait strictly for CLUT load to finish
  while (!(DMA2D->ISR & DMA2D_ISR_CTCIF))
    ;
  DMA2D->IFCR = DMA2D_IFCR_CCTCIF;
}

void DMA2DDevice::blit_indexed(Surface dst, Surface src, int x, int y) {
  wait_idle();

  // Mode: M2M with PFC (0x1) - Converts L8 -> RGB565 via CLUT
  DMA2D->CR = 0x00010000UL;

  // Output Setup
  DMA2D->OPFCCR = static_cast<u32>(dst.fmt);
  u32 dst_offset = (y * dst.width + x) * 2;
  DMA2D->OMAR = reinterpret_cast<u32>(dst.pixels) + dst_offset;
  DMA2D->OOR = dst.width - src.width;
  DMA2D->NLR = (src.width << 16) | src.height;

  // Foreground Setup (L8)
  DMA2D->FGPFCCR = static_cast<u32>(PixelFormat::L8); // 0x5
  DMA2D->FGMAR = reinterpret_cast<u32>(src.pixels);
  DMA2D->FGOR = 0;

  // Fire
  DMA2D->CR |= DMA2D_CR_START;
}

} // namespace stm
} // namespace hal
} // namespace ge
