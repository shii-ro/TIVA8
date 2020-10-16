# CHIP-8-TIVA-C-SERIES
A Chip 8 Interpreter for TIVA C Series TM4C123G.
Video is renderized in a ST7920 display via 4-bit parellel mode.

#Pin Settings 
E   PD2
RW  PD1
RS  PD0

DB4 PC4
DB5 PC5
DB6 PC6
DB7 PC7

#TODO
-Add suport for a 4x4 matrix keyoard.
-Add suport for a simple beep.
-Optimize display rendering function.
-Optimize delays.
-Optimize LCD driver and add 8 bit mode / serial mode.
-Reduce memory usage.