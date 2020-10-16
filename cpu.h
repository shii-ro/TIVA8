#ifndef CPU_H_
#define CPU_H_

#include <ctype.h>
#include <stdbool.h>

typedef unsigned char BYTE;
typedef unsigned short WORD;

#define MEM_SIZE    4096u
#define REG_SIZE    16u
#define KEY_SIZE    16u
#define STACK_SIZE  16u
#define VIDEO_SIZE  128 * 64
#define V_WIDTH     128
#define V_HEIGHT    64
#define START_ADDRESS 0x200
#define FONTSET_SIZE 80
#define FONTSET_ADDRESS 0x50
#define OPCODE (MEMORY[c.PC] << 8 | MEMORY[c.PC + 1])
#define NNN (OPCODE & 0x0FFF)
#define NN  (OPCODE & 0x00FF)
#define N   (OPCODE & 0x000F)
#define X   ((OPCODE & 0X0F00) >> 8)
#define Y   ((OPCODE & 0x00F0) >> 4)
#define F   ((OPCODE & 0xF000) >> 12)
#define SCREEN_WIDTH    64
#define SCREEN_HEIGHT   32


struct chip8_cpu_struct
{
    BYTE V[REG_SIZE];           //16 1byte registers, VF register used as flag     //4096 bytes of memory, programs starts at 0x200
    WORD I;                     //Address register, used on writing and reading memory        //16 keys
    WORD STACK[STACK_SIZE];     //16, 2bytes wide stack
    BYTE SP;                    //1byte stack pointer
    WORD PC;                    //Program Counter
    BYTE D_TIMER;               //Delay timer
    BYTE S_TIMER;               //Sound timer
};

extern struct chip8_cpu_struct c;

extern BYTE VIDEO[V_HEIGHT][V_WIDTH];
extern BYTE KEY[KEY_SIZE];
extern BYTE MEMORY[MEM_SIZE];
extern int cycles;

static const BYTE fontset[FONTSET_SIZE] = { 0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
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

void init_chip8();
void game_cycle();
void update_display();
void clear_display();
#endif /* CPU_H_ */
