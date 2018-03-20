/*
This is the implementation file for the menu
*/
#include "menu.h"
#include "main.h"
#include "disp_JHD162A_drv.h"
#include "MIDI_drv.h"
#include "eeprom_low_level_driver.h"
#include "DrumPad_drv.h"

rom far char menu_text_main11[]   = "--- Drumpads ---";
rom far char menu_text_main12[]   = "* Help    # Menu";
rom far char menu_text_help11[]   = "* = Exit/prevous";
rom far char menu_text_help12[]   = "# = Enter/next  ";
rom far char menu_text_help21[]   = "Numbers = 0-9 or";
rom far char menu_text_help22[]   = "A = up, B = down";
rom far char menu_text_help31[]   = "Pad 0 = bd-pedal";
rom far char menu_text_help32[]   = "Pad 1 = snare-dr";
rom far char menu_text_help41[]   = "Pad 2 = high-hat";
rom far char menu_text_help42[]   = "Pad 3 = open-hh ";
rom far char menu_text_help51[]   = "Pad 4-11 = cym- ";
rom far char menu_text_help52[]   = "bals etc. board ";
rom far char menu_text_help61[]   = "12-14 toms board";
rom far char menu_text_help62[]   = "MIDI chan. 10/15";
rom far char menu_text_choose11[] = "A Sounds B MIDI ";
rom far char menu_text_choose12[] = "channels C Patch";
rom far char menu_text_sound11[]  = "Enter drum pad  ";
rom far char menu_text_sound12[]  = "number (0-51):  ";
rom far char menu_text_sound21[]  = "Enter sound num-";
rom far char menu_text_sound22[]  = "ber (0-127):    ";
rom far char menu_text_patch11[]  = "Patch nn (1-4)  ";
rom far char menu_text_patch12[]  = "C=Load #=Save   ";
rom far char menu_text_patch21[]  = "Save to patch nn";
rom far char menu_text_patch22[]  = "Sure? *=No #=Yes";
rom far char menu_text_patch32[]  = "Playing patch...";
rom far char menu_text_patch33[]  = "Copy nn to nn...";
rom far char menu_text_midi11[]   = "Select and enter";
rom far char menu_text_midi12[]   = "MIDI out ch: nn ";
rom far char menu_text_midi21[]   = "MIDI in ch: nn  ";

unsigned char number = 0;
unsigned char pad = 0;
unsigned char sound = 0;
unsigned char patch = 1;
unsigned char midi_in = 0x09;
unsigned char midi_out = 0x09;
unsigned char length = 0;
unsigned char line = 0;
unsigned char cpos = 0;
	
void save_patch(unsigned char patch);
void load_patch(unsigned char patch);
void save_sound_number(void);
void send_note(void);
unsigned char read_sound_number(void);
void a_to_d_menu(unsigned char key);
void play_patch(unsigned char patch);
void send_midi(void);
unsigned char read_midi_out(void);
void save_midi_out(void);

enum MENU_STATE
{
	MENU_STATE_MAIN,
	MENU_STATE_HELP_1,
	MENU_STATE_HELP_2,
	MENU_STATE_HELP_3,
	MENU_STATE_HELP_4,
	MENU_STATE_HELP_5,
	MENU_STATE_HELP_6,
	MENU_STATE_CHOOSE,
	MENU_STATE_PAD_NUMBER,
	MENU_STATE_SOUND_NUMBER,
	MENU_STATE_PATCH_NUMBER,
	MENU_STATE_PATCH_SAVE,
	MENU_STATE_MIDI_OUT,
}menu_state;

enum MENU_SUBSTATE
{
	MENU_SUBSTATE_SOUND,
	MENU_SUBSTATE_MIDI_OUT,
}menu_substate;

void menu_init(void)
{
	menu_state = MENU_STATE_MAIN;
	menu_substate = MENU_SUBSTATE_SOUND;
	PrintLine1(menu_text_main11);
	PrintLine2(menu_text_main12);
}

