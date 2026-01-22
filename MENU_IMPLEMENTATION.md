# Menu System Implementation

This document describes the menu system implementation for the GE-Fangame project.

## Overview

The game now starts with a main menu instead of jumping directly into gameplay. The menu system is designed to be modular and reusable across different scenes.

## Architecture

### Components

1. **UI Menu Component** (`ge-app/include/ge-app/ui/menu.hpp`)
   - Reusable menu widget that can be used in any scene
   - Handles rendering of menu items with selection indicator
   - Manages joystick navigation with configurable thresholds
   - Caches text widths for optimal performance

2. **MenuScene** (`ge-app/include/ge-app/scenes/menu_scene.hpp`)
   - Main menu scene with title, subtitle, and menu options
   - Currently provides "Start Game" and "Exit" options
   - Can be extended with additional menu items

3. **Scene Management** (`ge-app/src/main.cpp`)
   - MainApp now supports switching between different scenes
   - Uses unique_ptr for proper memory management
   - Delegates input events to the current scene

### Input Controls

- **Joystick Y-axis**: Navigate menu items (up/down)
  - Threshold for movement: ±0.5
  - Threshold for center detection: ±0.3
- **Button 1**: Select the highlighted menu item
- **Button 2**: Currently has no function in the menu (reserved for future use)

## Key Features

### Performance Optimizations

1. **Text Width Caching**: Menu items cache their text widths on first render
2. **Pre-calculated Layouts**: Title and subtitle widths are calculated once in the constructor
3. **Efficient Rendering**: Only renders necessary elements, no redundant calculations

### Type Safety

1. **Enum Class**: MenuAction uses `enum class` for strong typing
2. **Proper Type Conversions**: All size_t and int conversions are explicit
3. **Const Correctness**: Methods marked const where appropriate

### Modularity

1. **Reusable Components**: The Menu class can be used in GameScene or other future scenes
2. **Clean Separation**: UI logic is separate from scene logic
3. **Extensible Design**: Easy to add new menu items or create new scenes

## Usage Example

To use the menu component in a different scene:

```cpp
#include "ge-app/ui/menu.hpp"

class MyScene : public Scene {
public:
  MyScene(App &app) : Scene{app} {
    menu.add_item("Option 1", 1);
    menu.add_item("Option 2", 2);
  }

  void tick(float dt) override {
    auto joystick = app.get_joystick_state();
    menu.move_selection(joystick.y);
  }

  void render(Surface &fb) override {
    menu.render(fb, Font::regular_font());
  }

  void on_button_clicked(Button btn) override {
    if (btn == Button::Button1) {
      int selected = menu.get_selected_id();
      // Handle selection
    }
  }

private:
  ui::Menu menu;
};
```

## Files Modified

- `ge-app/CMakeLists.txt`: Added new header files
- `ge-app/include/ge-app/font.hpp`: Added get_glyph_width() method
- `ge-app/src/ge-app/font.cpp`: Implemented get_glyph_width() method
- `ge-app/src/main.cpp`: Added scene management and menu integration
- `ge-hal/include/ge-hal/app.hpp`: Added request_quit() method
- `ge-hal/src/pc/app.cpp`: Implemented request_quit() for PC platform
- `ge-hal/src/stm/app.cpp`: Added no-op request_quit() for STM32 platform

## Files Added

- `ge-app/include/ge-app/ui/menu.hpp`: Reusable menu UI component
- `ge-app/include/ge-app/scenes/menu_scene.hpp`: Main menu scene implementation

## Future Enhancements

Potential improvements for the menu system:

1. **Animations**: Add fade-in/fade-out transitions between scenes
2. **Sound Effects**: Play sounds when navigating or selecting menu items
3. **Settings Menu**: Add options for audio volume, controls, etc.
4. **Pause Menu**: Use the menu component in GameScene for a pause menu
5. **Visual Effects**: Add particle effects or animated backgrounds

## Testing

The implementation should be tested to verify:

- [x] Menu appears on startup
- [ ] Joystick navigation works correctly (up/down movement)
- [ ] Button 1 selects the highlighted option
- [ ] "Start Game" transitions to GameScene
- [ ] "Exit" closes the application (PC only)
- [ ] Menu renders correctly on both PC and STM32 displays

Note: Due to build environment constraints, full testing requires the Nix development environment or manual setup of SDL3 and build tools.
