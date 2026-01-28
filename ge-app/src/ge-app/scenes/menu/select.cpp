#include "ge-app/scenes/menu/select.hpp"

#include "assets/out/textures/menu-bg.h"
#include "ge-app/font.hpp"
#include "ge-app/scenes/menu/main.hpp"
#include "ge-hal/gpu.hpp"

namespace ge {
namespace scenes {
namespace menu {

MenuSelectScene::MenuSelectScene(MenuScene &parent)
    : Scene{parent.get_app()}, parent{parent},
      subtitle{"A Fangame by CTB Girls' Dorm."},
      menu_bg_texture{menu_bg, menu_bg_WIDTH, menu_bg_HEIGHT,
                      menu_bg_FORMAT_CPP} {
  menu_items[0] = {"Start Game", static_cast<int>(MenuAction::StartGame)};
  menu_items[1] = {"Options", static_cast<int>(MenuAction::Options)};
  menu_items[2] = {"Credits", static_cast<int>(MenuAction::Credits)};
  menu_items[3] = {"Exit", static_cast<int>(MenuAction::ExitGame)};

  menu.set_items(menu_items, 4);
}

void MenuSelectScene::tick(float /*dt*/) {
  auto joystick = app.get_joystick_state();
  menu.move_selection(joystick.y);
}

void MenuSelectScene::render(Surface &fb_region) {
  hal::gpu::blit(fb_region, menu_bg_texture);

  Font::regular_font().render_colored(subtitle, -1, fb_region, 80, 90, 0xFFFF);

  auto menu_region = fb_region.subsurface(0, 96, fb_region.get_width(),
                                          fb_region.get_height() - 100);

  menu.render(menu_region, Font::regular_font());
}

bool MenuSelectScene::on_button_clicked(Button btn) {
  if (btn == Button::Button1) {
    int selected = menu.get_selected_id();
    on_menu_action(static_cast<MenuAction>(selected));
    return true;
  }
  return Scene::on_button_clicked(btn);
}

void MenuSelectScene::on_menu_action(MenuAction action) {
  switch (action) {
  case MenuAction::StartGame:
    parent.start_game();
    break;
  case MenuAction::Options:
    parent.show_settings();
    break;
  case MenuAction::Credits:
    parent.show_credits();
    break;
  case MenuAction::ExitGame:
    parent.exit();
  default:
    break;
  }
}

bool MenuSelectScene::is_active() const {
  return parent.is_current_screen(MenuSceneScreen::Select);
}
} // namespace menu
} // namespace scenes
} // namespace ge
