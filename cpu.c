#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cpu.h"
#include "rom.h"
#include "lcd.h"

struct chip8_cpu_struct c;

BYTE VIDEO[V_HEIGHT][V_WIDTH];
BYTE KEY[KEY_SIZE];
BYTE MEMORY[MEM_SIZE];
unsigned char new_array[1024];
int cycles;

static void OC_00E0()
{
    int x, y;
    for(y = 0; y < 64; y++)
        for(x = 0; x < 32; x++)
            VIDEO[x][y] = 0x00;
    lcd_fill_screen_graphics(0x00);
}

static void OC_00EE()
{
    c.PC = c.STACK[c.SP];
    c.PC -= 2;
    c.SP--;

}

static void OC_1XXX()
{
    c.PC = NNN;
    c.PC -= 2;
}

static void OC_2XXX()
{
    c.SP++;
    c.STACK[c.SP] = c.PC + 2;
    c.PC = NNN;
    c.PC -= 2;
}

static void OC_3XXX()
{
    if (c.V[X] == NN) c.PC += 2;
}

static void OC_4XXX()
{
    if (c.V[X] != NN) c.PC += 2;
}

static void OC_5XXX()
{
    if (c.V[X] == c.V[Y]) c.PC += 2;
}

static void OC_6XXX()
{
    c.V[X] = NN;
}

static void OC_7XXX()
{
    c.V[X] += NN;
}

static void OC_8XY0()
{
    c.V[X] = c.V[Y];
}
static void OC_8XY1()
{
    c.V[X] = c.V[X] | c.V[Y];
}

static void OC_8XY2()
{
    c.V[X] = c.V[X] & c.V[Y];
}

static void OC_8XY3()
{
    c.V[X] = c.V[X] ^ c.V[Y];
}

static void OC_8XY4()
{
    if (c.V[X] + c.V[Y] > 255) c.V[0xF] = 1;
    else c.V[0xF] = 0;
    c.V[X] += c.V[Y];
}

static void OC_8XY5()
{
    if (c.V[X] > c.V[Y]) c.V[0xF] = 1;
    else c.V[0xF] = 0;
    c.V[X] -= c.V[Y];
}

static void OC_8XY6()
{
    if (c.V[X] % 2 == 1) c.V[0xF] = 1;
    else c.V[0xF] = 0;
    c.V[X] = c.V[X] >> 1;
}

static void OC_8XY7()
{
    if (c.V[Y] > c.V[X]) c.V[0xF] = 1;
    else c.V[0xF] = 0;
    c.V[X] = c.V[Y] - c.V[X];
}

static void OC_8XYE()
{
    c.V[0xF] = (c.V[X] & 0x80u) >> 7u;
    c.V[X] = c.V[X] << 1;
}

static void OC_9XXX()
{
    if (c.V[X] != c.V[Y])
        c.PC += 2;
}
static void OC_AXXX()
{
    c.I = NNN;
}

static void OC_BXXX()
{
    c.PC = c.V[X] + NNN;
    //c.PC-=2;
}

static void OC_CXXX()
{

    c.V[X] = (rand() % 255) & NN;
}

static void OC_DXXX()
{
    int Y_pos = c.V[Y] % 31;
    int X_pos = c.V[X] % 63;
    unsigned char byte_index;
    unsigned char bit_index;

    c.V[0xF] = 0;
    for (byte_index = 0; byte_index < N; byte_index++)
    {
        unsigned char byte = MEMORY[c.I + byte_index];

        for (bit_index = 0; bit_index < 8; bit_index++)
        {
            if((byte & (0x80u >> bit_index)) != 0)
            {
                if(VIDEO[Y_pos + byte_index][X_pos + bit_index] != 0) c.V[0xF] = 1;
                VIDEO[Y_pos + byte_index][X_pos + bit_index] ^= 1;
            }
        }

        unsigned int i, x, y;


        for (y = 0; y < 2; y++)
        {
            unsigned int Y_p = (Y_pos + byte_index) * 2 + y;

            if (Y_p < 32)
            {
                lcd_instr(SET_GRAM_ADDR | Y_p);
                lcd_instr(0x80);
            }
            else
            {
                lcd_instr(SET_GRAM_ADDR | Y_p - 32);
                lcd_instr(0x88);
            }
            for (x = 0; x < 8; x++)
            {
                unsigned short word = 0;
                for (i = 0; i < 8; i++)
                {
                    word |= (VIDEO[Y_pos + byte_index][(x * 8) + i] & 1u) << ((7 - i) * 2);
                    word |= (VIDEO[Y_pos + byte_index][(x * 8) + i] & 1u) << ((7 - i) * 2 + 1);
                }
                lcd_data((word >> 8) & 0x0FF);
                lcd_data(word & 0x0FF);
            }
        }
    }
}

