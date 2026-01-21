#include "ge-app/assets/bgm.hpp"

#include "assets/out/sounds/ambient-bgm.h"
#include "assets/out/sounds/menu-bgm.h"

namespace ge {
namespace assets {

const Bgm &Bgm::ambient() {
  static const Bgm bgm{bgm_ambient, bgm_ambient_len};
  return bgm;
}

const Bgm &Bgm::menu() {
  static const Bgm bgm{bgm_menu, bgm_menu_len};
  return bgm;
}

} // namespace assets
} // namespace ge
