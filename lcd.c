#include <tm4c123gh6pm.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "lcd.h"

#define HIGH  (db & 0xF0)
#define LOW   ((db & 0x0F) << 4)

#define E   (1u << 2)   //PD2
#define RW  (1u << 1)   //PD1
#define RS  1u          //PD0

#define DB4 (1u << 4)          //PC4
#define DB5 (1u << 5)          //PC5
#define DB6 (1u << 6)          //PC6
#define DB7 (1u << 7)          //PC7

void init_PORTS()
{
    unsigned long volatile delay;
    SYSCTL_RCGCGPIO_R |= (SYSCTL_RCGC2_GPIOD | SYSCTL_RCGC2_GPIOC); //Enable clock for PORTD and PORTC
    delay = SYSCTL_RCGC2_R;

    GPIO_PORTC_DIR_R = (DB4 | DB5 | DB6 | DB7);   //Set specified pins as output
    GPIO_PORTD_DIR_R |= (RS | RW | E);            //Set specified pins as output

    GPIO_PORTC_DEN_R = (DB4 | DB5 | DB6 | DB7);
    GPIO_PORTD_DEN_R |= (RS | RW | E);           //Digital enable specified pins

}


void delay_ms(float value)
{
    double ms = value * 16000;
    NVIC_ST_CTRL_R = 0; /* (1) disable SysTick during setup */
    NVIC_ST_RELOAD_R = ms - 1; /* (2) number of counts to wait */
    NVIC_ST_CURRENT_R = 0; /* (3) any value written to CURRENT clears */
    NVIC_ST_CTRL_R |= 0x5; /* (4) enable SysTick with core clock */

    while ((NVIC_ST_CTRL_R & 0x00010000) == 0)
    {
        ; /* wait for COUNT flag */
    }
}

void lcd_enable()
{
    GPIO_PORTD_DATA_BITS_R[E] = E;
    delay_ms(0.002);
    GPIO_PORTD_DATA_BITS_R[E] = ~E;
}

void lcd_data(unsigned char db)
{
    delay_ms(0.100);
    GPIO_PORTD_DATA_R = 0b00000001;       //RS =1, E=0, RW=0 == Write data mode

    GPIO_PORTC_DATA_R = HIGH;
    lcd_enable();

    GPIO_PORTC_DATA_R = LOW;
    lcd_enable();
}

void lcd_instr(unsigned char db)
{
    delay_ms(0.100);
    GPIO_PORTD_DATA_R = 0x00;             //RS =0, E=0, RW=0 == Write Instruction mode

    GPIO_PORTC_DATA_R = HIGH;             //Send HIGH to databus
    lcd_enable();

    GPIO_PORTC_DATA_R = LOW;              //Send LOW to databus
    lcd_enable();
}

void lcd_write_graphics(const unsigned char *image)
{
    unsigned char x, y;
    for (y = 0; y < 64; y++)
    {
        if (y < 32)
        {
            lcd_instr(SET_GRAM_ADDR | y);
            lcd_instr(0x80);
        }
        else
        {
            lcd_instr(SET_GRAM_ADDR | (y - 32));
            lcd_instr(0x88);
        }
        for (x = 0; x < 8; x++)
        {
            lcd_data(image[x * 2 + 16 * y]);
            lcd_data(image[x * 2 + 1 + 16 * y]);
        }
    }
}

void lcd_fill_screen_graphics(unsigned char fill_value)
{
    unsigned char x, y;
    for (y = 0; y < 64; y++)
    {
        if (y < 32)
        {
            lcd_instr(SET_GRAM_ADDR | y);
            lcd_instr(0x80);
        }
        else
        {
            lcd_instr(SET_GRAM_ADDR | (y - 32));
            lcd_instr(0x88);
        }
        for (x = 0; x < 8; x++)
        {
            lcd_data(fill_value);
            lcd_data(fill_value);
        }
    }
}

