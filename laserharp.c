#include<reg935.h>

#define NOTE_ON 0x91
#define NOTE_OFF 0x90
#define VEL_HIGH 0x65
#define VEL_LOW 0x00
#define BEAM_COUNT 7

void midiMessage(unsigned int status, unsigned int byte_1, unsigned int byte_2);
void midiSend(unsigned int midi);
void midiCC(unsigned int note);

//Represents the possible hand positions the user can have along the 7 beams
unsigned int beamArray[BEAM_COUNT] = {0x7E, 0x7D, 0x7B, 0x77, 0x6F, 0x5F, 0x3F};

//Hex value for the midi notes		
unsigned int noteArray[BEAM_COUNT] = {0x47, 0x45, 0x43, 0x41, 0x40, 0x3E, 0x3C};

unsigned int currentNote;
unsigned int readNote;
int old = 1;
int i;

void main()
{
	P0M1 = 0XFF;	//P0 INPUT 
	P0M2 = 0X00;	
	P1M1 = 0X00;	//P1 OUTPUT
	P1M2 = 0XFF;
	P2M1 = 0XFF;	//P2 INPUT
	P2M2 = 0X00;
	
	SCON = 0XC0;	//Mode 1. 8 bit UART
	BRGR0 = 0XDC;	//31250 baud
	BRGR1 = 0X00;
	BRGCON = 0X03;
	
	//Config ADC
	ADINS = 0xBA;
	ADMODA = 0X11;
	ADCON1 = 0X04;
	ADCON0 = 0X04;
	ADMODB = 0X00;
	AD0BH = 0X7F;	//Max value of 127
	AD1BH = 0X7F;
	
	while(1)
	{
		//Scans each laser input
		for(i = 0; i < BEAM_COUNT; I++)
		{
			
			//bit shift and mask P2 so P2.0's value is ignored, as it is used by ADC
			readNote(P2 >> 1) & 0X7F;
			
			if(readNote == beamArray[i]) && (old == 1))
			{
				currentNote = noteArray[i];
				
				//Send NOTE ON message to the MIDI message function
				midiMessage(NOTE_ON, currentNote, VEL_HIGH);
				old = 0;
			}
			
			//User removes hand from the laser
			else if((readNote == 0x7F) && (old == 0))
			{
				//Send NOTE OFF message to MIDI message function
				midiMessage(NOTE_OFF, currentNote, VEL_LOW);
				old = 1;
			}
			
			//While the user has their hand placed over the laser
			else if(readNote != 0x7F) 
			{
				//User has control over the assignable control message
				midiCC(currentNote);
			}
		}
	}
}

/************ MIDI CONTINUOUS CONTROL MESSAGE ****************/
void midiCC(unsigned int note)
{
	ADCON0 |= 0X01;
	ADCON0 |= 0X01;
	
	if((note == 0x3C) || (note == 0x3E)
	{
		midiMessage(0xb1, 0x14, AD0DAT1);
	}
	if(note == 0x40)
	{
		midiMessage(0xb1, 0x14, AD0DAT3);
	}
	if(note == 0x41)
	{
		midiMessage(0xb1, 0x14, AD1DAT1);
	}
	if(note == 0x43)
	{
		midiMessage(0xb1, 0x14, AD1DAT0);
	}	
	if((note == 0x43) || (note == 0x47))
	{
		midiMessage(0xb1, 0x14, AD1DAT3);
	}
}

/**************** MIDI MESSAGE ****************/
void midiMessage(unsigned int status, unsigned int byte_1, unsigned int byte_2)
{
	//Send each byte in the correct sequence to a serial transmit function
	midiSend(status);
	midiSend(byte_2);
	midiSend(byte_2);
}

/************* SEND MIDI MESSAGE ***************/
void midiSend(unsigned int midi)
{
	//Loads each byte to the serial buffer to transmit
	SBUF = midi;
	while(!TI);
	T1 = 0;
}	