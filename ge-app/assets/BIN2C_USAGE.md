# bin2c Asset Pipeline Usage Guide

This document describes how to use the bin2c asset pipeline to convert various asset types (images, audio, fonts) into C/H header files for embedding in the application.

## Table of Contents

- [Basic Image Conversion](#basic-image-conversion)
- [Animated Images](#animated-images)
- [Rotated Sprite Sheets](#rotated-sprite-sheets)
- [Audio Files](#audio-files)
- [Bitmap Fonts](#bitmap-fonts)
- [Advanced Usage](#advanced-usage)

## Basic Image Conversion

### CMake Functions

#### `raw_image(SYMBOL_NAME IMAGE_FILE [ARGS ...])`

Converts an image to RGB565 format (no alpha channel).

```cmake
raw_image(water_texture out/textures/watertexture.png)
```

#### `raw_image_alpha(SYMBOL_NAME IMAGE_FILE [ARGS ...])`

Converts an image to ARGB1555 format (with 1-bit alpha channel).

```cmake
raw_image_alpha(default_boat out/textures/default-boat.png)
```

### Custom Arguments

You can override the default color mode by passing `ARGS`:

```cmake
# Use rgb565 instead of the default argb1555
raw_image_alpha(my_sprite out/textures/sprite.png ARGS rgb565)
```

### Direct Script Usage

```bash
# Basic usage - RGB565 (no alpha)
python3 scripts/bin2c_image.py input.png output.c output.h symbol_name rgb565

# With alpha channel - ARGB1555
python3 scripts/bin2c_image.py input.png output.c output.h symbol_name argb1555
```

### Generated Output

The generated header file will contain:

```c
#define symbol_name_WIDTH 64
#define symbol_name_HEIGHT 64
extern const uint16_t symbol_name[];
extern const uint32_t symbol_name_len;
```

## Animated Images

The bin2c_image script supports animated images in formats like APNG, WEBP, and GIF. It extracts all frames and creates a horizontal spritesheet.

### CMake Function

#### `raw_image_animated(SYMBOL_NAME IMAGE_FILE [MODE mode] [ARGS ...])`

```cmake
# Basic animated image (defaults to argb1555)
raw_image_animated(explosion_anim out/effects/explosion.gif)

# With custom mode
raw_image_animated(water_anim out/effects/water.webp MODE rgb565)
```

### Direct Script Usage

```bash
python3 scripts/bin2c_image.py animation.gif output.c output.h anim_symbol argb1555 --animated
```

### Generated Output

For animated images, the header includes frame information:

```c
#define anim_symbol_WIDTH 256          // Total spritesheet width (frame_width * frame_count)
#define anim_symbol_HEIGHT 64           // Frame height
#define anim_symbol_FRAME_WIDTH 64      // Individual frame width
#define anim_symbol_FRAME_HEIGHT 64     // Individual frame height
#define anim_symbol_FRAME_COUNT 4       // Number of frames

// Frame durations in milliseconds
static const unsigned short anim_symbol_FRAME_DURATIONS[] = {100, 100, 100, 100};

extern const uint16_t anim_symbol[];
extern const uint32_t anim_symbol_len;
```

### Usage in Code

```c
// Calculate frame offset
int frame_index = (current_time_ms / anim_symbol_FRAME_DURATIONS[0]) % anim_symbol_FRAME_COUNT;
int frame_offset = frame_index * anim_symbol_FRAME_WIDTH;

// Draw the current frame
draw_sprite(&anim_symbol[frame_offset * anim_symbol_FRAME_HEIGHT], 
            anim_symbol_FRAME_WIDTH, 
            anim_symbol_FRAME_HEIGHT);
```

## Rotated Sprite Sheets

Generate a spritesheet containing multiple rotations of an image. Useful for sprites that need to rotate (e.g., compass needles, spinning objects, rotating characters).

### CMake Function

#### `raw_image_rotated(SYMBOL_NAME IMAGE_FILE ROTATION_COUNT [MODE mode] [ARGS ...])`

```cmake
# Generate 16 rotations of a compass needle
raw_image_rotated(compass_needle_rotated out/textures/compass-needle.png 16)

# Generate 8 rotations with custom mode
raw_image_rotated(spinning_coin out/textures/coin.png 8 MODE rgb565)
```

### Direct Script Usage

```bash
# Generate 16 rotated frames
python3 scripts/bin2c_image.py compass.png output.c output.h rotated_sprite argb1555 --rotate 16
```

### Generated Output

```c
#define rotated_sprite_WIDTH 768        // Total spritesheet width (48px * 16 rotations)
#define rotated_sprite_HEIGHT 48        // Frame height
#define rotated_sprite_FRAME_WIDTH 48   // Individual frame width
#define rotated_sprite_FRAME_HEIGHT 48  // Individual frame height
#define rotated_sprite_FRAME_COUNT 16   // Number of rotation frames
#define rotated_sprite_ROTATION_COUNT 16

extern const uint16_t rotated_sprite[];
extern const uint32_t rotated_sprite_len;
```

### Usage in Code

```c
// Get rotation frame based on angle (0-359 degrees)
int frame_index = (angle * rotated_sprite_ROTATION_COUNT) / 360;
int frame_offset = frame_index * rotated_sprite_FRAME_WIDTH;

// Draw the rotated frame
draw_sprite(&rotated_sprite[frame_offset * rotated_sprite_FRAME_HEIGHT],
            rotated_sprite_FRAME_WIDTH,
            rotated_sprite_FRAME_HEIGHT);
```

## Audio Files

### CMake Function

#### `raw_audio(SYMBOL_NAME WAV_FILE [ARGS ...])`

Converts WAV files (must be 8kHz, int16) to 8-bit unsigned format.

```cmake
raw_audio(bgm_ambient out/sounds/ambient-bgm.wav)
raw_audio(sfx_explosion out/sounds/explosion.wav)
```

### Direct Script Usage

```bash
python3 scripts/bin2c_audio.py input.wav output.c output.h symbol_name
```

### Requirements

- Audio must be 8kHz sample rate
- Format: int16

## Bitmap Fonts

### CMake Function

#### `bitmap_font(SYMBOL_NAME FONT_FILE FONT_SIZE [ARGS ...])`

Converts TrueType/OpenType fonts to bitmap fonts.

```cmake
bitmap_font(font_pixeloid_9px src/fonts/Pixeloid/TTF/PixeloidSans.ttf 9)
bitmap_font(font_pixeloid_18px src/fonts/Pixeloid/TTF/PixeloidSans.ttf 18)
```

### Direct Script Usage

```bash
python3 scripts/bin2c_bitmap_font.py font.ttf output.c output.h symbol_name 12
```

### Generated Output

```c
#define symbol_name_FIRST_CHAR 32
#define symbol_name_LAST_CHAR 126
#define symbol_name_CELL_WIDTH 16
#define symbol_name_CELL_HEIGHT 12
#define symbol_name_BYTES_PER_ROW 2

static const unsigned char symbol_name_ADVANCES[] = {4,5,6,...};

extern const uint8_t symbol_name[];
extern const uint32_t symbol_name_len;
```

## Advanced Usage

### Combining Features

While you can't combine rotation and animation in a single call, you can process the same image multiple ways:

```cmake
# Original static image
raw_image_alpha(coin out/textures/coin.png)

# Rotated version
raw_image_rotated(coin_rotated out/textures/coin.png 12)

# Animated version (if coin.png is an animated image)
raw_image_animated(coin_anim out/textures/coin_animated.webp)
```

### Custom Color Modes

Both image conversion modes are supported:

- **rgb565**: 16-bit color, no alpha (5 bits red, 6 bits green, 5 bits blue)
- **argb1555**: 16-bit color with 1-bit alpha (1 bit alpha, 5 bits each for R/G/B)

### Passing Additional Arguments

All wrapper functions support passing additional arguments via the `ARGS` parameter:

```cmake
# Pass custom arguments to the underlying script
raw_image_alpha(sprite out/sprite.png ARGS --custom-flag value)
```

## Color Format Details

### RGB565

- Total: 16 bits per pixel
- Red: 5 bits (0-31)
- Green: 6 bits (0-63)
- Blue: 5 bits (0-31)
- No alpha channel
- Best for: Opaque images, backgrounds, textures

### ARGB1555

- Total: 16 bits per pixel
- Alpha: 1 bit (0=transparent, 1=opaque)
- Red: 5 bits (0-31)
- Green: 5 bits (0-31)
- Blue: 5 bits (0-31)
- Best for: Sprites with transparency, UI elements

## Examples

### Complete Example: Rotating Compass Needle

```cmake
# In CMakeLists.txt
raw_image_rotated(compass_needle_rot out/textures/needle.png 32)
```

```c
// In your C code
#include "assets/out/textures/needle.h"

void draw_compass(int angle_degrees) {
    // Calculate which rotation frame to use
    int frame = (angle_degrees * compass_needle_rot_ROTATION_COUNT) / 360;
    frame = frame % compass_needle_rot_ROTATION_COUNT;
    
    // Calculate offset into the spritesheet
    int x_offset = frame * compass_needle_rot_FRAME_WIDTH;
    
    // Draw the rotated needle
    // (Implementation depends on your graphics system)
}
```

### Complete Example: Animated Explosion Effect

```cmake
# In CMakeLists.txt
raw_image_animated(explosion out/effects/explosion.gif)
```

```c
// In your C code
#include "assets/out/effects/explosion.h"

typedef struct {
    int frame;
    uint32_t frame_start_time;
} Animation;

void update_explosion(Animation* anim, uint32_t current_time_ms) {
    uint32_t elapsed = current_time_ms - anim->frame_start_time;
    
    if (elapsed >= explosion_FRAME_DURATIONS[anim->frame]) {
        anim->frame = (anim->frame + 1) % explosion_FRAME_COUNT;
        anim->frame_start_time = current_time_ms;
    }
}

void draw_explosion(Animation* anim) {
    int x_offset = anim->frame * explosion_FRAME_WIDTH;
    // Draw frame at offset
}
```
