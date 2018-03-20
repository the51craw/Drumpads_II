#include "main.h"
#include "DrumPad_drv.h"

unsigned char ADC_channel_select = 0; // Start at channel 0;
unsigned char Overhearing_protection = 0; // Count down from OVER_HEARING_TIMEOUT to 0 while blocking SOFT_BEAT hits

#pragma udata Buffer
unsigned char Buffer[128];  // Stores up to 128 bytes = 32 MIDI events
unsigned char StorePos = 0; // Position to store next 4 event bytes, wraps around
unsigned char ReadPos  = 0; // Position to read, no data to fetch if same as StorePos

unsigned char LastHit[13];
unsigned char TopValue[13];
unsigned char TimeStamp[13];
unsigned char State[13];

void DrumPad_drv_check_ad(void);
void StartConversion(void);
unsigned char IsOverhearing(void);

#define USED_AD_CHANNELS (unsigned int)13 

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// Exported funktions
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// DrumPad_Setup
////////////////////////////////////////////////////////////////

void DrumPad_Setup(void)
{
	unsigned char i;

	// Set AD control registers:
	TRISA = 0xBF;	// Set all as input except RA6. All are analog except RA4 that is HH-pedal input, and RA6 that is output only.
	TRISB = 0xFF;	// Set RB0-4 as inputs (AN12, 10, 8, 9, 11) RB4-7 are used as flashing pins
	TRISE = 0xFF;	// Set RE0-2 as inputs (AN5 - AN7)

	// We should always have a conversion going on. Then we know that CONVERSION_DONE is valid.
	StartConversion();
}

void DrumPad_IncrementTimers(void)
{
	unsigned char i;

	for(i = 0; i < USED_AD_CHANNELS; i++)
	{
		if (TimeStamp[i] < (unsigned char)0xff)
		{
			TimeStamp[i]++;
		}
	}
	if(Overhearing_protection > (unsigned int)0)
	{
		Overhearing_protection--;
	}
}
	
////////////////////////////////////////////////////////////////
// DrumPad_Get
// Returns 0xFF if a new event is filled out in event
// Returns 0x00 if not, but first checks A/D conversions
////////////////////////////////////////////////////////////////

unsigned char DrumPad_Get(MIDI_EVENT * event)
{
	DrumPad_drv_check_ad();

	// If we have data in in-buffer, get it:
	if(ReadPos != StorePos)
	{
		event->Event    = Buffer[ReadPos++];
		ReadPos &= 0x7F;
		event->Channel  = Buffer[ReadPos++];
		ReadPos &= 0x7F;
		event->Note     = Buffer[ReadPos++];
		ReadPos &= 0x7F;
		event->Velocity = Buffer[ReadPos++];
		ReadPos &= 0x7F;
		return 0xFF;
	}

	return 0x00;
}

void DrumPad_Reset(void)
{
	unsigned char i;

	for(i = 0; i < USED_AD_CHANNELS; i++)
	{
		TopValue[i] = 0;			// Reset top value list
		TimeStamp[i] = 0;			// Reset timestamps:
		State[i] = 0;				// Reset states:
		LastHit[i] = 0;
	}

	// Reset buffer positions and channel #:
	StorePos = 0;
	ReadPos  = 0;
	ADC_channel_select = 0;
}	

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// Internals
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

/*
REGISTER 21-3: ADCON2: A/D CONTROL REGISTER 2
ADFM (ZERO) ACQT2 ACQT1 ACQT0 ADCS2 ADCS1 ADCS0
ADFM = 1 for right adjuted result.
ACQ0-2 = Aquisition time. 0, 2, 4, 6, 8, 12, 16, 20 Tad
ADCS0-2 = Conversion clock. 000 = F0sc/2, 001 = Fosc/4 etc 110 = Fosc/64 
*/

void StartConversion(void)
{
	ADCON1 = 0x02;                          // No VRef:s, use all channels
	ADCON0 = (ADC_channel_select << 0x02);  // Set channel select bits

	// Left justify (0xxx xxxx)
	// Acquisition Time = 4 (xx01 0xxx)
	// Clock Select = FOSC/16 (xxxx x101) = 1,25 MHz -> Tad = 0.8 us
	// AD conversion time = (succ. approx 10 bits = max 4 steps)
	ADCON2 = 0x95;

	ADCON0 |= 0x01;                         // Enable A/D
	START_CONVERSION;                       // Start conversion
}

