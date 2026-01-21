import sys
import soundfile as sf
import bin2c
import numpy as np


def main(inp_wav: str, out_c: str, out_h: str, sym: str):
    data, sample_rate = sf.read(inp_wav, dtype="int16")
    assert sample_rate == 8000, "Only 8kHz WAV files are supported"
    # convert int16 to int8
    data_u8 = ((data.astype(np.int32) + 32768) >> 8).astype(np.uint8)
    bin2c.main(data_u8, out_c, out_h, sym)


if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("usage: bin2c_wav.py <input.wav> <output.c> <output.h> <symbol>")
        sys.exit(1)

    inp_wav, out_c, out_h, sym = sys.argv[1:5]
    main(inp_wav, out_c, out_h, sym)
