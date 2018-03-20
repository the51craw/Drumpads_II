#ifndef DRUMPAD_H_DEFINED
#define DRUMPAD_H_DEFINED

// Driver for drumpads using A/D
#include "p18f4550.h"

#define START_CONVERSION ADCON0 |= 0x02
#define CONVERSION_DONE !(ADCON0 & 0x02)

struct midi_event      
{                      
	unsigned char Event;    // 0x80 = Key on, 0x90 = Key off
	unsigned char Channel;  // 0x00 - 0x0F, normally 0x09 (Channel 10)
	unsigned char Note;     // 0x00 - 0x7F, note number
	unsigned char Velocity; // 0x00 - 0x7f, on/off key velocity  
};

#define MIDI_EVENT struct midi_event

void DrumPad_Setup(void);
void DrumPad_Reset(void);
unsigned char DrumPad_Get(MIDI_EVENT * event); // returns 0xFF if new event available, else 0x00
void DrumPad_IncrementTimers(void);
#endif
