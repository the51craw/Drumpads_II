#include "p18f4550.h"
#include <string.h>
#include "disp_JHD162A_drv.h"

// #pragma warning(disable:2058)

////////////////////////////////////////////////////////////////
// Driver internals
////////////////////////////////////////////////////////////////

// Display control port and pins.
// Port is PORTC and pins 0-2 are in use.
#define DISPLAY_CONTROL_PORT PORTC
#define DISPLAY_CONTROL_TRIS TRISC
#define DISPLAY_CONTROL_LAT  LATC
#define DISPLAY_RS_PIN 0x04
#define DISPLAY_RW_PIN 0x02
#define DISPLAY_E_PIN  0x01
#define DISPLAY_CONTROL_PINS (DISPLAY_RS_PIN | DISPLAY_RW_PIN | DISPLAY_E_PIN)

// Display data/command port is PORTD
#define DISPLAY_DATA_PORT PORTD
#define DISPLAY_DATA_TRIS TRISD
#define DISPLAY_DATA_LAT LATD
#define DISPLAY_DATA_PINS 0xFF

// R/W Read/Write      0 = write, 1 = read
// RS  Register Select 0 = control(command) register, 1 = data register
// E   Enable          0 = disable, 1 = enable (actually, clocks in data/command on negative flank)
// Pin manipulation macros:
#define SET_RS_PIN                 DISPLAY_CONTROL_LAT  |= DISPLAY_RS_PIN
#define RESET_RS_PIN               DISPLAY_CONTROL_LAT  &= ~DISPLAY_RS_PIN
#define SET_RW_PIN                 DISPLAY_CONTROL_LAT |= DISPLAY_RW_PIN
#define RESET_RW_PIN               DISPLAY_CONTROL_LAT &= ~DISPLAY_RW_PIN
#define SET_E_PIN                  DISPLAY_CONTROL_LAT |= DISPLAY_E_PIN
#define RESET_E_PIN                DISPLAY_CONTROL_LAT &= ~DISPLAY_E_PIN

// Command definitions for lcd_wrcmd(void)
#define CLEAR_DISPLAY 0x01
#define CURSOR_HOME 0x02
#define ENTRY_MODE_SET_INCREMENT_AND_SHIFT_DISPLAY 0x07
#define ENTRY_MODE_SET_DECREMENT_AND_SHIFT_DISPLAY 0x05
#define ENTRY_MODE_SET_INCREMENT_AND_DO_NOT_SHIFT_DISPLAY 0x06
#define ENTRY_MODE_SET_DECREMENT_AND_DO_NOT_SHIFT_DISPLAY 0x04
#define DISPLAY_ON_CURSOR_ON_BLINKING_ON 0x0F
#define DISPLAY_OFF_CURSOR_ON_BLINKING_ON 0x0B
#define DISPLAY_ON_CURSOR_OFF_BLINKING_ON 0x0D
#define DISPLAY_OFF_CURSOR_OFF_BLINKING_ON 0x09
#define DISPLAY_ON_CURSOR_ON_BLINKING_OFF 0x0E
#define DISPLAY_OFF_CURSOR_ON_BLINKING_OFF 0x0A
#define DISPLAY_ON_CURSOR_OFF_BLINKING_OFF 0x0C
#define DISPLAY_OFF_CURSOR_OFF_BLINKING_OFF 0x08
#define CURSOR_SHIFT_LEFT 0x18
#define CURSOR_SHIFT_RIGHT 0x1C
#define CURSOR_NO_SHIFT 0x10
#define SYSTEM_SET_EIGHT_BITS_TWO_LINES_5X10_DOTS 0x3C
#define SYSTEM_SET_EIGHT_BITS_TWO_LINES_5X7_DOTS 0x38
#define SYSTEM_SET_EIGHT_BITS_ONE_LINE_5X10_DOTS 0x34
#define SYSTEM_SET_EIGHT_BITS_ONE_LINE_5X7_DOTS 0x30
#define SYSTEM_SET_FOUR_BITS_TWO_LINES_5X10_DOTS 0x2C
#define SYSTEM_SET_FOUR_BITS_TWO_LINES_5X7_DOTS 0x28
#define SYSTEM_SET_FOUR_BITS_ONE_LINE_5X10_DOTS 0x24
#define SYSTEM_SET_FOUR_BITS_ONE_LINE_5X7_DOTS 0x20
#define SET_CG_RAM_ADDRESS 0x40 //Or on address as last 6 bits
#define SET_DD_RAM_ADDRESS 0x80 //Or on address as last 7 bits

