#ifndef CHIP8_H
#define CHIP8_H
#include <cstdint>
#include <fstream>
#include <chrono>
#include <random>

// Memory regions
const unsigned int START_ADDRESS = 0x200;
const unsigned FONTSET_START_ADDRESS = 0x50;

// Other Constants
const unsigned int FONTSET_SIZE = 80;

// FONTSET Sprites in memory (Each 5 bytes)
uint8_t fontset[FONTSET_SIZE] = 
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};



class Chip8 
{
public:
    // Internal components
    uint8_t registers[16]{};
    uint8_t memory[4096]{};
    uint16_t stack[16]{};
    uint16_t index{};
    uint16_t pc{};
    uint8_t sp{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    uint8_t keypad[16]{};
    uint32_t video[64 * 32]{};
    uint16_t opcode; 

    // Members required for Random Number generation
    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;

    // Methods
    Chip8();
    void LoadROM(char const* filename);
}; 

#endif