void menu(unsigned char key)
{
	unsigned char i;
	unsigned char temp_pad;
	unsigned char temp_midi_out;

	switch (menu_state)
	{
		case MENU_STATE_MAIN:
			switch (key)
			{
				case 'A':
					pad = (unsigned char)0;
					midi_out = read_midi_out();
					sound = read_sound_number();
					send_note();
					a_to_d_menu(key);
				break;
				case 'B':
					pad = (unsigned char)0;
					midi_out = read_midi_out();
					sound = read_sound_number();
					send_note();
					a_to_d_menu(key);
				break;
				case 'C':
					a_to_d_menu(key);
				break;
				case 'D':
					a_to_d_menu(key);
				break;
				case '*':
					menu_state = MENU_STATE_HELP_1;
					PrintLine1(menu_text_help11);
					PrintLine2(menu_text_help12);
				break;
				case '#':
					menu_state = MENU_STATE_CHOOSE;
					PrintLine1(menu_text_choose11);
					PrintLine2(menu_text_choose12);
				break;
				default: // Digits
				break;
			}
		break;
		case MENU_STATE_HELP_1:
			switch (key)
			{
				case 'A':
				case 'B':
				case 'C':
				case 'D': // Intentional fall-through
					a_to_d_menu(key);
				break;
				case '*':
					menu_state = MENU_STATE_MAIN;
					PrintLine1(menu_text_main11);
					PrintLine2(menu_text_main12);
				break;
				case '#':
					menu_state = MENU_STATE_HELP_2;
					PrintLine1(menu_text_help21);
					PrintLine2(menu_text_help22);
				break;
				default: // Digits
				break;
			}
		break;
		case MENU_STATE_HELP_2:
			switch (key)
			{
				case 'A':
				case 'B':
				case 'C':
				case 'D': // Intentional fall-through
					a_to_d_menu(key);
				break;
				case '*':
					menu_state = MENU_STATE_MAIN;
					PrintLine1(menu_text_main11);
					PrintLine2(menu_text_main12);
				break;
				case '#':
					menu_state = MENU_STATE_HELP_3;
					PrintLine1(menu_text_help31);
					PrintLine2(menu_text_help32);
				break;
				default: // Digits
				break;
			}
		break;
		case MENU_STATE_HELP_3:
			switch (key)
			{
				case 'A':
				case 'B':
				case 'C':
				case 'D': // Intentional fall-through
					a_to_d_menu(key);
				break;
				case '*':
					menu_state = MENU_STATE_MAIN;
					PrintLine1(menu_text_main11);
					PrintLine2(menu_text_main12);
				break;
				case '#':
					menu_state = MENU_STATE_HELP_4;
					PrintLine1(menu_text_help41);
					PrintLine2(menu_text_help42);
				break;
				default: // Digits
				break;
			}
		break;
		case MENU_STATE_HELP_4:
			switch (key)
			{
				case 'A':
				case 'B':
				case 'C':
				case 'D': // Intentional fall-through
					a_to_d_menu(key);
				break;
				case '*':
					menu_state = MENU_STATE_MAIN;
					PrintLine1(menu_text_main11);
					PrintLine2(menu_text_main12);
				break;
				case '#':
					menu_state = MENU_STATE_HELP_5;
					PrintLine1(menu_text_help51);
					PrintLine2(menu_text_help52);
				break;
				default: // Digits
				break;
			}
		break;
		case MENU_STATE_HELP_5:
			switch (key)
			{
				case 'A':
				case 'B':
				case 'C':
				case 'D': // Intentional fall-through
					a_to_d_menu(key);
				break;
				case '*':
					menu_state = MENU_STATE_MAIN;
					PrintLine1(menu_text_main11);
					PrintLine2(menu_text_main12);
				break;
				case '#':
					menu_state = MENU_STATE_HELP_6;
					PrintLine1(menu_text_help61);
					PrintLine2(menu_text_help62);
				break;
				default: // Digits
				break;
			}
		break;
		case MENU_STATE_HELP_6:
			switch (key)
			{
				case 'A':
				case 'B':
				case 'C':
				case 'D': // Intentional fall-through
					a_to_d_menu(key);
				break;
				case '*':
				case '#': // Intentional fall-through
					menu_state = MENU_STATE_MAIN;
					PrintLine1(menu_text_main11);
					PrintLine2(menu_text_main12);
				break;
				default: // Digits
				break;
			}
		break;
		case MENU_STATE_CHOOSE:
			switch (key)
			{
				case 'A':
				case 'B':
				case 'C':
				case 'D': // Intentional fall-through
					a_to_d_menu(key);
				break;
				case '*':
				case '#': // Intentional fall-through
					menu_state = MENU_STATE_MAIN;
					PrintLine1(menu_text_main11);
					PrintLine2(menu_text_main12);
				break;
				default: // Digits
				break;
			}
		break;
		case MENU_STATE_PAD_NUMBER:
			switch (key)
			{
				case 'A':
					if (number < GetMaxSoundNumber())
					{
						number++;
						PrintNumeric(number, length, line, cpos);
						pad = number;
						midi_out = read_midi_out();
						sound = read_sound_number();
						send_note();
					}
				break;
				case 'B':
					if (number > (unsigned char)0)
					{
						number--;
						PrintNumeric(number, length, line, cpos);
						pad = number;
						midi_out = read_midi_out();
						sound = read_sound_number();
						send_note();
					}
				break;
				case 'C':
				break;
				case 'D':
					if (number <= GetMaxSoundNumber())
					{
						pad = number;
						midi_out = read_midi_out();
						sound = read_sound_number();
						send_note();
					}
				break;
				case '*':
					if (number < (unsigned char)16)
					{
						pad = number;
					}
					menu_state = MENU_STATE_MAIN;
					PrintLine1(menu_text_main11);
					PrintLine2(menu_text_main12);
				break;
				case '#':
					switch (menu_substate)
					{
						case MENU_SUBSTATE_SOUND:
							if (number <= GetMaxSoundNumber())
							{
								length = (unsigned char)3;
								line = (unsigned char)2;
								cpos = (unsigned char)12;
								pad = number;
								midi_out = read_midi_out();
								sound = read_sound_number();
								send_note();
								number = sound;
								menu_state = MENU_STATE_SOUND_NUMBER;
								PrintLine1(menu_text_sound21);
								PrintLine2(menu_text_sound22);
								PrintNumeric(number, length, line, cpos);
							}
						break;
						case MENU_SUBSTATE_MIDI_OUT:
							if (number <= GetMaxSoundNumber())
							{
								length = (unsigned char)2;
								line = (unsigned char)2;
								cpos = (unsigned char)13;
								pad = number;
								midi_out = read_midi_out();
								sound = read_sound_number();
								send_note();
								number = midi_out;
								menu_state = MENU_STATE_MIDI_OUT;
								PrintLine1(menu_text_midi11);
								PrintLine2(menu_text_midi12);
								PrintNumeric(number, length, line, cpos);
							}
						break;
					}
				break;
				default: // Digits
					number = (number * (unsigned char)10) % (unsigned char)100;
					number += key - 0x30;
					while (number > (unsigned char)16)
					{
						number -= (unsigned char)10;
					}
					PrintNumeric(number, length, line, cpos);
				break;
			}
		break;
		case MENU_STATE_SOUND_NUMBER:
			switch (key)
			{
				case 'A':
					if (number < (unsigned char)127)
					{
						number++;
						PrintNumeric(number, length, line, cpos);
						sound = number;
						midi_out = read_midi_out();
						send_note();
					}
				break;
				case 'B':
					if (number > (unsigned char)0)
					{
						number--;
						PrintNumeric(number, length, line, cpos);
						sound = number;
						midi_out = read_midi_out();
						send_note();
					}
				break;
				case 'C':
				break;
				case 'D':
					if (number < (unsigned char)128)
					{
						sound = number;
						midi_out = read_midi_out();
						send_note();
					}
				break;
				case '*':
					menu_state = MENU_STATE_MAIN;
					PrintLine1(menu_text_main11);
					PrintLine2(menu_text_main12);
				break;
				case '#':
					if (number < (unsigned char)128)
					{
						sound = number;
						save_sound_number();
						SetSound(pad, sound);
						menu_state = MENU_STATE_MAIN;
						PrintLine1(menu_text_main11);
						PrintLine2(menu_text_main12);
					}
				break;
				default: // Digits
					number = (number * (unsigned char)10) % (unsigned char)100;
					number += key - 0x30;
					while (number > (unsigned char)127)
					{
						number -= (unsigned char)100;
					}
					PrintNumeric(number, length, line, cpos);
				break;
			}
		break;
		case MENU_STATE_PATCH_NUMBER:
			switch (key)
			{
				case 'A':
					if (number < (unsigned char)4)
					{
						number++;
						PrintNumeric(number, length, line, cpos);
						temp_pad = pad;
						temp_midi_out = midi_out;
						play_patch(number);
						pad = temp_pad;
						midi_out = temp_midi_out;
					}
				break;
				case 'B':
					if (number > (unsigned char)1)
					{
						number--;
						PrintNumeric(number, length, line, cpos);
						temp_pad = pad;
						temp_midi_out = midi_out;
						play_patch(number);
						pad = temp_pad;
						midi_out = temp_midi_out;
					}
				break;
				case 'C':
					// Page zero is the sandbox we play in.
					// User sees pages 1 to 7.
					// When selecting a patch we copy from that page
					// to page zero and updates sounds in RAM.
					// We also copy channel numbers at a location 0x80 higher up.
					temp_pad = pad;
					temp_midi_out = midi_out;
					patch = number;
					load_patch(patch);
					pad = temp_pad;
					midi_out = temp_midi_out;
					menu_state = MENU_STATE_MAIN;
					PrintLine1(menu_text_main11);
					PrintLine2(menu_text_main12);
				break;
				case 'D':
						temp_pad = pad;
						temp_midi_out = midi_out;
						play_patch(number);
						pad = temp_pad;
						midi_out = temp_midi_out;
				break;
				case '*':
					menu_state = MENU_STATE_MAIN;
					PrintLine1(menu_text_main11);
					PrintLine2(menu_text_main12);
				break;
				case '#':
					length = (unsigned char)2;
					line = (unsigned char)1;
					cpos = (unsigned char)14;
					menu_state = MENU_STATE_PATCH_SAVE;
					PrintLine1(menu_text_patch21);
					PrintLine2(menu_text_patch22);
					PrintNumeric(number, length, line, cpos);
				break;
				default: // Digits
					number = (number * (unsigned char)10) % (unsigned char)100;
					number += key - (unsigned char)0x30;
					while (number > (unsigned char)16)
					{
						number -= (unsigned char)10;
					}
					PrintNumeric(number, length, line, cpos);
				break;
			}
		break;
		case MENU_STATE_PATCH_SAVE:
			switch (key)
			{
				case 'A':
				break;
				case 'B':
				break;
				case 'C':
				break;
				case 'D':
				break;
				case '*':
					menu_state = MENU_STATE_MAIN;
					PrintLine1(menu_text_main11);
					PrintLine2(menu_text_main12);
				break;
				case '#':
					// Page zero is the sandbox we play in.
					// User sees pages 1 to 15.
					// When selecting a patch we copy from that page
					// to page zero and updates sounds in RAM.
					temp_pad = pad;
					temp_midi_out = midi_out;
					save_patch(number);
					pad = temp_pad;
					midi_out = temp_midi_out;
					menu_state = MENU_STATE_MAIN;
					PrintLine1(menu_text_main11);
					PrintLine2(menu_text_main12);
				break;
				default: // Digits
				break;
			}
		break;
		case MENU_STATE_MIDI_OUT:
			switch (key)
			{
				case 'A':
					number = 14;
					PrintNumeric(number, length, line, cpos);
					midi_out = number;
					sound = read_sound_number();
					send_note();
				break;
				case 'B':
					number = 9;
					PrintNumeric(number, length, line, cpos);
					midi_out = number;
					sound = read_sound_number();
					send_note();
				break;
				case 'C':
				break;
				case 'D':
					send_note();
				break;
				case '#':
					save_midi_out();
					SetChannel(pad, midi_out);
				case '*': // Intentional fall-through:
					menu_state = MENU_STATE_MAIN;
					PrintLine1(menu_text_main11);
					PrintLine2(menu_text_main12);
				break;
				break;
				default: // Digits
				break;
			}
		break;
	}
}