// Internal function prototypes
void soft_nop(void);
void lcd_wrcmd(unsigned char lcdcmd);
void lcd_wrdta(unsigned char lcddta);
void lcd_init(void);
unsigned char lcd_checkbusy(void);
void print_line_rom(rom far char * p);
void print_line_ram(char * p);
void my_atoi(char * dest, int length, int value);
void my_strcpy(char * dest, char * source);

unsigned char data_length;
char digits[32];

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// Exported functions
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

void PrintLine1(rom far char * p)
{
	pause(1);
	lcd_wrcmd(SET_DD_RAM_ADDRESS + 0x00);
	print_line_rom(p);
}

void PrintLine2(rom far char * p)
{
	pause(1);
	lcd_wrcmd(SET_DD_RAM_ADDRESS + 0x40);
	print_line_rom(p);
}

void PrintNumeric(int value, int length, int line, int pos)
{
	pause(1);
	my_atoi(digits, length, value);
	if (line == 2)
	{
		pos += 0x40;
	}
	lcd_wrcmd(SET_DD_RAM_ADDRESS + pos);
//strcpy(digits, "Va faan?");
//my_strcpy(digits, "Va faan?");
//digits [0] = 'V';
//digits [1] = 'a';
//digits [2] = ' ';
//digits [3] = 'f';
//digits [4] = 'a';
//digits [5] = 'a';
//digits [6] = 'n';
//digits [7] = '?';
//digits [8] = 0;
	print_line_ram(digits);
}

////////////////////////////////////////////////////////////////
// Clear display:
////////////////////////////////////////////////////////////////

void Clr(void)
{
	lcd_wrcmd(CLEAR_DISPLAY);
	lcd_wrcmd(CURSOR_HOME);
}

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// Internal driver functions:
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// Atoi replacement
////////////////////////////////////////////////////////////////

int power(int i)
{
	int result = 1;

	while (--i)
	{
		result *= 10;
	}

	return result;
}

void my_atoi(char * dest, int length, int value)
{
	int v = value;
	int temp;
	int l = length;
	int pos = 0;

	if (l < 0)
	{
		l = -l;
		if (v < 0)
		{
			dest[pos++] = '-';
			v = -v;
		}
		else
		{
			dest[pos++] = ' ';
		}
	}

	while (l)
	{
		temp = v / power(l);
		dest[pos++] = 0x30 + temp;
		v = v - temp * power(l);
		l--;
	}
	dest[pos] = 0;
}

void my_strcpy(char * dest, char * source)
{
	int i = 0;
	while (i < 16 && ((dest[i] = source[i]) != 0)) i++;
}

void print_line_ram(char * p)
{
	int i = 16;
	
	pause(1);

	while (i-- && *p)
	{
		lcd_wrdta(*p++);
	}
}

void print_line_rom(rom far char * p)
{
	int i = 16;
	
	pause(1);

	while (i-- && *p)
	{
		lcd_wrdta(*p++);
	}
}

////////////////////////////////////////////////////////////////
// Pauses and wait while busy
////////////////////////////////////////////////////////////////

void pause(unsigned char p)
{
	unsigned char i;
	unsigned char j;

	for(i = 0; i < p; i++)
	{
		for(j = 0; j < 0xFE; j++)
		{
			j = j;
		}
	}
}

void delay_10_ms(void)
{
	unsigned char i;
	for (i = 0; i < (unsigned int)10; i++)
	{
		soft_nop();
	}
}

void soft_nop(void)
{
	unsigned char i;
	for(i = 0; i < 0x80; i++);
}

unsigned char lcd_checkbusy(void) // Check busy on bit 7
{
	unsigned char lcdstate;
	DISPLAY_DATA_TRIS |= DISPLAY_DATA_PINS;			// Set data port to input
	DISPLAY_CONTROL_TRIS &= ~DISPLAY_CONTROL_PINS;	// Set command port to output
	RESET_E_PIN;									// Should always be low except when pulsing in data/command
	RESET_RS_PIN;            						// Set command mode
	SET_RW_PIN;
	soft_nop();
	SET_E_PIN;
	soft_nop();
	lcdstate = DISPLAY_DATA_LAT;      				// Read status bit
	soft_nop();
	RESET_E_PIN;
	soft_nop();
	return(lcdstate & 0x80);
}

////////////////////////////////////////////////////////////////
// Read and write commands
////////////////////////////////////////////////////////////////

