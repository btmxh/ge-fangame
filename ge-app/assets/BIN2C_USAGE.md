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

## Rotated Images

Rotate an image by a specified angle (must be a multiple of 45 degrees). The image dimensions will be adjusted to fit the rotated content. Useful for pre-rotated sprites or assets that need to be displayed at specific angles.

### CMake Function

#### `raw_image_rotated(SYMBOL_NAME IMAGE_FILE ROTATION_ANGLE [MODE mode] [ARGS ...])`

**Note:** ROTATION_ANGLE must be a multiple of 45 degrees (0, 45, 90, 135, 180, 225, 270, 315, etc.)

```cmake
# Rotate a compass needle by 90 degrees
raw_image_rotated(compass_needle_90 out/textures/compass-needle.png 90)

# Rotate by 45 degrees with custom mode
raw_image_rotated(diamond_45 out/textures/diamond.png 45 MODE rgb565)

# Rotate by 180 degrees
raw_image_rotated(upside_down out/textures/sprite.png 180)
```

### Direct Script Usage

```bash
# Rotate by 90 degrees
python3 scripts/bin2c_image.py compass.png output.c output.h rotated_sprite argb1555 --rotate 90

# Rotate by 270 degrees
python3 scripts/bin2c_image.py icon.png output.c output.h icon_270 rgb565 --rotate 270
```

### Generated Output

```c
#define rotated_sprite_WIDTH 48         // Width after rotation
#define rotated_sprite_HEIGHT 48        // Height after rotation
#define rotated_sprite_ROTATION_ANGLE 90

extern const uint16_t rotated_sprite[];
extern const uint32_t rotated_sprite_len;
```

**Note:** Width and height may differ from the original image dimensions depending on the rotation angle. For example, a 40x60 image rotated 90° becomes 60x40.

### Usage in Code

```c
// Simply draw the pre-rotated image
draw_sprite(rotated_sprite,
            rotated_sprite_WIDTH,
            rotated_sprite_HEIGHT);
```

### Creating Rotation Spritesheets Manually

If you need multiple rotations in a spritesheet, generate each rotation separately and combine them manually:

```cmake
# Generate rotations at 0, 45, 90, 135, 180, 225, 270, 315 degrees
raw_image(sprite_0 out/textures/sprite.png)
raw_image_rotated(sprite_45 out/textures/sprite.png 45)
raw_image_rotated(sprite_90 out/textures/sprite.png 90)
raw_image_rotated(sprite_135 out/textures/sprite.png 135)
raw_image_rotated(sprite_180 out/textures/sprite.png 180)
raw_image_rotated(sprite_225 out/textures/sprite.png 225)
raw_image_rotated(sprite_270 out/textures/sprite.png 270)
raw_image_rotated(sprite_315 out/textures/sprite.png 315)
```

Then use them in your code to select the appropriate rotation based on angle.

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

You can process the same image in different ways:

```cmake
# Original static image
raw_image_alpha(coin out/textures/coin.png)

# Rotated versions at different angles
raw_image_rotated(coin_90 out/textures/coin.png 90)
raw_image_rotated(coin_180 out/textures/coin.png 180)

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

### Complete Example: Rotated Compass Needle

If you need the needle at a specific angle (e.g., pointing East):

```cmake
# In CMakeLists.txt - generate a 90-degree rotated needle
raw_image_rotated(compass_needle_90 out/textures/needle.png 90)
```

```c
// In your C code
#include "assets/out/textures/needle.h"

void draw_compass_east() {
    // Draw the pre-rotated needle (pointing east)
    draw_sprite(compass_needle_90,
                compass_needle_90_WIDTH,
                compass_needle_90_HEIGHT);
}
```

For dynamic rotation with multiple angles, generate multiple rotations:

```cmake
# Generate 8 rotations (every 45 degrees)
raw_image(needle_0 out/textures/needle.png)
raw_image_rotated(needle_45 out/textures/needle.png 45)
raw_image_rotated(needle_90 out/textures/needle.png 90)
raw_image_rotated(needle_135 out/textures/needle.png 135)
raw_image_rotated(needle_180 out/textures/needle.png 180)
raw_image_rotated(needle_225 out/textures/needle.png 225)
raw_image_rotated(needle_270 out/textures/needle.png 270)
raw_image_rotated(needle_315 out/textures/needle.png 315)
```

```c
// In your C code
void draw_compass(int angle_degrees) {
    // Round to nearest 45-degree increment
    int rotation_index = ((angle_degrees + 22) / 45) % 8;

    // Array of pointers to the different rotations
    const uint16_t* rotations[] = {
        needle_0, needle_45, needle_90, needle_135,
        needle_180, needle_225, needle_270, needle_315
    };

    // Array of dimensions for each rotation (dimensions may vary)
    int widths[] = {
        needle_0_WIDTH, needle_45_WIDTH, needle_90_WIDTH, needle_135_WIDTH,
        needle_180_WIDTH, needle_225_WIDTH, needle_270_WIDTH, needle_315_WIDTH
    };
    int heights[] = {
        needle_0_HEIGHT, needle_45_HEIGHT, needle_90_HEIGHT, needle_135_HEIGHT,
        needle_180_HEIGHT, needle_225_HEIGHT, needle_270_HEIGHT, needle_315_HEIGHT
    };

    // Draw the appropriate rotation
    draw_sprite(rotations[rotation_index],
                widths[rotation_index],
                heights[rotation_index]);
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
