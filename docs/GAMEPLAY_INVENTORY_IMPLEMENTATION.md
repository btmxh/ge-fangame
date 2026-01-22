# Gameplay Management UI and Inventory System Implementation

This document describes the gameplay management UI and inventory system implementation for the GE-Fangame project.

## Overview

The game now includes a comprehensive management system that allows players to:
- View their status and statistics
- Manage their inventory of caught fish
- Access management features through the existing Management game mode

## Architecture

### New Components

1. **Inventory System** (`ge-app/include/ge-app/game/inventory.hpp`)
   - Stores caught fish with name, rarity, and timestamp
   - Maximum capacity of 50 items
   - Provides methods to add, retrieve, and query fish by rarity
   - No heap allocation - uses fixed-size array

2. **Management Menu Scene** (`ge-app/include/ge-app/scenes/management_menu_scene.hpp`)
   - Main entry point for management UI
   - Provides menu options: View Status, View Inventory, Back to Game
   - Uses existing menu UI component

3. **Status Scene** (`ge-app/include/ge-app/scenes/status_scene.hpp`)
   - Displays player statistics and game state
   - Shows current time, day, and game mode
   - Displays inventory summary with counts by rarity
   - Color-coded rarity display

4. **Inventory Scene** (`ge-app/include/ge-app/scenes/inventory_scene.hpp`)
   - Shows detailed list of all caught fish
   - Scrollable interface for large inventories
   - Color-coded by rarity
   - Shows fish name and rarity for each item

### Enhanced Components

1. **Fishing System** (Updated `ge-app/include/ge-app/game/fishing.hpp`)
   - Now saves caught fish to inventory
   - Weighted random rarity system for realistic catches
   - Displays inventory status after catching fish
   - Handles inventory full condition

2. **Game Scene** (Updated `ge-app/include/ge-app/scenes/game_scene.hpp`)
   - Contains player inventory
   - Connects inventory to fishing system
   - Switches to Management Menu when entering Management mode
   - Provides getters for Clock, Inventory, and ModeIndicator

3. **Main App** (Updated `ge-app/src/main.cpp`)
   - Added three new scene types: ManagementMenu, Status, Inventory
   - Implemented scene switching for management UI
   - Connected all scenes with proper navigation flow

## Fish Rarity System

Fish are categorized into four rarity tiers with weighted random selection:

### Common Fish (75 total weight)
- **Tropical Fish** (weight: 30) - Most common
- **Sea Bass** (weight: 25) - Very common
- **Sardine** (weight: 20) - Common

### Uncommon Fish (37 total weight)
- **Tuna** (weight: 15)
- **Salmon** (weight: 15)
- **Pufferfish** (weight: 10)
- **Old Boot** (weight: 7) - Loot item

### Rare Fish (13 total weight)
- **Clownfish** (weight: 8)
- **Golden Fish** (weight: 5)

### Legendary Items (2 total weight)
- **Treasure Chest** (weight: 2) - Rarest catch!

Total weight: 127

The weighted system ensures common fish appear ~59% of the time, uncommon ~29%, rare ~10%, and legendary ~2%.

## Inventory Data Structure

### FishItem
```cpp
struct FishItem {
  const char *name;
  FishRarity rarity;
  i64 caught_time; // Timestamp in milliseconds
};
```

### FishRarity Enum
```cpp
enum class FishRarity {
  Common,
  Uncommon,
  Rare,
  Legendary
};
```

### Inventory Class
- **Capacity**: 50 items maximum
- **Methods**:
  - `add_fish()` - Add fish to inventory
  - `get_item_count()` - Get total items
  - `get_item()` - Retrieve item by index
  - `get_count_by_rarity()` - Count fish of specific rarity
  - `is_full()` - Check if inventory is at capacity
  - `clear()` - Remove all items

## Scene Navigation Flow

```
Main Menu
    ↓
Game Scene (Steering/Fishing/Management modes)
    ↓ (Press B to cycle modes, reach Management)
    ↓
Management Menu Scene
    ├─→ View Status → Status Scene
    │                     ↓ (Press B to go back)
    │                     Management Menu
    │
    ├─→ View Inventory → Inventory Scene
    │                         ↓ (Press B to go back)
    │                         Management Menu
    │
    └─→ Back to Game → Game Scene
```

## Controls

### Game Scene (All Modes)
- **Button 2 (B)**: Cycle game modes (Steering → Fishing → Management)
- **Button 1 (A)**: Context-specific actions

### Management Menu Scene
- **Joystick Y**: Navigate menu items (up/down)
- **Button 1 (A)**: Select menu item

### Status Scene
- **Button 2 (B)**: Return to Management Menu

### Inventory Scene
- **Joystick Y**: Scroll through fish list (up/down)
- **Button 2 (B)**: Return to Management Menu

## Status Screen Display

The Status Scene shows:
1. **Title**: "=== Player Status ==="
2. **Game Time**: Current day and time (e.g., "Day 1, 6 AM (1.0x)")
3. **Current Mode**: Steering/Fishing/Management
4. **Inventory Stats**:
   - Total fish count (e.g., "Total Fish: 15 / 50")
   - Common count (white)
   - Uncommon count (green)
   - Rare count (blue)
   - Legendary count (magenta)
5. **Instructions**: "Press B to return"

## Inventory Screen Display

