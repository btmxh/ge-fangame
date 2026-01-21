import sys
from PIL import Image, ImageDraw, ImageFont
import bin2c  # pyright: ignore[reportImplicitRelativeImport]
import math


def main(font_file: str, font_size: int, out_c: str, out_h: str, symbol: str):
    # print getcwd
    print("Current working directory:", sys.path[0])
    print(font_file)
    font = ImageFont.truetype(font_file, font_size)

    # --- STEP 1: DYNAMIC CALCULATION ---

    # 1. Calculate Vertical Metrics (Height)
    ascent, descent = font.getmetrics()

    # Ensure height is even just in case, or add 1px padding if you want 'breathing room'
    # logical_height is usually enough.
    logical_height = ascent + descent
    CELL_H = logical_height

    # 2. Calculate Horizontal Metrics (Max Width)
    max_w = 0
    first, last = 32, 126
    chars_to_gen = range(first, last + 1)  # ASCII 32 to 126

    for code in chars_to_gen:
        ch = chr(code)
        bbox = font.getbbox(ch)  # (left, top, right, bottom)
        if bbox:
            w = bbox[2] - bbox[0]
            if w > max_w:
                max_w = w

    # 3. Round Width up to nearest 8 pixels (for byte alignment)
    # If max_w is 10, CELL_W becomes 16. If max_w is 6, CELL_W becomes 8.
    CELL_W = math.ceil(max_w / 8) * 8

    # Calculate bytes per row (stride)
    BYTES_PER_ROW = CELL_W // 8

    print(f"Detected Metrics for {font_size}px:")
    print(f"  Max Char Width: {max_w}px -> Rounded to CELL_W: {CELL_W}px")
    print(f"  Line Height:    {CELL_H}px")
    print(f"  Struct Size:    {CELL_H * BYTES_PER_ROW} bytes per char")

    # --- STEP 2: GENERATION ---

    buf = bytearray()
    advances = []

    # We align text based on the baseline.
    # Usually, drawing at y = ascent - bbox_top is complex.
    # The simplest reliable way with Pillow is to draw at (0, 0) and crop,
    # or trust the font metrics if drawing into the full cell.
    # We will draw so the 'baseline' sits at `ascent`.

    for code in chars_to_gen:
        ch = chr(code)
        img = Image.new("1", (CELL_W, CELL_H), 0)
        draw = ImageDraw.Draw(img)

        # Draw text.
        # Note: In most fonts, drawing at (0,0) puts the baseline at `ascent`.
        # However, some fonts render slightly off.
        # Using specific anchor="ls" (Left, Baseline) forces the anchor to be the baseline.
        # We place the baseline at x=0, y=ascent.
        draw.text((0, ascent), ch, fill=1, font=font, anchor="ls")

        pixels = list(img.getdata())

        # Calculate real advance width for the proportional spacing
        # If the font has it, use getlength, otherwise fall back to bbox
        real_advance = int(font.getlength(ch))

        # Pack bits
        for y in range(CELL_H):
            for b in range(BYTES_PER_ROW):
                byte_val = 0
                for bit in range(8):
                    # Local x within the byte
                    x_global = b * 8 + bit

                    if x_global < CELL_W:
                        if pixels[y * CELL_W + x_global]:
                            byte_val |= 1 << (7 - bit)
                buf.append(byte_val)
        advances.append(real_advance)

    bin2c.main(
        buf,
        out_c,
        out_h,
        symbol,
        f"""
#define {symbol}_FIRST_CHAR {first}
#define {symbol}_LAST_CHAR {last}
#define {symbol}_CELL_WIDTH {CELL_W}
#define {symbol}_CELL_HEIGHT {CELL_H}
#define {symbol}_BYTES_PER_ROW {BYTES_PER_ROW}

static const unsigned char {symbol}_ADVANCES[] = {{{", ".join(str(a) for a in advances)}}};
    """,
    )


if __name__ == "__main__":
    if len(sys.argv) != 6:
        print(
            "usage: bin2c_bitmap_font.py <font.ttf> <output.c> <output.h> <symbol> <font_size>"
        )
        sys.exit(1)

    font_path, out_c, out_h, symbol, font_size_str = sys.argv[1:6]
    font_size = int(font_size_str)
    main(font_path, font_size, out_c, out_h, symbol)