void a_to_d_menu(unsigned char key)
{
	switch (key)
	{
		case 'A':
			number = pad;
			length = (unsigned char)2;
			line = (unsigned char)2;
			cpos = (unsigned char)14;
			menu_state = MENU_STATE_PAD_NUMBER;
			menu_substate = MENU_SUBSTATE_SOUND;
			PrintLine1(menu_text_sound11);
			PrintLine2(menu_text_sound12);
			PrintNumeric(number, length, line, cpos);
		break;
		case 'B':
			number = pad;
			length = (unsigned char)2;
			line = (unsigned char)2;
			cpos = (unsigned char)13;
			menu_state = MENU_STATE_PAD_NUMBER;
			menu_substate = MENU_SUBSTATE_MIDI_OUT;
			PrintLine1(menu_text_sound11);
			PrintLine2(menu_text_sound12);
			PrintNumeric(number, length, line, cpos);
		break;
		case 'C':
			number = patch;
			length = (unsigned char)2;
			line = (unsigned char)1;
			cpos = (unsigned char)6;
			menu_state = MENU_STATE_PATCH_NUMBER;
			PrintLine1(menu_text_patch11);
			PrintLine2(menu_text_patch12);
			PrintNumeric(number, length, line, cpos);
			play_patch(number);
		break;
		case 'D':
			if (sound < (unsigned char)128)
			{
				send_note();
			}
		break;
	}	
}