The Inventory Scene shows:
1. **Title**: "=== Inventory (X/50) ===" where X is current count
2. **Fish List**: Numbered list with name and rarity
   - Common: White text
   - Uncommon: Green text
   - Rare: Blue text
   - Legendary: Magenta text
3. **Scroll Indicators**: "^ More above" / "v More below" when applicable
4. **Empty State**: "No fish caught yet!" if inventory is empty
5. **Instructions**: "Press B to return"

The list is scrollable with up to 8 items visible at once.

## Integration Points

### Fishing System
When a fish is caught:
1. Weighted random selection determines fish and rarity
2. Fish is added to inventory (if not full)
3. Dialog shows catch result with inventory count
4. If inventory is full, shows "Inventory full!" message

### Game Scene
The GameScene:
- Creates and manages the Inventory instance
- Passes inventory reference to Fishing system
- Provides getters for Clock, Inventory, and ModeIndicator
- Switches to Management Menu when entering Management mode

### Scene Management
The MainApp manages all scenes:
- MenuSceneImpl - Main menu
- GameSceneImpl - Gameplay
- SettingsSceneImpl - Settings
- CreditsSceneImpl - Credits
- ManagementMenuSceneImpl - Management menu
- StatusSceneImpl - Status display
- InventorySceneImpl - Inventory display

## Performance Considerations

### No Dynamic Allocation
- Inventory uses fixed 50-item array
- All scenes use static storage in MainApp
- No heap allocations during gameplay

### Efficient Rendering
- Status scene renders once per frame
- Inventory scene only renders visible items (8 at a time)
- Color calculations are simple bit operations

### Minimal Memory Footprint
- FishItem: ~16 bytes (pointer + enum + i64)
- Inventory: ~800 bytes (50 items + metadata)
- All scenes: Static storage in MainApp

## Files Created

1. **ge-app/include/ge-app/game/inventory.hpp** - Inventory data structure
2. **ge-app/include/ge-app/scenes/management_menu_scene.hpp** - Management menu
3. **ge-app/include/ge-app/scenes/status_scene.hpp** - Status display
4. **ge-app/include/ge-app/scenes/inventory_scene.hpp** - Inventory display
5. **docs/GAMEPLAY_INVENTORY_IMPLEMENTATION.md** - This documentation

## Files Modified

1. **ge-app/include/ge-app/game/fishing.hpp**
   - Added inventory integration
   - Implemented weighted rarity system
   - Added fish data with rarities

2. **ge-app/include/ge-app/scenes/game_scene.hpp**
   - Added Inventory member
   - Connected inventory to fishing
   - Added management mode switching
   - Added getter methods

3. **ge-app/src/main.cpp**
   - Added three new scene types
   - Implemented GameSceneImpl wrapper
   - Added scene switching methods
   - Connected all navigation

4. **ge-app/CMakeLists.txt**
   - Added new header files to build

## Testing Recommendations

### Status Screen Testing
- [ ] Verify status display shows correct game time
- [ ] Verify mode indicator shows current mode
- [ ] Verify inventory counts match actual caught fish
- [ ] Verify rarity counts are accurate
- [ ] Verify back button returns to management menu

### Inventory Screen Testing
- [ ] Catch multiple fish and verify they appear in inventory
- [ ] Verify scrolling works with more than 8 fish
- [ ] Verify fish are color-coded by rarity
- [ ] Verify empty state displays when no fish caught
- [ ] Verify back button returns to management menu

### Fishing Integration Testing
- [ ] Catch fish and verify they are added to inventory
- [ ] Verify inventory count updates in dialog
- [ ] Fill inventory to 50 items and verify "full" message
- [ ] Verify rarity distribution matches weights

### Navigation Testing
- [ ] Cycle to Management mode from game
- [ ] Navigate to Status scene and back
- [ ] Navigate to Inventory scene and back
- [ ] Return to game from management menu
- [ ] Verify game state is preserved during management

## Known Limitations

1. **No Persistence**: Inventory resets when application closes
2. **Fixed Capacity**: Maximum 50 fish, no overflow handling
3. **Simple Display**: Inventory shows basic name and rarity only
4. **No Sorting**: Fish displayed in order caught
5. **No Deletion**: Cannot remove individual fish from inventory

## Future Enhancements

1. **Persistence**: Save inventory to flash memory on STM32
2. **Fish Details**: Show catch time, size, or other stats
3. **Sorting Options**: Sort by rarity, name, or catch time
4. **Fish Release**: Allow players to free fish to make room
5. **Sell System**: Trade fish for currency or items
6. **Achievements**: Track fishing milestones
7. **Fish Encyclopedia**: Record all discovered species
8. **Export Function**: Export inventory data for analysis

## Color Codes (RGB565)

- **Common**: 0xFFFF (White)
- **Uncommon**: 0x07E0 (Green)
- **Rare**: 0x001F (Blue)
- **Legendary**: 0xF81F (Magenta)
- **UI Text**: 0x7BEF (Light gray)
- **Title**: 0xFFFF (White)
- **Time Display**: 0xF7BE (Light yellow)

## Conclusion

This implementation provides a complete gameplay management UI with inventory system:
- ✅ Status screen showing player stats
- ✅ Inventory management with scrollable list
- ✅ Fish rarity system with weighted distribution
- ✅ Seamless integration with existing game modes
- ✅ Embedded-friendly with no dynamic allocation
- ✅ Comprehensive navigation flow

The system enhances the fishing gameplay by providing meaningful progression and collection mechanics while maintaining the project's performance and memory constraints.
