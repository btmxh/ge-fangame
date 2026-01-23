# Navigation and UI Flow Diagram

## Complete Scene Flow

```
┌─────────────────────────────────────────────────────────────────────────┐
│                          Application Start                               │
└────────────────────────────────┬────────────────────────────────────────┘
                                 │
                                 ▼
                  ┌──────────────────────────────┐
                  │      Main Menu Scene         │
                  │  • Start Game                │
                  │  • Options                   │
                  │  • Credits                   │
                  │  • Exit                      │
                  └──────────┬───────────────────┘
                             │
                   ┌─────────┴──────────┐
                   │                    │
                   ▼                    ▼
        ┌──────────────────┐   ┌──────────────────┐
        │  Settings Scene  │   │  Credits Scene   │
        │  • Music Volume  │   │  • Game Credits  │
        │  • SFX Volume    │   │  • Contributors  │
        │  • Button Flip   │   │                  │
        │  • Back          │   │  • Back          │
        └──────────────────┘   └──────────────────┘
                   │                    │
                   └─────────┬──────────┘
                             │
                             ▼
                  ┌──────────────────────────────┐
                  │        Game Scene            │
                  │                              │
                  │  ┌────────────────────────┐  │
                  │  │  Game Modes (Cycle B)  │  │
                  │  ├────────────────────────┤  │
                  │  │ 1. Steering Mode       │  │
                  │  │    • Navigate boat     │  │
                  │  │    • Explore sea       │  │
                  │  ├────────────────────────┤  │
                  │  │ 2. Fishing Mode        │  │
                  │  │    • Cast line         │  │
                  │  │    • Catch fish        │  │
                  │  │    • Build inventory   │  │
                  │  ├────────────────────────┤  │
                  │  │ 3. Management Mode ─┐  │  │
                  │  │    • Pause game      │  │  │
                  │  └────────────────────┬─┘  │  │
                  └─────────────────────┬─│────┘  │
                                        │ │       │
                        ┌───────────────┘ │       │
                        │                 │       │
                        ▼                 │       │
         ┌──────────────────────────────┐ │       │
         │  Management Menu Scene       │◄┘       │
         │                              │         │
         │  ┌────────────────────────┐  │         │
         │  │ • View Status          │  │         │
         │  │ • View Inventory       │  │         │
         │  │ • Back to Game         │  │         │
         │  └────────────────────────┘  │         │
         └────┬────────────────┬────────┘         │
              │                │                  │
     ┌────────┴─────┐    ┌────┴────────┐         │
     ▼              ▼    ▼             ▼         │
┌─────────────┐  ┌──────────────┐               │
│Status Scene │  │Inventory Scene│               │
│             │  │              │               │
│• Game Time  │  │• Fish List   │               │
│• Mode       │  │• Scrollable  │               │
│• Inventory  │  │• Color-coded │               │
│  Summary    │  │• Rarities    │               │
│             │  │              │               │
│• Total Fish │  │• 8 visible   │               │
│• Common: N  │  │  at once     │               │
│• Uncommon: N│  │              │               │
│• Rare: N    │  │• Empty state │               │
│• Legend.: N │  │  handling    │               │
│             │  │              │               │
│[Press B]    │  │[Press B]     │               │
└──────┬──────┘  └──────┬───────┘               │
       │                │                       │
       └────────┬───────┘                       │
                │                               │
                ▼                               │
        Back to Management Menu                 │
                │                               │
                └───────────────────────────────┘
                   (Back to Game option)
```

## Inventory Data Flow

```
┌────────────────────────────────────────────────────────────────┐
│                        Fishing System                           │
│                                                                 │
│  1. Player casts line (joystick flick)                         │
│  2. Wait for fish bite (2-15 seconds)                          │
│  3. Fish bites (wiggle intensity increases)                    │
│  4. Player presses A to catch (within 3.5s)                    │
│                    │                                            │
│                    ▼                                            │
│         ┌──────────────────────┐                               │
│         │  Weighted Selection  │                               │
│         ├──────────────────────┤                               │
│         │ Common:     ~59%     │                               │
│         │ Uncommon:   ~29%     │                               │
│         │ Rare:       ~10%     │                               │
│         │ Legendary:   ~2%     │                               │
│         └──────────┬───────────┘                               │
│                    │                                            │
└────────────────────┼────────────────────────────────────────────┘
                     │
                     ▼
        ┌────────────────────────┐
        │    FishItem Created     │
        │  • name: "Tropical Fish"│
        │  • rarity: Common       │
        │  • caught_time: 12345ms │
        └────────────┬───────────┘
                     │
                     ▼
        ┌────────────────────────┐
        │  Inventory::add_fish() │
        │                        │
        │  if (count < 50) {     │
        │    items[count++] = fish│
        │    return true         │
        │  }                     │
        │  return false (FULL)   │
        └────────────┬───────────┘
                     │
                     ▼
        ┌────────────────────────┐
        │   Display Dialog       │
        │                        │
        │ "Caught: Tropical Fish!│
        │  Inventory: 15/50"     │
        │                        │
        │  OR                    │
        │                        │
        │ "Inventory full!       │
        │  Cannot store fish."   │
        └────────────────────────┘
```

