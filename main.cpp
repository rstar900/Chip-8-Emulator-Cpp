#include "Chip8.hpp"
#include "Platform.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    // Check for correct command to run the executable with sufficient arguments
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    char const* romFilename = argv[3];

    // Instantiate SDL2 based graphical platform
    Platform platform("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale,
                      VIDEO_WIDTH, VIDEO_HEIGHT);

    // Instantiate Chip-8 Emulation Engine 
    Chip8 chip8;

    // Load the ROM
    chip8.LoadROM(romFilename);

    // Specify the bytes occupied by a single row of display (size of one pixel multiplied by Width)
    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

    auto lastCycleTime = std::chrono::high_resolution_clock::now(); // Starting time
    bool quit = false; // variable to check if the exit condition is true

    // Run the emulation cycles in loop until exit condition becomes true
    while(!quit)
    {
        // Register key input
        quit = platform.ProcessInput(chip8.keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();

        // Variable to store elapsed time between last cycle and this one
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt > cycleDelay)
        {
            // Update the last cycle time to current time
            lastCycleTime = currentTime;

            // Execute the emulation cycle
            chip8.Cycle();

            // Update the display
            platform.Update(chip8.video, videoPitch);
        }
    }

    return 0;
}