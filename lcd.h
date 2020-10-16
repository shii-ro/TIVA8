#ifndef LCD_H_
#define LCD_H_

#define DISPLAY_CLEAR     0b00000001
#define HOME              0b00000010

//Function set
#define FUNCTION_SET_4BIT 0b00100000
#define BASIC             0b000
#define EXTENDED          0b100

//Entry mode
#define ENTRY_MODESET     0b00000100
#define AC_INCREASE       0b10
#define AC_DECREASE       0b00
#define SHIFT_DISPLAY_L   0b11
#define SHIFT_DISPLAY_R   0b10

//Display status
#define DISPLAY_STATUS    0b00001000
#define DISPLAY_ON        0b100
#define DISPLAY_OFF       0b000
#define CURSOR_ON         0b10
#define CURSOR_OFF        0b00
#define BLINK_ON          0b1
#define BLINK_OFF         0b0

//Extended mode set options
#define G_DISPLAY_ON     0b10
#define G_DISPLAY_OFF    0b00
#define SET_GRAM_ADDR    0x80



void init_PORTS();
void delay_ms(float value);
void lcd_enable();
void lcd_data(unsigned char db);
void lcd_instr(unsigned char db);
void lcd_write_graphics(const unsigned char *image);
void lcd_fill_screen_graphics(unsigned char fill_value);
unsigned char lcd_read_byte(unsigned char addr_y, unsigned char addr_x);

void init_DISPLAY();
void init_DISPLAY_GRAPHICS();
void lcd_akari();

#endif /* LCD_H_ */
