#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "cpu.h"
#include "lcd.h"

int main(void){

    init_PORTS();
    init_DISPLAY_GRAPHICS();
    init_chip8();

    while(1)
    {
        game_cycle();
        delay_ms(1);
    }
}


