# ge-fangame

This is a fangame project for CTB Girls' Dorm's _Glow Embrace_ parody, released
in the EP _And so, I’ll keep wishing for that day to come._

## Build & Run

This project follows standard CMake build procedures.

### PC build

This project is meant to be run on a STM32 microcontroller, but building for PC
is possible for testing purposes.

First, ensure Python (for automatic asset bundling) and SDL3 is installed on
your system. Then, configure and build without any extra options.

```bash
cmake -S. -Bbuild && cmake --build build
```

The output executable is self-contained in a binary directory depending on your
CMake generator.

### STM32 build

TODO: this is currently unsupported for the time being.

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
