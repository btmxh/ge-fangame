#!/usr/bin/env python3
"""
texture_bin2c_rgb565_a1.py

Convert an input image into:
- texture_color: RGB565 pixels (uint16_t array, 1 entry per pixel)
- texture_alpha: 1-bit alpha mask (uint8_t array, packed MSB-first per byte)
- texture_width, texture_height: int

Outputs:
  <out_base>.h
  <out_base>.c
"""

from __future__ import annotations

import argparse
import os
import re
from math import ceil
from PIL import Image


def sanitize_symbol(sym: str) -> str:
    sym = sym.strip()
    sym = re.sub(r"[^A-Za-z0-9_]", "_", sym)
    if not sym or not (sym[0].isalpha() or sym[0] == "_"):
        sym = "_" + sym
    return sym


def argb888_to_argb1555(r: int, g: int, b: int, a: bool) -> int:
    return ((1 if a else 0) << 15) | ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3)


def pack_a1(alpha_bits: list[int]) -> bytes:
    out = bytearray(ceil(len(alpha_bits) / 8))
    for i, bit in enumerate(alpha_bits):
        if bit:
            out[i // 8] |= 1 << (7 - (i % 8))
    return bytes(out)


def format_u8_array(name: str, data: bytes, cols: int = 16) -> str:
    if not data:
        return f"const uint8_t {name}[] = {{}};\n"

    lines = [f"const uint8_t {name}[] = {{\n"]
    for i in range(0, len(data), cols):
        chunk = data[i : i + cols]
        lines.append("  " + ", ".join(f"0x{b:02X}" for b in chunk) + ",\n")
    lines.append("};\n")
    return "".join(lines)


def format_u16_array(name: str, data: list[int], cols: int = 8) -> str:
    if not data:
        return f"const uint16_t {name}[] = {{}};\n"

    lines = [f"const uint16_t {name}[] = {{\n"]
    for i in range(0, len(data), cols):
        chunk = data[i : i + cols]
        lines.append("  " + ", ".join(f"0x{v:04X}" for v in chunk) + ",\n")
    lines.append("};\n")
    return "".join(lines)


def make_include_guard(header_filename: str) -> str:
    base = os.path.basename(header_filename)
    return re.sub(r"[^A-Za-z0-9]", "_", base).upper() + "_"


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("input")
    ap.add_argument("out_base")
    ap.add_argument("symbol")
    ap.add_argument("--alpha-threshold", type=int, default=128)
    ap.add_argument("--alpha-from", choices=["alpha", "luminance"], default="alpha")
    args = ap.parse_args()

    sym = sanitize_symbol(args.symbol)
    out_h = args.out_base + ".h"
    out_c = args.out_base + ".c"

    img = Image.open(args.input).convert("RGBA")
    w, h = img.size
    px = img.load()

    color_u16: list[int] = []

    for y in range(h):
        for x in range(w):
            r, g, b, a = px[x, y]
            color_u16.append(argb888_to_argb1555(r, g, b, a >= args.alpha_threshold))

    os.makedirs(os.path.dirname(out_h) or ".", exist_ok=True)
    guard = make_include_guard(out_h)

    # Header
    h_txt = f"""#ifndef {guard}
#define {guard}

#include <stdint.h>

#ifdef __cplusplus
extern "C" {{
#endif

extern const uint16_t {sym}_color[];

extern const uint32_t {sym}_width;
extern const uint32_t {sym}_height;

#ifdef __cplusplus
}}
#endif

#endif
"""

    # C file
    c_txt = f"""#include "{os.path.basename(out_h)}"
#include <stdint.h>

{format_u16_array(f"{sym}_color", color_u16)}

const uint32_t {sym}_width  = {w};
const uint32_t {sym}_height = {h};
"""

    with open(out_h, "w", encoding="utf-8") as f:
        f.write(h_txt)
    with open(out_c, "w", encoding="utf-8") as f:
        f.write(c_txt)

    print(f"Wrote {out_h}, {out_c}")
    print(f"Pixels: {w}x{h}")
    print(f"ARGB1555 entries: {len(color_u16)}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
