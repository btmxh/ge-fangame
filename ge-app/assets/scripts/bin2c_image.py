#!/usr/bin/env python3

import sys
from PIL import Image
import numpy as np
import bin2c
import argparse


# Rotation constraint: Only multiples of this angle are allowed
ROTATION_ANGLE_INCREMENT = 45


def rgb888_to_rgb565(r, g, b):
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)


def argb888_to_argb1555(r, g, b, a):
    return ((1 if a else 0) << 15) | ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3)


def convert_image_to_data(img: Image.Image, mode: str):
    """Convert a PIL Image to pixel data in the specified format."""
    img = img.convert("RGBA")
    w, h = img.size
    px = img.load()

    data_u16 = []

    for y in range(h):
        for x in range(w):
            r, g, b, a = px[x, y]

            if mode == "rgb565":
                data_u16.append(rgb888_to_rgb565(r, g, b))
            elif mode == "argb1555":
                data_u16.append(argb888_to_argb1555(r, g, b, a > 0))
            else:
                raise ValueError(f"unknown mode: {mode}")

    return np.array(data_u16, dtype=np.uint16), w, h


def rotate_image(img: Image.Image, angle: int, mode: str):
    """Rotate an image by the specified angle (must be multiple of ROTATION_ANGLE_INCREMENT).
    
    Args:
        img: Input PIL Image
        angle: Rotation angle in degrees (must be multiple of ROTATION_ANGLE_INCREMENT)
        mode: Color mode (rgb565 or argb1555)
    
    Returns:
        Tuple of (data, w, h) for the rotated image
    
    Raises:
        ValueError: If angle is not a multiple of ROTATION_ANGLE_INCREMENT
    """
    if angle % ROTATION_ANGLE_INCREMENT != 0:
        raise ValueError(
            f"Rotation angle must be a multiple of {ROTATION_ANGLE_INCREMENT} degrees, got {angle}"
        )
    
    # Use BICUBIC for better quality rotation
    # expand=True allows the image dimensions to change to fit the rotated content
    rotated = img.rotate(-angle, resample=Image.BICUBIC, expand=True)
    
    data, w, h = convert_image_to_data(rotated, mode)
    
    return data, w, h


def process_animated_image(img_path: str, mode: str):
    """Process animated images (APNG, WEBP, GIF) and extract frames.
    
    Returns:
        Tuple of (data, w, h, frame_w, frame_h, frame_count, frame_durations)
        For single-frame images: frame_w, frame_h, frame_count, frame_durations are None
    """
    img = Image.open(img_path)
    
    frames = []
    frame_durations = []
    
    try:
        # Try to get frame count
        frame_count = getattr(img, 'n_frames', 1)
        
        for i in range(frame_count):
            img.seek(i)
            frame = img.convert("RGBA").copy()
            frames.append(frame)
            
            # Get frame duration in milliseconds (if available)
            duration = img.info.get('duration', 100)  # Default to 100ms
            frame_durations.append(duration)
    except (EOFError, AttributeError):
        # Single frame or error reading frames
        frames = [img.convert("RGBA")]
        frame_durations = [100]
    
    if len(frames) == 1:
        # Not actually animated, process as single image
        data, w, h = convert_image_to_data(frames[0], mode)
        return data, w, h, None, None, None, None
    
    # Create horizontal spritesheet from frames
    frame_w, frame_h = frames[0].size
    sheet_w = frame_w * len(frames)
    sheet_h = frame_h
    
    spritesheet = Image.new("RGBA", (sheet_w, sheet_h), (0, 0, 0, 0))
    
    for i, frame in enumerate(frames):
        spritesheet.paste(frame, (i * frame_w, 0))
    
    data, w, h = convert_image_to_data(spritesheet, mode)
    
    return data, w, h, frame_w, frame_h, len(frames), frame_durations


