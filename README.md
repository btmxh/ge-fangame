# ge-fangame

This is a fangame project for CTB Girls' Dorm's _Glow Embrace_ parody, released
in the EP _And so, I’ll keep wishing for that day to come._

## Build & Run

This project follows standard CMake build procedures. A `flake.nix` file is
provided, but the use of Nix in this project is not required. One should still
use it, however.

### PC build

This project is meant to be run on a STM32 microcontroller, but building for PC
is possible for testing purposes.

First, ensure Python (for automatic asset bundling) and SDL3 is installed on
your system. Then, configure and build without any extra options.

```bash
cmake -S. -Bbuild/pc && cmake --build build/pc
```

The output executable is self-contained in a binary directory depending on your
CMake generator.

### STM32 build

> [!NOTE]
> This project only supports the Discovery kit with STM32F429ZI MCU.
> Clocks, pins and peripherals are configured based on this kit alone.
> Once input and audio is supported, wiring instructions will be properly
> documented in this README file.

To build for STM32, only the `arm-none-eabi-gcc` toolchain is supported. LLVM
might work but some of the STM32-specific code heavily depends on GNU compiler
extensions, so maybe Clang will complain about that.

Python is still required for automatic asset bundling here. SDL3 is no longer
required.

To compile, configure your toolchain and run

```bash
cmake -S. -Bbuild/stm -DGE_HAL_STM32=ON && cmake --build build/stm

# or if your toolchain was not yet configured, use the template toolchain file
# in cmake/arm-none-eabi.cmake
cmake -S. -Bbuild/stm -DGE_HAL_STM32=ON\
        -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake &&
cmake --build build/stm
```

Handy scripts are provided in `scripts/` to flash and run. This requires
`openocd` and `tio` (to read from stdout/USART1). One should treat these scripts
as documentation, rather than proper tooling, as these do not cover all use
cases.

To flash, run:
```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program PATH_TO_THE_ELF verify reset exit"
```

To debug, run:
```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg

# in another terminal
gdb PATH_TO_THE_ELF
    # in gdb prompt
    (gdb) target extended-remote :3333
    (gdb) monitor reset halt # software reset
    (gdb) load # reflash the ELF
```

Make sure to set up proper udev rules if you are on Linux.

## Copyright

CTB Girls' Dorm's works are all for private use (not shared to anyone except the
author), including the EP _And so, I’ll keep wishing for that day to come._ Such
works, if distributed publicly, are blatant copyright infringements.

The EP _And so, I’ll keep wishing for that day to come._ contains three main
tracks (details about why these three songs are chosen are intentionally
omitted):

- [Kitto](https://www.youtube.com/watch?v=dVAf4gRZro0),
  from Tsunomaki Watame's _Hop Step Sheep_ album.
- [Moshimo Unmei No Hito ga Iru No Nara](https://www.youtube.com/watch?v=l-k70gZmFqU),
  from the single with the same title by Nishino Kana.
- [Glow Embrace](https://www.youtube.com/watch?v=BWEmA6AxIyM),
  from [Shirakami Fubuki](https://www.youtube.com/@ShirakamiFubuki)'s _FBKINGDOM_
  album.

These tracks are rearranged for use as background music (BGM) in this fangame.
The use of both _Kitto_ and _Glow Embrace_ are protected under Cover Corp's
[Derivative Works Guidelines](https://hololivepro.com/en/terms/). However,
Nishino Kana's _Moshimo Unmei No Hito ga Iru No Nara_ is not covered by any
derivative works policy, and therefore this GitHub repo will use a different
BGM rearrangement for that part of the game.

Character sprites and backgrounds are hand-drawn by the author, but based on
the three _hololive_ VTubers' design: Shirakami Fubuki, Tsunomaki Watame, and
Nakiri Ayame (once again, details about why these three VTubers are chosen
are intentionally omitted). The use of these characters are also protected
under the same Derivative Works Guidelines.

> [!NOTE]
> While this project is _legally_ a _hololive_ fangame, **DO NOT** expect it to
> be one in any meaningful sense. The game story and characters are majorly
> different from that of the VTubers.

## Credits

Since I can't really draw and rearrange music, most of the assets that I "made"
for this project are heavily based on existing works by other people. This will
be updated as I add more assets to the project.

- `ambient-bgm.fur`: Ambient BGM loop rearranged from _Glow Embrace_ by
  Shirakami Fubuki.
  - Piano notes are arranged based on
    [this piano cover](https://www.youtube.com/watch?v=CIZMn2jcgmQ).
  - Noises are arranged based on
    [this drum cover](https://www.youtube.com/watch?v=B5z_OdGznFk).

## License

This project is in the public domain.