## Status Screen Layout

```
┌─────────────────────────────────────────────┐
│ === Player Status ===                       │
│                                             │
│ Day 1, 6 AM (1.0x)                         │
│ Current Mode: Fishing                       │
│                                             │
│ Inventory:                                  │
│   Total Fish: 15 / 50                       │
│   Common: 10        (white)                 │
│   Uncommon: 3       (green)                 │
│   Rare: 1           (blue)                  │
│   Legendary: 1      (magenta)               │
│                                             │
│                                             │
│                                             │
│                                             │
│                                             │
│ Press B to return                           │
└─────────────────────────────────────────────┘
```

## Inventory Screen Layout

```
┌─────────────────────────────────────────────┐
│ === Inventory (15/50) ===                   │
│                                             │
│ ^ More above                                │
│                                             │
│ 5. Tropical Fish (Common)     [white]       │
│ 6. Sea Bass (Common)          [white]       │
│ 7. Tuna (Uncommon)            [green]       │
│ 8. Clownfish (Rare)           [blue]        │
│ 9. Sardine (Common)           [white]       │
│ 10. Treasure Chest (Legendary) [magenta]    │
│ 11. Pufferfish (Uncommon)     [green]       │
│ 12. Golden Fish (Rare)        [blue]        │
│                                             │
│ v More below                                │
│                                             │
│ Press B to return                           │
└─────────────────────────────────────────────┘
```

## Management Menu Layout

```
┌─────────────────────────────────────────────┐
│ === Management ===                          │
│                                             │
│ Select an option:                           │
│                                             │
│   ► View Status                             │
│     View Inventory                          │
│     Back to Game                            │
│                                             │
│                                             │
│                                             │
│                                             │
│                                             │
│                                             │
│                                             │
│                                             │
│                                             │
└─────────────────────────────────────────────┘
```

## Color Codes Reference

| Element | Color | RGB565 | Usage |
|---------|-------|--------|-------|
| Common Fish | White | 0xFFFF | Most frequent catches |
| Uncommon Fish | Green | 0x07E0 | Moderate catches |
| Rare Fish | Blue | 0x001F | Valuable catches |
| Legendary Items | Magenta | 0xF81F | Rarest treasures |
| UI Text | Light Gray | 0x7BEF | Instructions |
| Title | White | 0xFFFF | Scene headers |
| Time Display | Light Yellow | 0xF7BE | Game clock |

## Button Mappings

### Universal Controls
- **Button 2 (B)**: Back/Cancel/Mode Switch
- **Button 1 (A)**: Confirm/Select

### Context-Specific Controls

#### Game Scene - Steering Mode
- **Joystick**: Steer boat
- **Button 2 (B)**: Switch to Fishing mode

#### Game Scene - Fishing Mode
- **Joystick Flick**: Cast fishing line
- **Button 1 (A)**: Catch fish (when biting)
- **Button 2 (B)**: Switch to Management mode

#### Game Scene - Management Mode
- Automatically switches to Management Menu Scene

#### Management Menu
- **Joystick Y**: Navigate menu (up/down)
- **Button 1 (A)**: Select option

#### Status Scene
- **Button 2 (B)**: Return to Management Menu

#### Inventory Scene
- **Joystick Y**: Scroll list (up/down)
- **Button 2 (B)**: Return to Management Menu

## Fish Rarity Distribution Chart

```
Distribution of 1000 catches (theoretical):

Common:     ████████████████████████████████████████████████████ 590 (59%)
Uncommon:   ██████████████████████████ 290 (29%)
Rare:       ██████████ 100 (10%)
Legendary:  █ 20 (2%)
```

## Implementation Architecture

```
┌─────────────────────────────────────────────────────────┐
│                        MainApp                           │
│  • Manages all scene instances (static storage)         │
│  • Handles scene switching via methods                  │
│  • Delegates input to current_scene                     │
└────────────────────┬────────────────────────────────────┘
                     │
         ┌───────────┼───────────┬──────────────┬─────────────┐
         │           │           │              │             │
         ▼           ▼           ▼              ▼             ▼
    ┌────────┐ ┌─────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐
    │ Menu   │ │ Game    │ │ Settings │ │ Credits  │ │Management│
    │ Scene  │ │ Scene   │ │ Scene    │ │ Scene    │ │  Scenes  │
    └────────┘ └────┬────┘ └──────────┘ └──────────┘ └─────┬────┘
                    │                                       │
                    │                                       │
         ┌──────────┴──────────┐                  ┌────────┴────────┐
         │                     │                  │                 │
         ▼                     ▼                  ▼                 ▼
    ┌──────────┐         ┌──────────┐      ┌──────────┐     ┌──────────┐
    │Inventory │         │ Fishing  │      │  Status  │     │Inventory │
    │ (data)   │◄────────│  System  │      │  Scene   │     │  Scene   │
    └──────────┘         └──────────┘      └──────────┘     └──────────┘
         │                     │                  │                │
         │                     │                  │                │
         └─────────────────────┴──────────────────┴────────────────┘
                          Shared Game State
                      (Clock, Inventory, ModeIndicator)
```
