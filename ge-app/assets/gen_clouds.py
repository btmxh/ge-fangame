import argparse
from sys import argv
from PIL import Image
import tempfile
import bin2c  # pyright: ignore[reportImplicitRelativeImport]


def rle_encode_row(pixels, width, offset):
    """
    pixels: flat list/bytes of palette indices, row-major
    returns: bytes containing row-wise RLE
    """
    out = bytearray()
    idx = offset
    row = pixels[idx : idx + width]

    run_val = row[0]
    run_len = 1

    for p in row[1:]:
        if p == run_val and run_len < 255:
            run_len += 1
        else:
            out.append(run_len)
            out.append(run_val)
            run_val = p
            run_len = 1

    # flush last run
    out.append(run_len)
    out.append(run_val)

    return out


if __name__ == "__main__":
    image_path, out_stem = argv[1:3]
    out_c = out_stem + ".c"
    out_h = out_stem + ".h"
    symbol_name = "clouds"

    img = Image.open(image_path).convert("L")
    w, h = img.size
    px = list(img.getdata())  # pyright: ignore[reportArgumentType]

    # quantize to indexed
    colors = sorted(set(px))
    color_idx = {c: i for i, c in enumerate(colors)}
    px = [color_idx[v] for v in px]

    # rle encode each row
    rows = []
    for y in range(h):
        rle_encoded = rle_encode_row(px, w, w * y)
        rows.append(rle_encoded)

    max_len = max(len(r) for r in rows)
    data = bytearray()
    for row in rows:
        # pad to max_len
        padded = row + bytes(max_len - len(row))
        data.extend(padded)

    colors = ",".join(str(c) for c in colors)
    bin2c.main(
        data,
        out_c,
        out_h,
        symbol_name,
        header_additional=f"""
        static const int CLOUD_COLORS[]={{{colors}}};
        #define CLOUD_TEXTURE_WIDTH {w}
        """,
    )
