#ifndef MAIN_H_DEFINED
#define MAIN_H_DEFINED
// Uncomment only the define below that corresponds to the clock in use.
//#define CLOCK_FREQUENCY_20_MHZ
#define CLOCK_FREQUENCY_48_MHZ
#define NUMBER_OF_PADS 22
void MIDI_KeyOn(unsigned char channel, unsigned char key_number, unsigned char velocity);
void MIDI_KeyOff(unsigned char channel, unsigned char key_number, unsigned char velocity);
void SetSound(unsigned char pad, unsigned char sound);
void SetChannel(unsigned char pad, unsigned char channel);
unsigned char GetMaxSoundNumber(void);
unsigned char GetChannel(unsigned char pad);

#endif
