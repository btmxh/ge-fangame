#include "ge-app/scenes/game/management/menu.hpp"
#include "ge-app/scenes/game/management/main.hpp"

namespace ge {
namespace scenes {
namespace game {
namespace mgmt {

MenuScene::MenuScene(ManagementUIScene &parent)
    : Scene{parent.get_app()}, parent{parent} {
  menu_items[0] =
      ui::MenuItem{"View Status", static_cast<int>(Action::ViewStatus)};
  menu_items[1] =
      ui::MenuItem{"View Inventory", static_cast<int>(Action::ViewInventory)};
  menu_items[2] = ui::MenuItem{"View Map", static_cast<int>(Action::ViewMap)};
  menu.set_items(menu_items, GE_ARRAY_SIZE(menu_items));
}

void MenuScene::on_menu_action(Action action) {
  switch (action) {
  case Action::ViewStatus:
    parent.show_status_screen();
    break;
  case Action::ViewInventory:
    parent.show_inventory_screen();
    break;
  case Action::ViewMap:
    parent.show_map_screen();
    break;
  default:
    break;
  }
}

bool MenuScene::is_active() const {
  return parent.is_screen_active(ManagementUIScreen::Menu);
}
void MenuScene::render(Surface &fb_region) {
  auto fb = parent.render_bg(fb_region);

  // Title
  Font::regular_font().render_colored("Management", -1, fb, 10, 10, 0x0000);

  // Subtitle
  Font::regular_font().render_colored("Select an option:", -1, fb, 10, 24,
                                      0x7BEF);

  // Render menu
  auto menu_region = fb.subsurface(0, 40, fb.get_width(), fb.get_height() - 40);
  menu.render(menu_region, Font::regular_font());
}
} // namespace mgmt
} // namespace game
} // namespace scenes
} // namespace ge
