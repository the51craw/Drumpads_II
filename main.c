// Uncomment line below to have velocity displayed for test:
//#define DISPLAY_VELOCITY

#include "p18f4550.h"
#include "config.h"
#include "MIDI_drv.h"
#include "DrumPad_drv.h"
#include "disp_JHD162A_drv.h"
#include "keyboard.h"
#include "main.h"
#include "menu.h"
#include "eeprom_low_level_driver.h"
#include <string.h>

void MIDI_Init(void);
void MIDI_Send(unsigned char MIDI_data);

// These are the original sounds setup. It is not used anymore, but still in code for reference.
// If EEPROM is empty (which the ¤%&# MPLAB causes everytime) we will use these values. Test on bd.
// Setup is now in EEPROM, first page.
#define OUCH_S       (unsigned char)29
#define BASTRUMMA    (unsigned char)36
#define SIDSDK1      (unsigned char)37
#define VIRVELTRUMMA (unsigned char)38
#define CLAP808      (unsigned char)39
#define RVB_S1       (unsigned char)40
#define HIGHAT       (unsigned char)42
#define ROOM_T2      (unsigned char)43
#define PEDALHH      (unsigned char)44
#define OPENHH       (unsigned char)46
#define ROOM_T3      (unsigned char)47
#define CRSCYM       (unsigned char)49
#define ROOM_T4      (unsigned char)50
#define RIDE         (unsigned char)51
#define TABMOUTINE   (unsigned char)54
#define COWBELL      (unsigned char)56
#define IMPACT_S     (unsigned char)58
#define CAN1         (unsigned char)60
#define LBONG        (unsigned char)61
#define HCONG        (unsigned char)63
#define LCONG        (unsigned char)64
#define AGOGO2       (unsigned char)67
#define AGOGO1       (unsigned char)68
#define CABASA       (unsigned char)69
#define MARACAS      (unsigned char)70
#define CLAVES1      (unsigned char)75
#define CLAVES2      (unsigned char)82

// The following are not pads, they are names for the sounds in the MPX_8 module and plays on channel 10 (0x09)!
#define MPX8_1 (unsigned char)101			// 22 - MPX8 pad 1
#define MPX8_2 (unsigned char)102			// 23 - MPX8 pad 2
#define MPX8_3 (unsigned char)103			// 24 - MPX8 pad 3
#define MPX8_4 (unsigned char)104			// 25 - MPX8 pad 4
#define MPX8_5 (unsigned char)105			// 26 - MPX8 pad 5
#define MPX8_6 (unsigned char)106			// 27 - MPX8 pad 6
#define MPX8_7 (unsigned char)107			// 28 - MPX8 pad 7
#define MPX8_8 (unsigned char)108			// 29 - MPX8 pad 8

#define DRUM_CHANNEL 0x09
#define SOUND_DURATION 5000

void TMR_Interrupt(void);
void Test_hit(void);
void long_pause(void);

#pragma code lo_vector = 0x18
void interrupt_at_lo_vector(void)
{
  _asm GOTO TMR_Interrupt _endasm
}
#pragma code

#pragma interruptlow TMR_Interrupt
// At Clock = 20 MHz we get here at 1 ms intervals.
void TMR_Interrupt(void)
{
	DrumPad_IncrementTimers();
#ifdef CLOCK_FREQUENCY_20_MHZ
	TMR0L = 237;
#endif
#ifdef CLOCK_FREQUENCY_48_MHZ
	TMR0L = 210;
#endif
	INTCON &= 0b11111011;
}

unsigned char Sound[NUMBER_OF_PADS];
unsigned char Channel[NUMBER_OF_PADS];	// We now have means to set channel per pad
unsigned char maxSoundNumber = (unsigned char)15;

unsigned char GetMaxSoundNumber(void)
{
	return maxSoundNumber;
}

void SetSound(unsigned char pad, unsigned char sound)
{
	Sound[pad] = sound;
}

void SetChannel(unsigned char pad, unsigned char channel)
{
	Channel[pad] = channel;
}

unsigned char GetChannel(unsigned char pad)
{
	return Channel[pad];
}

void Test_hit(void)
{
	MIDI_KeyOn(DRUM_CHANNEL, VIRVELTRUMMA, 0x40);
	MIDI_KeyOn(DRUM_CHANNEL, BASTRUMMA, 0x40);
	MIDI_KeyOff(DRUM_CHANNEL, VIRVELTRUMMA, 0x00);
	MIDI_KeyOff(DRUM_CHANNEL, BASTRUMMA, 0x00);
}

void long_pause(void)
{
	unsigned char i;
	for(i = 0; i < (unsigned int)10; i++)
	{
		pause(0xFF);
	}
}

