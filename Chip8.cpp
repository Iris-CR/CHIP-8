#include <cstdint>
#include <cstring>      // For memset
#include <fstream>
#include <chrono>
#include <random>

#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32

class Chip8
{
    public:
        uint8_t registers[16]{};        // Could be named "Vx"
        uint8_t memory[4096]{};
        uint16_t indexRegister{};
        uint16_t pc{};
        uint16_t stack[16]{};
        uint8_t sp{};
        uint8_t delayTimer{};
        uint8_t soundTimer{};
        uint8_t keypad[16]{};
        uint32_t video[VIDEO_WIDTH*VIDEO_HEIGHT]{};
        uint16_t opcode;

        static const unsigned int START_ADDRESS = 0x200;
        static const unsigned int FONTSET_START_ADDRESS = 0x50;

        static const unsigned int FONTSET_SIZE = 80;
        uint8_t fontset[FONTSET_SIZE] =         // Might add static constexpr before the type in future
        {
            0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
            0x20, 0x60, 0x20, 0x20, 0x70,   // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
            0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
            0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0,   // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
            0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
            0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
            0xF0, 0x80, 0xF0, 0x80, 0x80    // F
        };
        std::default_random_engine randGen;
        std::uniform_int_distribution<uint8_t> randByte;

        void LoadROM(char const* filename);

        void OP_00E0();
        void OP_00EE();
        void OP_1nnn();
        void OP_2nnn();
        void OP_3xkk();
        void OP_4xkk();
        void OP_5xy0();
        void OP_6xkk();
        void OP_7xkk();
        void OP_8xy0();
        void OP_8xy1();
        void OP_8xy2();
        void OP_8xy3();
        void OP_8xy4();
        void OP_8xy5();
        void OP_8xy6();
        void OP_8xy7();
        void OP_8xyE();
        void OP_9xy0();
        void OP_Annn();
        void OP_Bnnn();
        void OP_Cxkk();
        void OP_Dxyn();
        void OP_Ex9E();
        void OP_ExA1();
        void OP_Fx07();
        void OP_Fx0A();
        void OP_Fx15();
        void OP_Fx18();
        void OP_Fx1E();
        void OP_Fx29();
        void OP_Fx33();
        void OP_Fx55();
        void OP_Fx65();

    Chip8()
        : randGen(std::chrono::system_clock::now().time_since_epoch().count())  // Constructor Initialization List
    {
        pc = START_ADDRESS;
        for (unsigned int i = 0; i < FONTSET_SIZE; i++)
        {
            memory[FONTSET_START_ADDRESS + i] = fontset[i];
        }
        randByte = std::uniform_int_distribution<uint8_t>(0, 255U); // Generate a random number between 0 and 255 inclusive, the U tells that is unsigned
    }
};

void Chip8::LoadROM(char const* filename)
{
    // Open the file as stream of binary and move the file pointer to the end
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        // Get size of file and allocate a buffer to hold the contents
        std::streampos size = file.tellg();
        char * buffer = new char[size];

        // Go back to the beginning of the file and fill the buffer
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        // Load the ROM contents into the CHIP-8 memory
        for (long i = 0; i < size; i++)
        {
            memory[START_ADDRESS + i] = buffer[i];
        }
        
        delete[] buffer;
    }
    
}

// Operations

void Chip8::OP_00E0()
{
    memset(video, 0, sizeof(video));
}

void Chip8::OP_00EE()
{
    sp--;
    pc = stack[sp];
}

void Chip8::OP_1nnn()
{
    uint16_t address = opcode & 0x0FFFu;
    pc = address;
}

void Chip8::OP_2nnn()
{
    uint16_t address = opcode & 0x0FFFu;
    stack[sp] = pc;
    sp++;
    pc = address;
}

void Chip8::OP_3xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] == byte)
        pc += 2;
}

void Chip8::OP_4xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] != byte)
        pc += 2;
}

void Chip8::OP_5xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] == registers[Vy])
        pc += 2;
}

void Chip8::OP_6xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = byte;
}

void Chip8::OP_7xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] += byte;
}

void Chip8::OP_8xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] |= registers[Vy];
}

void Chip8::OP_8xy2()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] &= registers[Vy];
}

void Chip8::OP_8xy3()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] ^= registers[Vy];
}

