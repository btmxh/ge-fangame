import sys
import os


def main(data, out_c, out_h, name, header_additional="", dtype="uint8_t"):
    # -------- generate .c --------
    with open(out_c, "w") as f:
        f.write("#include <stdint.h>\n")
        f.write(f'#include "{os.path.basename(out_h)}"\n\n')

        f.write(f"const {dtype} {name}[] = {{\n")
        for i, b in enumerate(data):
            f.write(f"0x{b:02x},")
            if i % 16 == 15:
                f.write("\n")
        f.write("\n};\n\n")

        f.write(f"const uint32_t {name}_len = {len(data)};\n")

        # Create parent dir
        os.makedirs(os.path.dirname(out_c), exist_ok=True)
        os.makedirs(os.path.dirname(out_h), exist_ok=True)

        with open(out_h, "w") as f:
            f.write("#pragma once\n\n")
            f.write("#include <stdint.h>\n\n")
            f.write("#ifdef __cplusplus\n")
            f.write('extern "C" {\n')
            f.write("#endif\n\n")

            f.write(f"extern const {dtype} {name}[];\n")
            f.write(f"extern const uint32_t {name}_len;\n\n")
            if header_additional:
                f.write(header_additional)
                f.write("\n")

            f.write("#ifdef __cplusplus\n")
            f.write("}\n")
            f.write("#endif\n")


if __name__ == "__main__":
    if len(sys.argv) < 5:
        print("usage: bin2c.py <input.bin> <output.c> <output.h> <symbol> [additional_args...]")
        sys.exit(1)

    inp, out_c, out_h, name = sys.argv[1:5]
    # Additional args are ignored for now but accepted for consistency

    with open(inp, "rb") as f:
        data = f.read()
        main(data, out_c, out_h, name)
