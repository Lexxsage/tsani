#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include <tsani.h>
#include "213.h"
#include "toolbox.h"

static int panelHandle;
char read0;
char read1;
char read2;
int write;
int sequence;
int sequence_speed = 0;

void begin_sequence();
{
	sequence_speed = 1;
}

void reverse_sequence();
{
	sequence_speed = -1;
}

void stop_sequence();
{
	sequence_speed = 0;
}

void increase_sequence()
{
	if((sequence & 0x800) == 1)
	{
		sequence << 1;
		sequence += 0x001;
	}
	else
	{
		sequence << 1;
	}
}

void decrease_sequence()
{
	if((sequence & 0x001) == 1)
	{
		sequence >> 1;
		sequence += 0x800;
	}
	else
	{
		sequence >> 1;
	}
}

void invert_digit(int digit_num)
{
	int byte = 1;
	int current_byte;
	for(int i = 0; i < digit_num; i++)
	{
		byte << 1;
	}
	current_byte = byte & sequence;
	if(current_byte == 1)
	{
		sequence -= byte;
	}
	else
	{
		sequence += byte;
	}
}

void setLights(int number)
{
	char out0 = (number & 0xFF);
	char out1 = (number >> 8) & 0x0F;
	portOut(0, out0);
	portOut(1, out1);
}
int main (int argc, char *argv[])
{
    int error = 0;
    
    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "213.uir", PANEL));
	
	//11111111 = 0xFF
	//00001111 = 0x0F
    portMask(0, 0xFF);
	portMask(1, 0x0F);
	portMask(2, 0x07);
	portOut(2, 0x01);
	
		
	
    /* display the panel and run the user interface */
    errChk (DisplayPanel (panelHandle));
    errChk (RunUserInterface ());

Error:
    /* clean up */
    DiscardPanel (panelHandle);
    return 0;
}

int CVICALLBACK panelCB (int panel, int event, void *callbackData,
        int eventData1, int eventData2)
{
    if (event == EVENT_CLOSE)
        QuitUserInterface (0);
    return 0;
}

int CVICALLBACK callTimer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2) {
	switch (event) {
		case EVENT_TIMER_TICK:
			portIn(0, &read0);
			portIn(1, &read1);
			portIn(2, &read2);
			SetCtrlVal(panelHandle, PANEL_READ0, read0);
			SetCtrlVal(panelHandle, PANEL_READ1, read1);
			SetCtrlVal(panelHandle, PANEL_READ2, read2);
			sequence = ((read1 & 0x0F) << 8) + read0;
			if (((read1 >> 4) & 0x1) == 1)
			{
				sequence_speed = -1;
			}
			else if(((read1 >> 5) & 0x1) == 1)
			{
				sequence_speed = 0;
			}
			else if(((read1 >> 6) & 0x1) == 1)
				sequence_speed = 1;
			if((read1 >> 7) == 1)
			{
				invert_digit(7);
				setLights(sequence);
			}
			if(sequence_speed == 0)
			{
				GetCtrlVal(panelHandle, PANEL_NUMAD, &write);
				sequence = write;
				setLights(sequence);
			}
			else if(sequence_speed == 1)
			{
				increase_sequence();
				setLights(sequence);
			}
			else
			{
				decrease_sequence();
				setLights(sequence);
			}

			break;
		case EVENT_DISCARD:

			break;
	}
	return 0;
}
