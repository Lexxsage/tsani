
#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include <tsani.h>
#include "lab.2.1.2.v1.h"
#include "toolbox.h"

static int panelHandle;
double in, hor, ver;

int main (int argc, char *argv[])
{
    int error = 0;
    
    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "lab.2.1.2.v1.uir", PANEL));
    
    /* display the panel and run the user interface */
    errChk (DisplayPanel (panelHandle));
    errChk (RunUserInterface ());

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

int CVICALLBACK timer (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2) {
	switch (event) {
		case EVENT_TIMER_TICK:
			{
				analogIn(2,&in);
				SetCtrlVal(panelHandle,PANEL_AI2,in);
				hor = -(in-2.5)/2.5;
				SetCtrlVal(panelHandle,PANEL_HOR,hor);
				analogIn(3,&in);
				SetCtrlVal(panelHandle,PANEL_AI3,in);
				ver = (in-2.3)/2.5;
				SetCtrlVal(panelHandle,PANEL_VER,ver);
				PlotPoint(panelHandle,PANEL_GRAPH,hor,ver,VAL_SOLID_SQUARE, VAL_BLACK);
				
				break;
			}
		case EVENT_DISCARD:

			break;
	}
	return 0;
}