static void OC_EX9E()
{
    if (KEY[c.V[X]])
        c.PC += 2;
}

static void OC_EXA1()
{
    if (KEY[c.V[X]])
        c.PC += 2;
}

static void OC_FX07()
{
    c.V[X] = c.D_TIMER;
}

static void OC_FX0A()
{
    //todo
    c.PC -= 2;
}

static void OC_FX15()
{
    c.D_TIMER = c.V[X];
}

static void OC_FX18()
{
    c.S_TIMER = c.V[X];
}

static void OC_FX1E()
{
    if (c.I + c.V[X] > 0xFFF)
        c.V[0xF] = 1;
    else
        c.V[0xF] = 0;
    c.I += c.V[X];
}

static void OC_FX29()
{
    c.I = MEMORY[c.V[X] + 0x50] * 5;
}

static void OC_FX33()
{
    unsigned char value = c.V[X];
    MEMORY[c.I + 2] = value % 10;
    value /= 10;
    MEMORY[c.I + 1] = value % 10;
    value /= 10;
    MEMORY[c.I] = value % 10;
}

static void OC_FX55()
{
    int i;
    for (i = 0; i <= X; i++)
    {
        MEMORY[c.I + i] = c.V[i];
    }
    c.I += (X + 1);
}
static void OC_FX65()
{
    int i;
    for (i = 0; i <= X; i++)
    {
        c.V[i] = MEMORY[c.I + i];
    }
    c.I += (X + 1);
}

void init_chip8()
{
    c.PC = START_ADDRESS;
    c.I = 0;
    c.D_TIMER = c.S_TIMER = 0;
    c.SP = 0;
    cycles = 0;
    WORD i, k;

    for(i = 0; i < V_HEIGHT; i++)
            {
                for(k = 0; k < V_WIDTH; k++)
                    VIDEO[i][k] = 0;
            }

    for (i = 0; i <= FONTSET_SIZE; i++)
        MEMORY[FONTSET_ADDRESS + i] = fontset[i];

    for (i = 0; i < 16; i++)
        KEY[i] = 0;

    for (i = 0; i <= invaders_size; i++)
        MEMORY[START_ADDRESS + i] = invaders_hex[i];
}

void game_cycle()
{
    switch (F)
    {
    case 0x0:
        switch (NN)
        {
        case 0xE0:
            OC_00E0();
            break;
        case 0xEE:
            OC_00EE();
            break;
        };break;
    case 0x01:
        OC_1XXX();
        break;
    case 0x02:
        OC_2XXX();
        break;
    case 0x03:
        OC_3XXX();
        break;
    case 0x04:
        OC_4XXX();
        break;
    case 0x05:
        OC_5XXX();
        break;
    case 0x06:
        OC_6XXX();
        break;
    case 0x07:
        OC_7XXX();
        break;
    case 0x08:
        switch (N)
        {
        case 0x00:
            OC_8XY0();
            break;
        case 0x01:
            OC_8XY1();
            break;
        case 0x02:
            OC_8XY2();
            break;
        case 0x03:
            OC_8XY3();
            break;
        case 0x04:
            OC_8XY4();
            break;
        case 0x05:
            OC_8XY5();
            break;
        case 0x06:
            OC_8XY6();
            break;
        case 0x07:
            OC_8XY7();
            break;
        case 0x0E:
            OC_8XYE();
            break;
        };break;
    case 0x09:
        OC_9XXX();
        break;
    case 0x0A:
        OC_AXXX();
        break;
    case 0x0B:
        OC_BXXX();
        break;
    case 0x0C:
        OC_CXXX();
        break;
    case 0x0D:
        OC_DXXX();
        break;
    case 0x0E:
        switch (N)
        {
        case 0x01:
            OC_EXA1();
            break;
        case 0x0E:
            OC_EX9E();
            break;
        };break;
    case 0x0F:
        switch (NN)
        {
        case 0x07:
            OC_FX07();
            break;
        case 0x15:
            OC_FX15();
            break;
        case 0x18:
            OC_FX18();
            break;
        case 0x1E:
            OC_FX1E();
            break;
        case 0x29:
            OC_FX29();
            break;
        case 0x33:
            OC_FX33();
            break;
        case 0x55:
            OC_FX55();
            break;
        case 0x65:
            OC_FX65();
            break;
        };break;
    }

    cycles++;
    if (cycles == 8)
    {
        if (c.D_TIMER > 0)
            c.D_TIMER--;
        if (c.S_TIMER > 0)
            c.S_TIMER--;
        cycles = 0;
    }
    c.PC += 2;
}
