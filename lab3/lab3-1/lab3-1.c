//==============================================================================
//
// Title:       lab3-1
// Purpose:     A short description of the application.
//
// Created on:  01.01.2008 at 0:36:28 by Gennady Kuzin.
// Copyright:   NSU. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include <tsani.h>
#include "lab3-1.h"
#include "toolbox.h"
#include "avalon.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

static int panelHandle;
double arr[256];
double ide[256];
double arr_2[256];

void per(){
	int i;
	double u;
	for (i=0; i<256;i++){
		ide[i]=3.3*i/255;
	}
	for (i=0; i<256;i++){
		code((char)i);
		analogIn(0, &u);
		arr[i]=u;
		arr_2[i]=u;
	}
	for (i=0; i<256;i++){
		arr_2[i]=arr_2[i]-arr[0];
	}
	for (i=0; i<256;i++){
		arr_2[i]=arr_2[i]/arr_2[255]*ide[255];
	}
	for (i=0; i<256;i++){
		arr[i]=arr[i]/3.3*255;
	}
	
	PlotY(panelHandle, PANEL_GRAPH, arr, 256, VAL_DOUBLE, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_RED);
}

void inl(){
	double dif[256];
	for (int i=0; i<256;i++){
		dif[i]=fabs(arr_2[i]-ide[i]);
		dif[i]=dif[i]/3.3*255;
	}
		PlotY(panelHandle, PANEL_GRAPH_2, dif, 256, VAL_DOUBLE, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_GREEN);
}

void diff(){
	double diff[255];
	for (int i=0; i<255;i++){
		diff[i]=(arr_2[i+1]-arr_2[i])*255/3.3-1;
	}
		PlotY(panelHandle, PANEL_GRAPH_3, diff, 255, VAL_DOUBLE, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_GREEN);
}

/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
    int error = 0;
    
    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "lab3-1.uir", PANEL));
	ni6251Slot(2);
	init();
	per();
	inl();
	diff();
    
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