void Chip8::OP_8xy4()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint16_t sum = registers[Vx] + registers[Vy];

    sum > 255U ? registers[0xF] = 1 : registers[0xF] = 0;               // Ternary If sum is greater than 8 bits

    registers[Vx] = sum & 0xFFu;                                        // Picks the first 8 bits and puts in Vx
}

void Chip8::OP_8xy5()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] > registers[Vy] ? registers[0xF] = 1 : registers[0xF] = 0;
    registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[0xF] = registers[Vx] & 0x1u;                              // Picks the last bit and puts in VF
    registers[Vx] >>= 1;                                                // Divide the binary number by 2
}

void Chip8::OP_8xy7()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vy] > registers[Vx] ? registers[0xF] = 1 : registers[0xF] = 0;
    registers[Vx] = registers[Vy] - registers[Vx];
}

void Chip8::OP_8xyE()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;                     // Picks the first bit and puts in VF
    registers[Vx] <<= 1;                                                // Multiply the binary number by 2
}

void Chip8::OP_9xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] != registers[Vy])
        pc += 2;
}

void Chip8::OP_Annn()
{
    uint16_t address = opcode & 0x0FFFu;
    indexRegister = address;
}

void Chip8::OP_Bnnn()
{
    uint16_t address = opcode & 0x0FFFu;
    
    pc = address + registers[0];
}

void Chip8::OP_Cxkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = randByte(randGen) & byte;
}

void Chip8::OP_Dxyn()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    // Makes the image wrap around the display if going beyond it should
    uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; row++)
    {
        uint8_t spriteByte = memory[indexRegister + row];

        for (unsigned int col = 0; col < 8; col++)
        {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t * screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

            if (spritePixel)
            {
                if (*screenPixel == 0xFFFFFFFF)     // Screen pixel ON, there is a collision
                {
                    registers[0xF] = 1;
                }
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

void Chip8::OP_Ex9E()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[Vx];

    if (keypad[key])
        pc +=2;
}

void Chip8::OP_ExA1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[Vx];

    if (!keypad[key])
        pc +=2;
}

void Chip8::OP_Fx07()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[Vx] = delayTimer;
}

void Chip8::OP_Fx0A()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    if (keypad[0])
    {
        registers[Vx] = 0;
    } else if (keypad[1])
    {
        registers[Vx] = 1;
    } else if (keypad[2])
    {
        registers[Vx] = 2;
    } else if (keypad[3])
    {
        registers[Vx] = 3;
    } else if (keypad[4])
	{
		registers[Vx] = 4;
	} else if (keypad[5])
	{
		registers[Vx] = 5;
	} else if (keypad[6])
	{
		registers[Vx] = 6;
	} else if (keypad[7])
	{
		registers[Vx] = 7;
	} else if (keypad[8])
	{
		registers[Vx] = 8;
	} else if (keypad[9])
	{
		registers[Vx] = 9;
	} else if (keypad[10])
	{
		registers[Vx] = 10;
	} else if (keypad[11])
	{
		registers[Vx] = 11;
	} else if (keypad[12])
	{
		registers[Vx] = 12;
	} else if (keypad[13])
	{
		registers[Vx] = 13;
	} else if (keypad[14])
	{
		registers[Vx] = 14;
	} else if (keypad[15])
	{
		registers[Vx] = 15;
	} else
	{
		pc -= 2;
	}
}

void Chip8::OP_Fx15()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    delayTimer = registers[Vx];
}

void Chip8::OP_Fx18()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    soundTimer = registers[Vx];
}

void Chip8::OP_Fx1E()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    indexRegister += registers[Vx];
}

void Chip8::OP_Fx29()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = registers[Vx];

    indexRegister = FONTSET_START_ADDRESS + (5 * digit);
}

void Chip8::OP_Fx33()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t value = registers[Vx];

    // Ones-Place
    memory[indexRegister + 2] = value %10;
    value /= 10;

    // Tens-Place
    memory[indexRegister + 1] = value %10;
    value /= 10;

    // Hundreds-Place
    memory[indexRegister] = value %10;
}

void Chip8::OP_Fx55()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; i++)
    {
        memory[indexRegister + i] = registers[i];
    }
}

void Chip8::OP_Fx65()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; i++)
    {
        registers[i] = memory[indexRegister + i];
    }
}