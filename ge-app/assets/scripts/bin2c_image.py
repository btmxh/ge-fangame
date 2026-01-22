#!/usr/bin/env python3

import sys
from PIL import Image
import numpy as np
import bin2c


def rgb888_to_rgb565(r, g, b):
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)


def argb888_to_argb1555(r, g, b, a):
    return ((1 if a else 0) << 15) | ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3)


def main(inp_img: str, out_c: str, out_h: str, sym: str, mode: str):
    img = Image.open(inp_img).convert("RGBA")
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

    data_u16 = np.array(data_u16, dtype=np.uint16)

    # Emit C/H via shared helper
    bin2c.main(
        data_u16,
        out_c,
        out_h,
        sym,
        header_additional=f"""
#define {sym}_WIDTH {w}
#define {sym}_HEIGHT {h}
        """,
        dtype="uint16_t",
    )


if __name__ == "__main__":
    if len(sys.argv) < 5:
        print(
            "usage: bin2c_image.py <input.png> <output.c> <output.h> <symbol> [mode] [additional_args...]"
        )
        sys.exit(1)

    inp_img, out_c, out_h, sym = sys.argv[1:5]
    mode = sys.argv[5] if len(sys.argv) >= 6 else "rgb565"
    # Additional args beyond mode are ignored for now but accepted for consistency

    main(inp_img, out_c, out_h, sym, mode)