// Here we store 4 bytes for an event. The two first will
// be combined into the first byte in a normal 3-byte
// midi event when played from the buffer later.
// StorePos &= 0x7F; instructions are for wrapping
// the buffer around.
void store(unsigned char velocity)
{
	// Event:
	if(velocity > (unsigned char)0)
	{
		// Store a note-on:
		Buffer[StorePos++] = 0x80;
	}
	else
	{
		// Store a note-off:
		Buffer[StorePos++] = 0x90;
	}
	StorePos &= 0x7F;
	// Channel:
	Buffer[StorePos++] = 0x09; // We now handle channels in main.
	StorePos &= 0x7F;
	// Key (pad, actually):
	Buffer[StorePos++] = ADC_channel_select; 
	StorePos &= 0x7F;
	// Velocity:
	Buffer[StorePos++] = velocity;
	StorePos &= 0x7F;
}

// Here we switch to next A/D channel.
void next(void)
{
	ADC_channel_select++;
	if(ADC_channel_select >= USED_AD_CHANNELS)
	{
		ADC_channel_select = 0;
	}
}

// States 
#define IDLE (unsigned char)0                 		// This is before level starts rising.
#define DETECTING_HIT (unsigned char)1        		// This is while detecting highest valid peak.
#define WAIT_WHILE_BOUNCHING (unsigned char)2 		// This is for not registering oscillations in drum-skin.
#define RAMPING_DOWN (unsigned char)3       		// This is while ramping down and accepting new hits compared to last hit.

// Over-hearing protection
#define OVERHEARING_EARLY (unsigned char) 1			// Time another hit causes an overhearing might be AFTER the detected hit
#define OVERHEARING_LATE (unsigned char) 2			// Time another hit causes an overhearing might be BEFORE the detected hit
#define OVERHEARING_DIFFERENCE (unsigned char) 20	// Difference between current peak (under) and any other 
													// channel's current peak that might be overhearing.
// Times and levels
// A hit has like 5 - 10 peaks during 10 - 20 ms + more lower and lower peaks following (debounce).
// The highest of the initial peaks marks the velocity of the hit, detected during detection time.
#define THRESHOLD (unsigned char) 20				// Lowest A/D value to start detecting a hit
#define NEW_HIT_DIFFERENCE (unsigned char) 10		// Smallest difference between last debounce peak and a new peak to detect on the same channel (not overhearing)
#define DETECTION_TIME (unsigned char) 10			// Time to detect initial peaks
#define DEBOUNCE_TIME (unsigned char) 10				// Time we do not accept new hits whatsoever.
#define RAMPING_DOWN_TIME (unsigned char) 50		// Listening for new peaks that are not more than NEW_HIT_DIFFERENCE under last hit.
											 		// This time repeats until last peak is under THRESHOLD.