void main(void)
{
	MIDI_EVENT me;
	unsigned char pval = 0x80;
	unsigned char pad;
	unsigned char test;
	unsigned char MIDI_In_Byte[3];
	unsigned char ByteNumber;
    unsigned char inData;
	unsigned char inHHPedal;
	unsigned char hh_pedal_state;
	unsigned char kbrd;
	unsigned char incoming_serial_sound;
	int active_sound[NUMBER_OF_PADS];

	// Always reset work area of EEPROM to default sounds and channels.
	// Channels are 0x09 as default, but may be changed to 0x0E in patches.
	// To save EEPROM space, channel numbers are encoded in sound bytes MSb:
	// 0 = channel 10 (0x09) and 1 = channel 15 (0x0E).
	// Set sounds and channels in memory:
	Sound[0]  = BASTRUMMA;    // 00 - C1 MIDI in from bd-pedal                                                      
	Sound[1]  = VIRVELTRUMMA; // 01 - D1 Snare-drum, first input via RCA connector                         
	Sound[2]  = HIGHAT;       // 02 - F# Highat second input via RCA connector (hh-pedal down)             
	Sound[3]  = OPENHH;       // 03 - G# Highat second input via RCA connector (hh-pedal up)               
	Sound[4]  = PEDALHH;      // 04 - Sound when closing HH without hitting it                             
	Sound[5]  = CRSCYM;       // 06 - Cumbal Right                                                         
	Sound[6]  = RIDE;         // 05 - Cymbal Left                                                          
	Sound[7]  = COWBELL;      // 07 - Small devices rack on bd stand                                       
	Sound[8]  = CLAVES1;      // 08 - -"-                                                                  
	Sound[9]  = CLAVES2;      // 09 - -"-                                                                  
	Sound[10] = MPX8_6;       // 10 - -"-                                                                  
	Sound[11] = MPX8_7;       // 11 - -"-                                                                  
	Sound[12] = MPX8_8;       // 12 - -"-                                                                  
	Sound[13] = ROOM_T4;      // 13 - Tom rack                                                             
	Sound[14] = ROOM_T3;      // 15 - -"-                                                                  
	Sound[15] = ROOM_T2;      // 15 - -"-                                                                  
	Sound[16] = IMPACT_S;     // 16 - Extra pads #1                                                        
	Sound[17] = CAN1;         // 17 - -"-                                                                  
	Sound[18] = OUCH_S;       // 21 - -"-
	Sound[19] = CLAP808;      // 19 - -"-                                                                  
	Sound[20] = HCONG;        // 18 - -"-                                                                  
	Sound[21] = LCONG;        // 20 - -"-                                                                  

	// Transfer sounds and channels to EEPROM:
	for (pad = 0; pad < NUMBER_OF_PADS; pad++)
	{
		if(pad > (unsigned char)21)
		{
    		// Pads over 21 are not used in this controller, but rather set to HH:
			SetChannel(pad, 0x0e);
			eeprom_write_byte(pad, HIGHAT);
		}
		if (pad >= 10 && pad <= 12)
		{
			// Pads 10 - 12 on channel 9 for usage on Roland MPX8
			SetChannel(pad, 0x09);
			eeprom_write_byte(pad, Sound[pad]);
		}
		else
		{
			// All other pads on channel 14, Roland R-8M
			SetChannel(pad, 0x0e);
			eeprom_write_byte(pad, Sound[pad] | 0x80);
		}
	}

	// Set all active_sound to 0. This is for protection against hanging notes.
	// 0  -> sound is not active
	// > 0 -> sound is active
	// >= SOUND_DURATION -> sound must be silenced
	for (pad = 0; pad < NUMBER_OF_PADS; pad++)
	{
		active_sound[pad] = 0;
	}

	// Interrupts and timers are in use to increment timers that handles state-machine in DrumPad_drv:
	INTCON = 0b10100000;
	T0CON = 0b11010111;
#ifdef CLOCK_FREQUENCY_20_MHZ
	TMR0L = 237;
#endif
#ifdef CLOCK_FREQUENCY_48_MHZ
	TMR0L = 210;
#endif

	// Other initializations:
	pad = 0;
	ByteNumber = 0;
	MIDI_Init();
	MIDI_Reset_All();
	DrumPad_Setup();
	DrumPad_Reset();
	lcd_init();
	menu_init();

	Test_hit();

	while(1)
	{
		// We use RB5 (PGM) for hh-pedal.
		UCON = 0x00;
		TRISB |= 0x20;
//		inHHPedal = PORTB;
//		// Any change in inHHPdal?
//		if(hh_pedal_state != (inHHPedal & 0x20))
//		{
//			// Remember new state:
			hh_pedal_state = (inHHPedal & 0x20);
//	
//			// Was the pedal going down?
//			if(hh_pedal_state == (unsigned char)0x20)
//			{
//				// When pedal goes down, there is a sound from the closing high-hat:
//				MIDI_KeyOff(GetChannel(2), Sound[2], 0); // Sound[2] is closed HH.
//				MIDI_KeyOn(GetChannel(2), Sound[2], 0x7f);
//				active_sound[2] = 1;
//			}	
//		}
		// Change due to debounche problem:
		if (PORTB & 0x20)
		{
			// Pedal is down. Countdown debounce delay and play sound if zero:
			if (inHHPedal > 1)
			{
				inHHPedal--;
			}
			if (inHHPedal == 1)
			{
				MIDI_KeyOff(GetChannel(2), Sound[4], 0); // Sound[4] is closing HH.
				MIDI_KeyOn(GetChannel(2), Sound[4], 0x7f);
				active_sound[3] = 1;
				// Do not play again:
				inHHPedal = 0;
			}
		}
		else
		{
			// Pedal i up. Set debounce delay:
			inHHPedal = 100;
		}

		// At 20 MHz this loop takes 50 us, so we call DrumPad_Get(void) 20 times per ms.
		// At 48 MHz this loop takes 21 us, so we call DrumPad_Get(void) 48 times per ms.
		// Ask drumpad driver if any new pad-hits has been registered:
		if(DrumPad_Get(&me))
		{
			// Sound[0] = Base drum, not handeled by DrumPad_drv!
			// Sound[1] = Snare drum
			// Sound[2] = Closed highat
			// Sound[3] = Open highat NOTE! same pad as closed highat!
			// Sound[4] = Closing highat NOTE! same pad as closed highat!
			// Sound[5 - 12] = Cymbals etc...
			me.Note++; // BD has position 0!

			// Adjust two more if over HH since HH takes three sounds on the same pad:
			if (me.Note > (unsigned char)2)
			{
				me.Note += 2; 
			}

			// If HIGHAT and pedal open, set to OPENHH;
			if((me.Note == (unsigned char)2) && (hh_pedal_state == (unsigned char)0x20))
			{
				me.Note = 3; // Sound[2] is closed HH. Sound[3] is open HH.
			}
			
			// me.Note is actually the pad that was hit. Sound and channel if found in the
			// lists Cannel and Sound respectivly.
			// Send the note:
			MIDI_KeyOff(Channel[me.Note], Sound[me.Note], 0);
			MIDI_KeyOn(Channel[me.Note], Sound[me.Note], me.Velocity);
			active_sound[me.Note] = 1;
		}
		// See if there is any incoming serial data from the pedal or sounds 0 - 35:
		else if(PIR1 & 0x20)
		{
			if(RCSTA & 0x04)
			{
				// Framing error! Reset and restart MIDI message:
				RCSTA &= 0xef;
				RCSTA |= 0x10;
			}
			else if(RCSTA & 0x02)
			{
				// Overrun error! Reset and restart MIDI message:
				RCSTA &= 0xef;
				RCSTA |= 0x10;
			}
			inData = RCREG;
			if(inData == (unsigned int)0x99) // Note on, channel 10 = first byte of MIDI note on message (bdPedal always send on channel 10, and so must all serial devices).
			{
				ByteNumber = 1;
			}
			else if(ByteNumber == (unsigned int)1 && inData <= (unsigned int)0x24) // Fixes all sounds sent to the serial port.
			{
				incoming_serial_sound = inData;
				ByteNumber = 2;
			}
			else if(ByteNumber == (unsigned int)2) // If nothing else, this is the velocity byte.
			{
				if (incoming_serial_sound == (unsigned int)0x24)
				{
					// This is the base drum. Map to sound 0:
					incoming_serial_sound = (unsigned int)0x00;
				}
				else
				{
					// These are extra sounds, map above own sounds:
					incoming_serial_sound += (unsigned char)16;
					//incoming_serial_sound = 0x01;
					
					// Expand number of available sounds if this one was outside:
					if (incoming_serial_sound > maxSoundNumber)
					{
						maxSoundNumber = incoming_serial_sound;
					}
				}
				MIDI_KeyOff(GetChannel(incoming_serial_sound), Sound[incoming_serial_sound], 0x00);
				MIDI_KeyOn(GetChannel(incoming_serial_sound), Sound[incoming_serial_sound], inData); // (inData now contains velocity)
				active_sound[0] = 1;
				ByteNumber = 0;
			}
		}
		else
		{
			// Protection against hanging notes:
			if (active_sound[pad] > 0)
			{
				active_sound[pad]++;
			}	
			if (active_sound[pad] >= SOUND_DURATION)
			{
				MIDI_KeyOff(GetChannel(pad), Sound[pad], (unsigned char)0);
				//MIDI_KeyOff(4, Sound[pad], 0);
				active_sound[pad] = -1;				
			}
			pad++;
			if (pad > (unsigned char)15) pad = (unsigned char)0;
			//pad = (++pad + NUMBER_OF_PADS) % NUMBER_OF_PADS;
		
			// We cannot send a full event without catching pad and pedal inputs,
			// but we send one byte at a time, if available:
			MIDI_SendOneByte();
	
			// See if any key is pressed:
			kbrd = getc();
			if (kbrd != ' ')
			{
				menu(kbrd);
			}
		}
	}
}
