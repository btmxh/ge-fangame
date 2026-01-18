#pragma once

#include "ge-hal/core.hpp"

namespace ge {
namespace hal {
namespace stm {

void init_ltdc();

u16 *pixel_buffer(int buffer_index);

}
} // namespace hal
} // namespace ge
