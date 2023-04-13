![Banner](https://s-christy.com/status-banner-service/minesweeper/banner-slim.svg)

## Overview

This is a Minesweeper clone written in C++ using the SDL2 library. It includes
an auto-solving algorithm that not only provides hints for the statistically
best move, but can also automatically solve the game.

## Screenshots

<p align="center">
  <img src="./screenshots/image_0.png" />
</p>

<p align="center">
  <img src="./screenshots/image_1.png" />
</p>

<p align="center">
  <img src="./screenshots/image_2.png" />
</p>

## Features

- Classic Minesweeper gameplay
- Customizable board size and mine count
- Auto-solving algorithm providing hints and automatic solutions
- Statistically based move suggestions for when a guess is required
- Built with C++ and the SDL2 library for cross-platform support
- Command line argument parsing
- Built using CMake
- Image support for mines and flags.
- Font rendering support.
- Error handling.

## Usage

```
Usage:
  -h,--help              Display Usage statement.
  -n,--num-mines         Number of mines
  -x,--width             Board width
  -y,--height            Board height
```

## Controls

- Left-click on a cell to reveal its contents.
- Right-click on a cell to mark it as a mine.
- Press 'h' to get hints for the next best move.
- Press 'c' to let the auto-solver complete the game for you.
- Press 'f' to let the auto-solver mark places where there are definately mines.
- Press 'r' to restart the game with the same settings.
- Press 'ESC' to quit the game.

## Dependencies

```
cmake
g++
libsdl2-dev
libsdl2-gfx-dev
libsdl2-image-dev
libsdl2-ttf-dev
```

## License

This work is licensed under the GNU General Public License version 3 (GPLv3).

[<img src="https://s-christy.com/status-banner-service/GPLv3_Logo.svg" width="150" />](https://www.gnu.org/licenses/gpl-3.0.en.html)
