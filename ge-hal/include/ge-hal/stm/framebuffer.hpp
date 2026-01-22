#pragma once

#include "ge-hal/core.hpp"

namespace ge {
namespace hal {
namespace stm {

void init_ltdc();

u16 *pixel_buffer(int buffer_index);

// Begin a new frame - returns true if vblank occurred and we should render
bool begin_frame(u32 &buffer_index);

} // namespace stm
} // namespace hal
} // namespace ge