def main(inp_img: str, out_c: str, out_h: str, sym: str, mode: str, 
         rotation_angle: int = 0, animated: bool = False):
    """
    Main processing function.
    
    Args:
        inp_img: Input image path
        out_c: Output C file path
        out_h: Output H file path
        sym: Symbol name
        mode: Color mode (rgb565 or argb1555)
        rotation_angle: Rotation angle in degrees (must be multiple of 45, 0 = no rotation)
        animated: Whether to process as animated image
    """
    header_additional = ""
    
    if rotation_angle != 0:
        # Rotate image by specified angle
        img = Image.open(inp_img).convert("RGBA")
        data, w, h = rotate_image(img, rotation_angle, mode)
        
        header_additional = f"""
#define {sym}_WIDTH {w}
#define {sym}_HEIGHT {h}
#define {sym}_ROTATION_ANGLE {rotation_angle}
        """
    elif animated:
        # Process animated image
        data, w, h, frame_w, frame_h, frame_count, frame_durations = process_animated_image(inp_img, mode)
        
        if frame_count is None:
            # Single frame, treat as static image
            header_additional = f"""
#define {sym}_WIDTH {w}
#define {sym}_HEIGHT {h}
            """
        else:
            # Multi-frame animation
            duration_values_csv = ",".join(str(d) for d in frame_durations)
            header_additional = f"""
#define {sym}_WIDTH {w}
#define {sym}_HEIGHT {h}
#define {sym}_FRAME_WIDTH {frame_w}
#define {sym}_FRAME_HEIGHT {frame_h}
#define {sym}_FRAME_COUNT {frame_count}

static const unsigned short {sym}_FRAME_DURATIONS[] = {{{duration_values_csv}}};
            """
    else:
        # Simple static image
        img = Image.open(inp_img)
        data, w, h = convert_image_to_data(img, mode)
        
        header_additional = f"""
#define {sym}_WIDTH {w}
#define {sym}_HEIGHT {h}
        """

    # Emit C/H via shared helper
    bin2c.main(
        data,
        out_c,
        out_h,
        sym,
        header_additional=header_additional,
        dtype="uint16_t",
    )


if __name__ == "__main__":
    # Support both old-style positional args and new optional features
    parser = argparse.ArgumentParser(
        description='Convert images to C/H files with optional rotation and animation support',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Basic usage (backwards compatible)
  bin2c_image.py input.png output.c output.h symbol rgb565
  
  # With rotation (rotate by 90 degrees)
  bin2c_image.py input.png output.c output.h symbol rgb565 --rotate 90
  
  # Animated image (APNG, WEBP, GIF)
  bin2c_image.py animation.png output.c output.h symbol argb1555 --animated
        """
    )
    
    parser.add_argument('input', help='Input image file')
    parser.add_argument('output_c', help='Output C file')
    parser.add_argument('output_h', help='Output H file')
    parser.add_argument('symbol', help='Symbol name')
    parser.add_argument('mode', nargs='?', default='rgb565', 
                        choices=['rgb565', 'argb1555'],
                        help='Color mode (default: rgb565)')
    parser.add_argument('--rotate', type=int, metavar='ANGLE',
                        help='Rotation angle in degrees (must be multiple of 45)')
    parser.add_argument('--animated', action='store_true',
                        help='Process as animated image (APNG/WEBP/GIF)')
    parser.add_argument('extra_args', nargs='*', 
                        help='Additional arguments (for compatibility)')
    
    # Check if we're being called with old-style arguments (for backwards compatibility)
    # Old-style: no arguments start with '--'
    # New-style: at least one argument starts with '--'
    has_new_style_args = any(arg.startswith('--') for arg in sys.argv[1:])
    
    if len(sys.argv) >= 5 and not has_new_style_args:
        # Old-style: bin2c_image.py input.png output.c output.h symbol [mode] [...]
        inp_img = sys.argv[1]
        out_c = sys.argv[2]
        out_h = sys.argv[3]
        sym = sys.argv[4]
        mode = sys.argv[5] if len(sys.argv) >= 6 else "rgb565"
        rotation_angle = 0
        animated = False
        
        # Check for special flags in additional args (with bounds checking)
        remaining_args = sys.argv[6:] if len(sys.argv) > 6 else []
        i = 0
        while i < len(remaining_args):
            arg = remaining_args[i]
            if arg == '--rotate' and i + 1 < len(remaining_args):
                try:
                    rotation_angle = int(remaining_args[i + 1])
                    i += 2  # Skip the next argument too
                except ValueError:
                    i += 1
            elif arg == '--animated':
                animated = True
                i += 1
            else:
                i += 1
    else:
        # New-style: use argparse
        args = parser.parse_args()
        inp_img = args.input
        out_c = args.output_c
        out_h = args.output_h
        sym = args.symbol
        mode = args.mode
        rotation_angle = args.rotate or 0
        animated = args.animated
    
    main(inp_img, out_c, out_h, sym, mode, rotation_angle, animated)