// We always work with patch 0.
// When saving/loading patch we copy from/to patch 0
// to/from selected patch. This way we can do changes
// a patch without actually having to decide to save
// the changes. 
void save_patch(unsigned char patch)
{
	// Read sounds from EEPROM page 0 and store i EEPROM page patch:
	unsigned char i;
	unsigned char snd;
	unsigned char sndAddr;
	for (i = (unsigned char)0; i < (unsigned char)51; i++)
	{
		sndAddr = i;
		snd = eeprom_read_byte(sndAddr);
		sndAddr = i + (patch * (unsigned char)51);
		eeprom_write_byte(sndAddr, snd);
	}
}

void load_patch(unsigned char patch)
{
	// Read sounds from EEPROM page patch and store i EEPROM page 0
	// while updating the sound list:
	unsigned char i;
	unsigned char snd;
	unsigned char sndAddr;
	PrintLine1(menu_text_patch33);
	for (i = (unsigned char)0; i < (unsigned char)51; i++)
	{
		sndAddr = i + (patch * (unsigned char)51);
		snd = eeprom_read_byte(sndAddr);
		sndAddr = i;
		eeprom_write_byte(sndAddr, snd);
		if (i < (unsigned char)NUMBER_OF_PADS)
		{
    		if (snd & 0x80)
    		{
    			SetChannel(i, 0x0e);
    		}
    		else
    		{
    			SetChannel(i, 0x09);
    		}
    		SetSound(i, snd & 0x7f);
		}
	}
}

