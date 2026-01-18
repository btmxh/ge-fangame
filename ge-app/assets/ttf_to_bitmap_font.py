import argparse
from PIL import Image, ImageDraw, ImageFont
import bin2c  # pyright: ignore[reportImplicitRelativeImport]


GLYPH_W = 8
GLYPH_H = 12


def parse_args():
    p = argparse.ArgumentParser(
        description="Generate bitmap font (8x12, bit-packed rows, C source)"
    )

    p.add_argument("--ttf", default="ProggyClean.ttf")
    p.add_argument("--range", type=int, nargs=2, default=(32, 126))
    p.add_argument("--symbol", required=True)
    p.add_argument("--out", required=True)

    return p.parse_args()


def main():
    args = parse_args()
    first, last = args.range

    font = ImageFont.truetype(args.ttf, 16)

    buf = bytearray()

    for code in range(first, last + 1):
        ch = chr(code)

        img = Image.new("1", (GLYPH_W, GLYPH_H), 0)
        draw = ImageDraw.Draw(img)

        draw.text((0, 0), ch, fill=1, font=font)

        pixels = list(img.getdata())  # pyright: ignore[reportArgumentType]

        # Pack 8 pixels → 1 byte per row
        for y in range(GLYPH_H):
            row = 0
            for x in range(GLYPH_W):
                if pixels[y * GLYPH_W + x]:
                    row |= 1 << (7 - x)
            buf.append(row)

    bin2c.main(
        buf,
        args.out + ".c",
        args.out + ".h",
        args.symbol,
    )

    print(f"Generated {args.out}.c / {args.out}.h")


if __name__ == "__main__":
    main()
