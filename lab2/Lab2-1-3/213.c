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
int asequence = 0;
int if_inv = 0;


void convert()
{
	int temp = 0x200 & sequence;
	if(temp == 1)
	{
		asequence = sequence - 0x200;
	}
	else
	{
		asequence = sequence + 0x200;
	}
}

void begin_sequence()
{
	sequence_speed = 1;
}

void reverse_sequence()
{
	sequence_speed = -1;
}

void stop_sequence()
{
	sequence_speed = 0;
}

void increase_sequence()
{
	if((sequence & 0x800) == 0x800)
	{
		sequence = ((sequence * 2) & 0xFFF) + 0x001;
	}
	else
	{
		sequence = sequence * 2;
	}
}

void decrease_sequence()
{
	if((sequence & 0x001) == 1)
	{
		sequence = (sequence / 2) + 0x800;
	}
	else
	{
		sequence = sequence / 2;
	}
}

void setLights()
{
	char out0;
	char out1;
	if(if_inv == 0)
	{
		out0 = (sequence & 0xFF);
		out1 = (sequence >> 8) & 0x0F;
	}
	else
	{
		out0 = (asequence & 0xFF);
		out1 = (asequence >> 8) & 0x0F;
	}
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
			if (((read1 >> 4) & 0x1) == 0)
			{
				sequence_speed = 1;
			}
			else if(((read1 >> 5) & 0x1) == 0)
			{
				sequence_speed = 0;
			}
			else if(((read1 >> 6) & 0x1) == 0)
				sequence_speed = -1;
			if((read1 >> 7) == 0)
			{
				if(if_inv == 1)
				{
					if_inv = 0;
				}
				else
				{
					if_inv = 1;
				}
			}
			if(sequence_speed == 0)
			{
				GetCtrlVal(panelHandle, PANEL_NUMAD, &write);
				sequence = write;
				convert();
				setLights();
			}
			
			break;
		case EVENT_DISCARD:

			break;
	}
	return 0;
}

int CVICALLBACK callLTimer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2) {
	switch (event) {
		case EVENT_TIMER_TICK:
			if(sequence_speed == 1)
			{
				increase_sequence();
				convert();
				setLights();
				SetCtrlVal(panelHandle, PANEL_NUMAD, sequence);
			}
			else if(sequence_speed == -1)
			{
				decrease_sequence();
				convert();
				setLights();
				SetCtrlVal(panelHandle, PANEL_NUMAD, sequence);
			}
			
			break;
		case EVENT_DISCARD:

			break;
	}
	return 0;
}
