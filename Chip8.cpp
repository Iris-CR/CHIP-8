#include <cstdint>
#include <fstream>
#include <chrono>
#include <random>

class Chip8
{
    public:
        uint8_t registers[16]{};        // Coulb be named "Vx"
        uint8_t memory[4096]{};
        uint16_t indexRegister{};
        uint16_t pc{};
        uint16_t stack[16]{};
        uint8_t delayTimer{};
        uint8_t soundTimer{};
        uint8_t keypad[16]{};
        uint32_t video[64*32]{};
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