unsigned char lcd_read_byte(unsigned char addr_y, unsigned char addr_x)
{
    unsigned char byte = 0;

    lcd_instr(SET_GRAM_ADDR | addr_y);
    lcd_instr(SET_GRAM_ADDR | addr_x);


    GPIO_PORTC_DIR_R &= ~(DB4 | DB5 | DB6 | DB7);
    GPIO_PORTD_DATA_BITS_R[RW] = RW;
    GPIO_PORTD_DATA_BITS_R[RS] = RS;

    ////////after address set to read, dummy read(unless another read_byte issued)
    ////Dummy read
    //get upper 4 bits
    delay_ms(0.002);
    GPIO_PORTD_DATA_BITS_R[E] = E;
    delay_ms(0.002);
    GPIO_PORTD_DATA_BITS_R[E] = ~E;
    //get lower 4bits
    delay_ms(0.002);
    GPIO_PORTD_DATA_BITS_R[E] = E;
    delay_ms(0.002);
    GPIO_PORTD_DATA_BITS_R[E] = ~E;
    ////Dummy read end

    //get upper 4 bits
    delay_ms(0.002);
    GPIO_PORTD_DATA_BITS_R[E] = E;
    byte = GPIO_PORTC_DATA_R & 0xF0;
    delay_ms(0.002);
    GPIO_PORTD_DATA_BITS_R[E] = ~E;

    //get lower 4bits
    delay_ms(0.002);
    GPIO_PORTD_DATA_BITS_R[E] = E;
    byte |= (GPIO_PORTC_DATA_R & 0xF0) >> 4;
    delay_ms(0.002);
    GPIO_PORTD_DATA_BITS_R[E] = ~E;

    GPIO_PORTD_DATA_BITS_R[RW] = ~RW;
    GPIO_PORTD_DATA_BITS_R[RS] = ~RS;
    GPIO_PORTC_DIR_R |= (DB4 | DB5 | DB6 | DB7);

    return byte;
}


void init_DISPLAY()
{
    delay_ms(50);

    //Function Set  4  bit
    lcd_instr(FUNCTION_SET_4BIT | BASIC);
    delay_ms(0.120);

    //again
    lcd_instr(FUNCTION_SET_4BIT | BASIC);
    delay_ms(0.120);

    //Display on off control
    lcd_instr(DISPLAY_STATUS | DISPLAY_ON | CURSOR_OFF | BLINK_OFF);
    delay_ms(0.120);

    //Display Clear
    lcd_instr(DISPLAY_CLEAR);
    delay_ms(12);

    //Entry mode set
    lcd_instr(ENTRY_MODESET | AC_INCREASE);
    delay_ms(0.080);
}

void init_DISPLAY_GRAPHICS()
{
    delay_ms(50);

    //Function Set  4  bit
    lcd_instr(FUNCTION_SET_4BIT | BASIC);
    delay_ms(0.120);

    //again
    lcd_instr(FUNCTION_SET_4BIT | BASIC);
    delay_ms(0.120);

    //Display on off control
    lcd_instr(DISPLAY_STATUS | DISPLAY_ON | CURSOR_OFF | BLINK_OFF);
    delay_ms(0.120);

    //Display Clear
    lcd_instr(DISPLAY_CLEAR);
    delay_ms(12);

    //Entry mode set
    lcd_instr(ENTRY_MODESET | AC_INCREASE);
    delay_ms(0.080);

    //Display on off control
    lcd_instr(DISPLAY_STATUS | DISPLAY_ON | CURSOR_OFF | BLINK_OFF);
    delay_ms(0.080);

    //Display Clear
    lcd_instr(DISPLAY_CLEAR);
    delay_ms(10);

    //Entry mode set
    lcd_instr(ENTRY_MODESET | AC_INCREASE);
    delay_ms(0.080);

    //Select Extended mode set
    lcd_instr(FUNCTION_SET_4BIT | EXTENDED);
    delay_ms(0.120);

    //Graphics Display On
    lcd_instr(FUNCTION_SET_4BIT | EXTENDED | G_DISPLAY_ON);
    delay_ms(0.120);

    lcd_fill_screen_graphics(0x00);
}



