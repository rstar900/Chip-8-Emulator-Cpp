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

void Chip8::OP_8xy1()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    // OR Register Vx with value of Register Vy
    registers[x] |= registers[y];
}

void Chip8::OP_8xy2()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    // AND Register Vx with value of Register Vy
    registers[x] &= registers[y];
}

void Chip8::OP_8xy3()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    // XOR Register Vx with value of Register Vy
    registers[x] ^= registers[y];
}

void Chip8::OP_8xy4()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    uint16_t sum = registers[x] + registers[y]; // Vx + Vy

    if (sum > 255U)
        registers[0xF] = 1; // if overflow, then Vf = 1 (Carry)
    else
        registers[0xF] = 0;        

    // Load Register Vx with Least significant 8 bits of sum
    registers[x] = sum & 0xFFu;
}

void Chip8::OP_8xy5()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    if (registers[x] > registers[y])
        registers[0xF] = 1; // if Vx > Vy, set Vf to 1 (not borrow)
    else
        registers[0xF] = 0;        

    // Load Register Vx with Vx - Vy
    registers[x] -= registers[y];
}

void Chip8::OP_8xy6()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    registers[0xF] = (registers[x] & 0x1u); // Store Least Significant bit of Vx in Vf

    // Right shift value of Register Vx by 1
    registers[x] >>= 1;
}

void Chip8::OP_8xy7()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    if (registers[y] > registers[x])
        registers[0xF] = 1; // if Vy > Vx, set Vf to 1 (not borrow)
    else
        registers[0xF] = 0;        

    // Load Register Vx with Vy - Vx
    registers[x] = registers[y] - registers[x];
}

void Chip8::OP_8xyE()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    registers[0xF] = (registers[x] & 0x80u); // Store Most Significant bit of Vx in Vf

    // Left shift value of Register Vx by 1
    registers[x] <<= 1;
}

void Chip8::OP_9xy0()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy    
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    if (registers[x] != registers[y]) // if Register Vx != Vy, skip the next instruction
    {
        pc += 2;
    }
}
