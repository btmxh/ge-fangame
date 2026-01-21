import sys
from sys import argv
from PIL import Image
import bin2c  # pyright: ignore[reportImplicitRelativeImport]


def emit_run(out, run_len, run_val):
    if run_len <= 15:
        out.append(((run_len - 1) << 4) | run_val)
    else:
        out.append((0xF << 4) | run_val)
        out.append(run_len)


def rle_encode_row(pixels, width, offset):
    out = bytearray()
    row = pixels[offset : offset + width]

    run_val = row[0]
    run_len = 1

    for p in row[1:]:
        if p == run_val and run_len < 255:
            run_len += 1
        else:
            emit_run(out, run_len, run_val)
            run_val = p
            run_len = 1

    emit_run(out, run_len, run_val)
    return out


if __name__ == "__main__":
    if len(sys.argv) != 5:
        print(
            f"Usage: {argv[0]} <input image> <output c file> <output h file> <symbol>"
        )
    image_path, out_c, out_h, symbol = argv[1:5]

    # --- load grayscale image ---
    img = Image.open(image_path).convert("L")
    w, h = img.size
    px = list(img.getdata())  # flat grayscale list

    # --- quantize to palette indices ---
    colors = sorted(set(px))
    if len(colors) > 16:
        raise RuntimeError("Too many colors for packed RLE (max 16)")

    color_idx = {c: i for i, c in enumerate(colors)}
    px = [color_idx[v] for v in px]

    # --- encode rows ---
    row_offsets = []
    data = bytearray()

    for y in range(h):
        row_offsets.append(len(data))
        data.extend(rle_encode_row(px, w, w * y))

    # --- emit C ---
    colors_c = ",".join(str(c) for c in colors)
    offsets_c = ",".join(str(o) for o in row_offsets)

    bin2c.main(
        data,
        out_c,
        out_h,
        symbol,
        header_additional=f"""
        #define CLOUD_TEXTURE_WIDTH {w}
        #define CLOUD_TEXTURE_HEIGHT {h}

        static const unsigned char CLOUD_COLORS[] = {{{colors_c}}};
        static const unsigned short CLOUD_ROW_OFFSETS[] = {{{offsets_c}}};
        """,
    )
