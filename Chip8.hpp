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

// FONTSET Sprites in memory (Each 5 bytes) (static : For some reason I was getting duplicate symbol in main.o)
static uint8_t fontset[FONTSET_SIZE] = 
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

// Video Width and Height
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;

class Chip8 
{
public:

    // Internal components (general note: {} after definition initializes the members with zeroes)
    uint8_t registers[16]{};
    uint8_t memory[4096]{};
    uint16_t stack[16]{};
    uint16_t index{};
    uint16_t pc{};
    uint8_t sp{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    uint8_t keypad[16]{};
    uint32_t video[VIDEO_WIDTH * VIDEO_HEIGHT]{};
    uint16_t opcode; 

    // Members required for Random Number generation
    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;

    // General methods
    Chip8();
    void LoadROM(char const* filename);

    // Opcodes
    void OP_NULL(); // NOP instruction
    void OP_00E0(); // CLS
    void OP_00EE(); // RET
    void OP_1nnn(); // JP addr
    void OP_2nnn(); // CALL addr
    void OP_3xkk(); // SE Vx, byte
    void OP_4xkk(); // SNE Vx, byte
    void OP_5xy0(); // SE Vx, Vy
    void OP_6xkk(); // LD Vx, byte
    void OP_7xkk(); // ADD Vx, byte
    void OP_8xy0(); // LD Vx, Vy
    void OP_8xy1(); // OR Vx, Vy
    void OP_8xy2(); // AND Vx, Vy
    void OP_8xy3(); // XOR Vx, Vy
    void OP_8xy4(); // ADD Vx, Vy
    void OP_8xy5(); // SUB Vx, Vy
    void OP_8xy6(); // SHR Vx
    void OP_8xy7(); // SUBN Vx, Vy
    void OP_8xyE(); // SHL Vx, {, Vy}
    void OP_9xy0(); // SNE Vx, Vy
    void OP_Annn(); // LD I, addr
    void OP_Bnnn(); // JP V0, addr
    void OP_Cxkk(); // RND Vx, byte
    void OP_Dxyn(); // DRW Vx, Vy, nibble
    void OP_Ex9E(); // SKP Vx   
    void OP_ExA1(); // SKNP Vx
    void OP_Fx07(); // LD Vx, DT
    void OP_Fx0A(); // LD Vx, K
    void OP_Fx15(); // LD DT, Vx
    void OP_Fx18(); // LD ST, Vx
    void OP_Fx1E(); // ADD I, Vx
    void OP_Fx29(); // LD F, Vx
    void OP_Fx33(); // LD B, Vx
    void OP_Fx55(); // LD [I], Vx
    void OP_Fx65(); // LD Vx, [I]

    // Function pointer typedef
    typedef void (Chip8::*Chip8Func)(); // This is the syntax for defining pointers to Member functions of a class

    // Function tables
    Chip8Func table[0xF + 1]{&Chip8::OP_NULL}; // Master Table (Contains pointers to other table functions)
    Chip8Func table0[0xE + 1]{&Chip8::OP_NULL}; // Opcodes starting with 0x0
    Chip8Func table8[0xE + 1]{&Chip8::OP_NULL}; // Opcodes starting with 0x8 
    Chip8Func tableE[0xE + 1]{&Chip8::OP_NULL}; // Opcodes starting with 0xE
    Chip8Func tableF[0x65 + 1]{&Chip8::OP_NULL}; // Opcodes starting with 0xF

    // Table helper functions
    void Table0()
    {
        ((*this).*(table0[opcode & 0x000Fu]))();
    }

    void Table8()
    {
        ((*this).*(table8[opcode & 0x000Fu]))();
    }

    void TableE()
    {
        ((*this).*(tableE[opcode & 0x000Fu]))();
    }

    void TableF()
    {
        ((*this).*(tableF[opcode & 0x00FFu]))();
    }

    // Cycle function
    void Cycle();

}; 

#endif