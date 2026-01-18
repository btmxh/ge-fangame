#pragma once

#define GE_SDRAM __attribute__((section(".sdram")))

namespace ge {
namespace hal {
namespace stm {

void init_sdram();

}
} // namespace hal
} // namespace ge
