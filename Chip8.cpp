#include "Chip8.h"

// Constructor

Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    // Set the PC to starting address of the instructions
    pc = START_ADDRESS;

    // Load Fonts into memory
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
        memory[FONTSET_START_ADDRESS + i] = fontset[i];

    // Initialize Random Number Generator
    randByte =  std::uniform_int_distribution<uint8_t>(0, 255U);    
}

// Function to load ROM contents into the memory for execution

void Chip8::LoadROM(char const* filename) 
{
    // Open the file as a binary file and go to the end of the file
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open())
    {
        std::ios::streampos size = file.tellg(); // The position at the end of the file is it's size

        char* buffer = new char[size]; // Create a buffer to store the contents of rom

        file.seekg(0, std::ios::beg); // Go to the beginning of the file
        file.read(buffer, size); // Read the file from the beginning in the buffer

        file.close();

        //Load the contents of the ROM to memory
        for (long i = 0; i < size; ++i)
            memory[START_ADDRESS + i] = buffer[i];

        delete[] buffer; // Free up buffer space
    }
    
}

// Instructions

void Chip8::OP_NULL(){}

void Chip8::OP_00E0()
{
    // Clear the display
    memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE()
{
    // Decrement the SP and then assign the value to the PC
    pc = stack[--sp];
}

void Chip8::OP_1nnn()
{
    // PC is set to nnn (Least significant 12 bits)
    uint16_t address = opcode & 0x0FFFu;
    pc = address;
}

void Chip8::OP_2nnn()
{
    // The current PC value is stored on stack (Current_Instruction + 2) and SP is incremented
    stack[sp++] = pc;

    // PC is set to nnn (Least significant 12 bits)
    uint16_t address = opcode & 0x0FFFu;
    pc = address;
    
}

void Chip8::OP_3xkk()
{
    uint8_t kk = opcode & 0x00FFu; // Byte
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    if (registers[x] == kk) // if Register Vx == Byte, skip the next instruction
    {
        pc += 2;
    }
}

void Chip8::OP_4xkk()
{
    uint8_t kk = opcode & 0x00FFu; // Byte
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    if (registers[x] != kk) // if Register Vx != Byte, skip the next instruction
    {
        pc += 2;
    }
}

void Chip8::OP_5xy0()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    if (registers[x] == registers[y]) // if Register Vx == Register Vy, skip the next instruction
    {
        pc += 2;
    }
}

void Chip8::OP_6xkk()
{
    uint8_t kk = opcode & 0x00FFu; // Byte
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    // Load Byte into Vx
    registers[x] = kk;
}

void Chip8::OP_7xkk()
{
    uint8_t kk = opcode & 0x00FFu; // Byte
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    //  Add kk into Register Vx
    registers[x] += kk ;
}

void Chip8::OP_8xy0()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    // Load Register Vx with value of Register Vy
    registers[x] = registers[y];
}