import sys
import os

if len(sys.argv) != 5:
    print("usage: bin2c.py <input.bin> <output.c> <output.h> <symbol>")
    sys.exit(1)

inp, out_c, out_h, name = sys.argv[1:5]

# Read binary
with open(inp, "rb") as f:
    data = f.read()

# -------- generate .c --------
with open(out_c, "w") as f:
    f.write("#include <stdint.h>\n")
    f.write(f'#include "{os.path.basename(out_h)}"\n\n')

    f.write(f"const uint8_t {name}[] = {{\n")
    for i, b in enumerate(data):
        f.write(f"0x{b:02x},")
        if i % 16 == 15:
            f.write("\n")
    f.write("\n};\n\n")

    f.write(f"const uint32_t {name}_len = {len(data)};\n")

# -------- generate .h --------
guard = f"{name.upper()}_H"

with open(out_h, "w") as f:
    f.write("#pragma once\n\n")
    f.write("#include <stdint.h>\n\n")
    f.write("#ifdef __cplusplus\n")
    f.write('extern "C" {\n')
    f.write("#endif\n\n")

    f.write(f"extern const uint8_t {name}[];\n")
    f.write(f"extern const uint32_t {name}_len;\n\n")

    f.write("#ifdef __cplusplus\n")
    f.write("}\n")
    f.write("#endif\n")
