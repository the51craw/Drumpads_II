#ifndef DISP_JHD162A_H_DEFINED
#define DISP_JHD162A_H_DEFINED

/**
Print one line to buffer.
*/
void PrintLine1(rom far char * p);
void PrintLine2(rom far char * p);

/**
Print an integer.
value = value to print.
length = number of digits. negative to reserve space for minus sign.
line = line to print on, 1 or 2.
pos = position on line, 1 to 16.
*/
void PrintNumeric(int value, int length, int line, int pos);

/**
Clear display and set current position to 0:
*/
void Clr(void);
void lcd_init(void);
void pause(unsigned char p);
void delay_10_ms(void);

#endif