void lcd_wrcmd(unsigned char lcdcmd) // Write command in lcdcmd
{
	DISPLAY_DATA_TRIS &= ~DISPLAY_DATA_PINS;		// Set data/command port to output
	DISPLAY_CONTROL_TRIS &= ~DISPLAY_CONTROL_PINS;
//	while(lcd_checkbusy(void));
	RESET_E_PIN;									// Should always be low except when pulsing in data/command
	RESET_RW_PIN;									// Set write mode
	RESET_RS_PIN;									// Set command mode
/*	if(data_length == 4)
	{
		soft_nop(void);
		SET_E_PIN;									// Enable chip
		DISPLAY_DATA_PORT = lcdcmd & 0x0f;			// Put data on port
		soft_nop(void);
		SET_E_PIN;									// Enable chip
		soft_nop(void);
		RESET_E_PIN;								// Disable chip (latching in data)
		soft_nop(void);
		DISPLAY_DATA_PORT = (lcdcmd & 0xf0) >> 4;	// Put data on port
		SET_E_PIN;									// Enable chip
		soft_nop(void);
		RESET_E_PIN;								// Disable chip (latching in data)
	}
	else
*/	{
		soft_nop();
		SET_E_PIN;									// Enable chip
		soft_nop();
		DISPLAY_DATA_PORT = lcdcmd;					// Put data on port
		soft_nop();
		RESET_E_PIN;								// Disable chip (latching in data)
	}
	soft_nop();
} 

void lcd_wrdta(unsigned char lcddta)				// Write data in lcddta
{
	DISPLAY_DATA_TRIS &= ~DISPLAY_DATA_PINS;		// Set data/command port to output
	DISPLAY_CONTROL_TRIS &= ~DISPLAY_CONTROL_PINS;
//	while(lcd_checkbusy(void));
	RESET_E_PIN;									// Should always be low except when pulsing in data/command
	SET_RS_PIN;										// Set data mode
	RESET_RW_PIN;									// Set write mode
/*	if(data_length == 4)
	{
		soft_nop(void);
		SET_E_PIN;									// Enable chip
		DISPLAY_DATA_PORT = lcddta & 0x0f;			// Put data on port
		soft_nop(void);
		SET_E_PIN;									// Enable chip
		soft_nop(void);
		RESET_E_PIN;								// Disable chip (latching in data)
		soft_nop(void);
		DISPLAY_DATA_PORT = (lcddta & 0xf0) >> 4;	// Put data on port
		SET_E_PIN;									// Enable chip
		soft_nop(void);
		RESET_E_PIN;								// Disable chip (latching in data)
	}
	else
*/	{
		soft_nop();
		SET_E_PIN;									// Enable chip
		soft_nop();
		DISPLAY_DATA_PORT = lcddta;					// Put data on port
		soft_nop();
		RESET_E_PIN;								// Disable chip (latching in data)
	}
	soft_nop();
} 

////////////////////////////////////////////////////////////////
// Initialization
////////////////////////////////////////////////////////////////

void lcd_init(void)
{
	data_length = 8; // JHD162A does NOT support four bits data transfers!!!
	SPPCON = 0x00;   // Disable Streaming Parallel Port
	CCP1CON = 0x00;  // Disable Capture/Compare/PWM (ECCP) module
	DISPLAY_CONTROL_LAT &= 0xF8;
/*	if(data_length == 4)
	{
		lcd_wrcmd(SYSTEM_SET_FOUR_BITS_TWO_LINES_5X7_DOTS); // Set interface data length 4 bits (bit5=0), number of display lines (bit4=1), font type (bit3=1) 5*11/5*8
	}
	else
*/	{
		lcd_wrcmd(SYSTEM_SET_EIGHT_BITS_TWO_LINES_5X7_DOTS); // Set interface data length 8 bits (bit5=1), number of display lines (bit4=1), font type (bit3=0) 5*11/5*8
	}
	lcd_wrcmd(DISPLAY_ON_CURSOR_OFF_BLINKING_OFF); // Set Display (bit3=1), Display on (bit2=1), Cursor off (bit1=0), Blinking off (bit0=0)
	lcd_wrcmd(ENTRY_MODE_SET_INCREMENT_AND_DO_NOT_SHIFT_DISPLAY); // Set mode (bit2=1), Increment on (bit1=1), Shift off (bit0=0)
	lcd_wrcmd(CURSOR_NO_SHIFT);
	lcd_wrcmd(CLEAR_DISPLAY); // Clear display (bit0=1)
	lcd_wrcmd(CURSOR_HOME);
}

