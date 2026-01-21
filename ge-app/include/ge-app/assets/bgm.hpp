#pragma once

#include "ge-hal/core.hpp"
namespace ge {
namespace assets {

struct Bgm {
  const u8 *data;
  const u32 length;

  static const Bgm &menu();
  static const Bgm &ambient();
};

} // namespace assets
} // namespace ge
