#pragma once

#include "ge-hal/surface.hpp"
namespace ge {
namespace hal {

// two lies:
// - on PC backend, this is implemented using raw CPU memory copies
// - on STM32 backend, this is called DMA2D
namespace gpu {

void fill(Surface dst, u32 color);
void blit(Surface dst, Surface src);
void blit_blend(Surface dst, Surface src, u8 global_alpha);

void load_palette(u32 const *colors, int count);
void blit_indexed(Surface dst, Surface src);
void wait_idle();

} // namespace gpu

} // namespace hal
} // namespace ge
