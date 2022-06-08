# Chip-8-Emulator-Cpp
A Chip 8 emulator in C++ using SDL2 framework for graphics (referring this site: https://austinmorlan.com/posts/chip8_emulator/)
<br>
## Steps (Tested on MacOS, might differ a little bit on other platforms)
### 1. Open terminal and type (Brew needs to be preinstalled)
brew install sdl2
<br>
### 2. To compile at the location of the source file, go to the directory of the source code and type (clang++ and g++ both work)
/usr/bin/g++ -std=c++11 ./main.cpp ./Chip8.cpp ./Platform.cpp -o /Users/rachitgarg/Documents/Chip_8_Emulator/chip8 -lSDL2

## I have provided a pre-compiled binary for MacOS (x86-64)

### Usage:
./chip8 &lt;scale&gt; &lt;delay&gt; &lt;path_to_rom_file&gt;

## Video 
https://www.youtube.com/watch?v=7aISBVfSjWg