void DrumPad_drv_check_ad(void)
{
	unsigned char new_AD_value;
	if(CONVERSION_DONE)
	{
		if(ADRESH > (unsigned int)0)
			new_AD_value = 255;
		else
			new_AD_value = ADRESL;

		switch(State[ADC_channel_select])
		{
			// Stay in idle while new_AD_value is not above THRESHOLD.
			// TopValue[ADC_channel_select] is the currently highest A/D value,
			// and if we go with it as a hit, it is remembered in order to compare
			// with later peaks and decide what is just drumskin vibrationw
			// and which are new hits, e.g. in a drum roll.
			case IDLE:
				if(new_AD_value > THRESHOLD) // Input noise elimination
				{
					// Save the A/D value:
					TimeStamp[ADC_channel_select] = 0;
					TopValue[ADC_channel_select] = new_AD_value;
					State[ADC_channel_select] = DETECTING_HIT;
				}
			break;
			// Sample and save highest value for DETECTION_TIME ms:
			case DETECTING_HIT:
				if(new_AD_value > TopValue[ADC_channel_select])
				{
					// A/D value is higher than before, save it:
					TopValue[ADC_channel_select] = new_AD_value;
				}
				// We stay here only for DETECTION_TIME milliseconds, then we
				// take the highest value as the hit value, unless it is over-
				// hearing from another drum:
				if(TimeStamp[ADC_channel_select] > DETECTION_TIME)
				{
					// Save to MIDI buffer if not overhearing:
					//if(!IsOverhearing(void))
					{
						// We have a hit! Store a note-off and a note-on with the TopValue:
						store(0);
						// Snare-drum and highat have more sensitive mocriphones, divide by 2:
						if (ADC_channel_select < 2)
						{
							TopValue[ADC_channel_select] /= 1.5;
						}
							
						if (TopValue[ADC_channel_select] > (unsigned char)0x7f)
						{
							store(127);
						}
						else
						{
							store(TopValue[ADC_channel_select]);
						}
					}	
					// Prepare for ramping down while detecting new hits.
					// Last pe
					LastHit[ADC_channel_select] = TopValue[ADC_channel_select];
					TopValue[ADC_channel_select] = 0;
					State[ADC_channel_select] = WAIT_WHILE_BOUNCHING;
					TimeStamp[ADC_channel_select] = 0;
				}
			break;
			// WAIT_WHILE_BOUNCHING is when pad is reverbing, and no hits are to be recognized.
			// This state also is for disallowing unnaturally fast drum rolls.
			case WAIT_WHILE_BOUNCHING:
				if(TimeStamp[ADC_channel_select] > DEBOUNCE_TIME)
				{
					State[ADC_channel_select] = RAMPING_DOWN;
					TimeStamp[ADC_channel_select] = 0;
				}
				break;
			// Just wait and detect new hits while pad is reverbing. The trick here is to not
			// detect the pad reverbrations, but new hits in a drum roll. This is accomplished
			// by comparing the A/D value saved in LastHit[ADC_channel_select].
			// We now re-use TopValue[ADC_channel_select] for detecting drum-roll hits.
			case RAMPING_DOWN:
				// Ramping down timeout?
				if(TimeStamp[ADC_channel_select] > RAMPING_DOWN_TIME)
				{
					// Ramp is down:
					TopValue[ADC_channel_select] = 0;
					State[ADC_channel_select] = IDLE;
				}
				else
				{
					// Stay in ramping down while peaks are lower than NEW_HIT_DIFFERENCE under last hit:
					// Here we might be ramping upto a top.
					if(new_AD_value > TopValue[ADC_channel_select])
					{
						TopValue[ADC_channel_select] = new_AD_value;
					}
					else
					{
						// So, maybe ramping down from a peak.
						// Was it larger than last one, with a bit margin, that would indicates a new hit.
						if (TopValue[ADC_channel_select] > LastHit[ADC_channel_select] - NEW_HIT_DIFFERENCE)
						{
							// Ok, seems like a real new hit, let's go back to DETECTING_HIT:
							TimeStamp[ADC_channel_select] = 0;
							State[ADC_channel_select] = DETECTING_HIT;
						}
					}
				}
			break;
		}	

		// Move to next channel and start a new A/D conversion:
		next();
		StartConversion();
	}
}

unsigned char IsOverhearing(void)
{
	// We try to decide whether this is an overhearing or not
	// by examining the states, timing (and top-level NO!) of the
	// other drumpads.
	// If another sound has started within 5 - 20 milliseconds before
	// is probably overhearing. Note that overhearing due to vibration
	// in the base-plate makes first peak go negative, not positive as
	// is the case for the signal coming from the pad actually hit.
	// Filtering like this will unfortunately also remove one of two
	// simultaneous hits, so keep limits well trimmed.
	unsigned char i;

	for (i = 0; i < USED_AD_CHANNELS; i++)
	{
		if (i != ADC_channel_select)
		{
			// Check for OVERHEARING_LATE milliseconds left in state DETECTING_HIT
			// or max OVERHEARING_EARLY milliseconds into WAIT_WHILE_BOUNCHING,
			// which we just entered:
			if
			(
				(
					(
						// If found other pad detection in end of state RAMPING_DOWN
						(State[i] == RAMPING_DOWN)
						&&
						(TimeStamp[i] < (OVERHEARING_LATE))
					)
					||
					(
						// ...or in beginning of state DETECTING_HIT
						(State[i] == DETECTING_HIT)
						&&
						(TimeStamp[i] > OVERHEARING_EARLY)
					)
				)
				// ...and current hit is enough lower in volume than the found hit:
				&&
				TopValue[ADC_channel_select] < (TopValue[i] + OVERHEARING_DIFFERENCE)
				//TopValue[i] >= (TopValue[ADC_channel_select] - OVERHEARING_DIFFERENCE)
			)
			{
				// ...then it is an overhearing from another real hit:
				return 0xff;
			}
		}
	}
	// Indicate that this is a real hit:
	return 0x00;
}