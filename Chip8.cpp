#include "Chip8.hpp"

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

    // Set up function pointer tables

    // table assignments
    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1nnn;
    table[0x2] = &Chip8::OP_2nnn;
    table[0x3] = &Chip8::OP_3xkk;
    table[0x4] = &Chip8::OP_4xkk;
    table[0x5] = &Chip8::OP_5xy0;
    table[0x6] = &Chip8::OP_6xkk;
    table[0x7] = &Chip8::OP_7xkk;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::OP_9xy0;
    table[0xA] = &Chip8::OP_Annn;
    table[0xB] = &Chip8::OP_Bnnn;
    table[0xC] = &Chip8::OP_Cxkk;
    table[0xD] = &Chip8::OP_Dxyn;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    // table0 assignments
    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    // table8 assignments
    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    // tableE assignments
    tableE[0x1] = &Chip8::OP_ExA1;
    tableE[0xE] = &Chip8::OP_Ex9E;

    // tableF assignments
    tableF[0x07] = &Chip8::OP_Fx07;    
    tableF[0x0A] = &Chip8::OP_Fx0A;    
    tableF[0x15] = &Chip8::OP_Fx15;    
    tableF[0x18] = &Chip8::OP_Fx18;    
    tableF[0x1E] = &Chip8::OP_Fx1E;    
    tableF[0x29] = &Chip8::OP_Fx29;    
    tableF[0x33] = &Chip8::OP_Fx33;    
    tableF[0x55] = &Chip8::OP_Fx55;    
    tableF[0x65] = &Chip8::OP_Fx65;    
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

void Chip8::OP_Annn()
{
  // Set Index = Address
  uint16_t address = opcode & 0x0FFFu;
  index = address; 
}

void Chip8::OP_Bnnn()
{
  // Set PC = V0 + address (nnn)       
  uint16_t address = opcode & 0x0FFFu;
  pc = registers[0] + address;  
}

void Chip8::OP_Cxkk()
{
    // Set Vx = byte & random_byte
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Vx
    uint8_t byte = opcode & 0x00FFu;
    registers[x] = byte & randByte(randGen);

}

void Chip8::OP_Dxyn()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    uint8_t height = opcode & 0x000Fu;

    // Wrap around if x and y positions exceed screen width and height
    uint8_t xPos = registers[x] % VIDEO_WIDTH;
    uint8_t yPos = registers[y] % VIDEO_HEIGHT;

    //Set collision = 0 initially in VF
    registers[0xF] = 0;

    //Iterating rows = height and columns = 8 (Fixed for sprites)
    for (unsigned int row = 0; row < height; ++row)
    {
        uint8_t spriteByte = memory[index + row];

        for (unsigned int col = 0; col < 8; ++col) 
        {
            uint8_t spritePixel = spriteByte & (0x80u >> col);

            // Applied a fix by limiting the position inside the memory to avoid segmentation fault 11 
            uint32_t* screenPixel = &video[((yPos + row) * VIDEO_WIDTH + (xPos + col))%(VIDEO_HEIGHT * VIDEO_WIDTH)];

            // If Sprite Pixel is on, then there is possibility of collison
            if (spritePixel)
            {
                // If Screen Pixel is on, then set VF = 1 (collision occured)
                if(*screenPixel == 0xFFFFFFFF) 
                {
                    registers[0xF] = 1;
                }

                // XOR the Screen Pixel with 0xFFFFFFFF (as Pixel can be 0x00000000 or 0xFFFFFFFF in our case)
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }  

}

void Chip8::OP_Ex9E()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    uint8_t key = registers[x];

    if(keypad[key])
        pc += 2; // Skip the next instruction if key in Vx is pressed
}

void Chip8::OP_ExA1()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    uint8_t key = registers[x];

    if(!keypad[key])
        pc += 2; // Skip the next instruction if key in Vx is not pressed    
}

void Chip8::OP_Fx07()
{
    // Set Vx = delay timer value
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    registers[x] = delayTimer;
}

void Chip8::OP_Fx0A()
{
   uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

   // Wait for a key press and then store the value of the key in Vx
   // Easiest way to do this is to keep PC on the same instruction by
   // decrementing it by 2 whenever a key is not pressed

   if (keypad[0])
   {
    registers[x] = 0;
   }
   else if (keypad[1])
   {
       registers[x] = 1;
   } 
      else if (keypad[2])
   {
       registers[x] = 2;
   } 
   else if (keypad[3])
   {
       registers[x] = 3;
   } 
   else if (keypad[4])
   {
       registers[x] = 4;
   } 
   else if (keypad[5])
   {
       registers[x] = 5;
   } 
   else if (keypad[6])
   {
       registers[x] = 6;
   } 
   else if (keypad[7])
   {
       registers[x] = 7;
   } 
   else if (keypad[8])
   {
       registers[x] = 8;
   } 
   else if (keypad[9])
   {
       registers[x] = 9;
   } 
   else if (keypad[10])
   {
       registers[x] = 10;
   } 
   else if (keypad[11])
   {
       registers[x] = 11;
   } 
   else if (keypad[12])
   {
       registers[x] = 12;
   } 
   else if (keypad[13])
   {
       registers[x] = 13;
   } 
   else if (keypad[14])
   {
       registers[x] = 14;
   } 
   else if (keypad[15])
   {
       registers[x] = 15;
   } 
    else
    {
        pc -= 2;
    }
}

void Chip8::OP_Fx15()
{
    // Set Delay Timer = Vx
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    delayTimer = registers[x];
}

void Chip8::OP_Fx18()
{
    // Set Sound Timer = Vx
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    soundTimer = registers[x];
}

void Chip8::OP_Fx1E()
{
    // Index = Index + Vx
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    index += registers[x];
}

void Chip8::OP_Fx29()
{
    // I = Location of sprite for digit Vx
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    uint8_t digit = registers[x];
    index = FONTSET_START_ADDRESS + (5 * digit); // Because each digit occupies 5 Bytes
}

void Chip8::OP_Fx33()
{
    // Store Hundreds, Tens and Ones digits at Index, Index + 1 and Index + 2 
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    uint8_t value = registers[x];

    memory[index + 2] = value % 10; // Ones place
    value /= 10;

    memory[index + 1] = value % 10; // Tens place
    value /= 10;

    memory[index] = value % 10; // Hundreds place
}

void Chip8::OP_Fx55()
{
    // Store registers V0 through Vx in memory starting at location I (Index)
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    for (uint8_t i = 0; i <= x; ++i)
        memory[index + i] = registers[i];
}

void Chip8::OP_Fx65()
{
    // Read registers V0 through Vx from memory starting at location I (Index)
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    for (uint8_t i = 0; i <= x; ++i)
        registers[i] = memory[index + i];
} 

void Chip8::Cycle()
{
    // Fetch
    opcode = (memory[pc] << 8u) | memory[pc + 1];

    // Increment PC by 2
    pc += 2;

    // Decode & Execute
    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();

    // Decrement Delay Timer, if set
    if (delayTimer > 0)
        --delayTimer;

    // Decrement Sound Timer, if set
    if (soundTimer > 0)
        --soundTimer;
}