//==============================================================================
//
// Title:       lab2-1
// Purpose:     A short description of the application.
//
// Created on:  01.01.2008 at 0:08:29 by Gennady Kuzin.
// Copyright:   NSU. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include <tsani.h>
#include "lab2-1.h"
#include "toolbox.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

static int panelHandle;

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
    int error = 0;
	ni6251Slot(2);
	
	//portIn();
    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "lab2-1.uir", Lab21));
    
    /* display the panel and run the user interface */
    errChk (DisplayPanel (panelHandle));
    errChk (RunUserInterface ());
	ni6251Close();
Error:
    /* clean up */
    DiscardPanel (panelHandle);
    return 0;
}

//==============================================================================
// UI callback function prototypes

/// HIFN Exit when the user dismisses the panel.
int CVICALLBACK panelCB (int panel, int event, void *callbackData,
        int eventData1, int eventData2)
{
    if (event == EVENT_CLOSE)
        QuitUserInterface (0);
    return 0;
}

int  CVICALLBACK AO01Get(int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch(event)
	{
		case EVENT_COMMIT:
			double voltage, volt0, volt1;
			GetCtrlVal(Lab21, Lab21_AO01, &voltage);
			analogOut(0, voltage);
			analogIn(0, &volt0);
			analogOut(1, voltage);
			analogIn(1, &volt1);
			SetCtrlVal(Lab21, Lab21_AI0, volt0);
			SetCtrlVal(Lab21, Lab21_AI1, volt1);
			break;
		case EVENT_LEFT_CLICK:
			 break;
		case EVENT_RIGHT_CLICK:
			 break;
		case EVENT_GOT_FOCUS:
			 break;
		case EVENT_DISCARD:
			 break;
	} 
	return 0;
}