// We have a maximum of 4 patches with 51 sounds + the work area, also with 51 souds.
void play_patch(unsigned char patch)
{
	unsigned char i;
	PrintLine2(menu_text_patch32);
	sound = 0x00;
	for (i = (unsigned char)0; i <= GetMaxSoundNumber(); i++)
	{
		pad = (patch * (unsigned char)51) + i;
		midi_out = read_midi_out();
		sound = read_sound_number();
		send_note();
	}
	PrintLine2(menu_text_patch12);
}

unsigned char read_sound_number(void)
{
	return eeprom_read_byte(pad) & 0x7f;
}

void save_sound_number(void)
{
	if (midi_out == (unsigned char)0x0e)
	{
		sound |= 0x80;
	}
	eeprom_write_byte(pad, sound);
}

// Read midi out channel from EEPROM (stored in MSb of sound byte):
unsigned char read_midi_out(void)
{
	unsigned char byte = eeprom_read_byte(pad) & 0x80;
	if (byte == 0x80)
	{
		return 0x0e;
	}
	else
	{
		return 0x09;
	}
}

// Store midi out channel in EEPROM (in MSb of sound byte):
void save_midi_out(void)
{
	unsigned char temp = eeprom_read_byte(pad) & 0x7f;
	if (midi_out == (unsigned char)0x0e)
	{
		temp |= 0x80;
	}
	else
	{
		temp &= 0x7f;
	}
	eeprom_write_byte(pad, temp);
}

void send_note(void)
{
	MIDI_KeyOn(midi_out, sound, 0x40);
	send_midi();
	pause((unsigned char)255);
	MIDI_KeyOff(midi_out, sound, 0x00);
	send_midi();
}

void send_midi(void)
{
	unsigned char i;
	for (i = (unsigned char)0; i < (unsigned char)3; i++)
	{
		MIDI_SendOneByte();
		pause((unsigned char)25);
	}
}