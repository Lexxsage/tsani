//==============================================================================
//
// Title:       lab1-2
// Purpose:     A short description of the application.
//
// Created on:  01.01.2008 at 1:16:22 by Gennady Kuzin.
// Copyright:   NSU. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include "lab1-2.h"
#include "toolbox.h"

#define LENGTH 1000

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

static int panelHandle;
double A = 5;//amplitude
double f = 100;//freq, hz
double phi = 0;//phase, rad
double arrSin[LENGTH], arrPower[LENGTH];
double noise = 0;
double I = 0;


//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions
void CreateSignal(){
	for (int i=0; i < LENGTH; i++){
		arrSin[i]=A*sin(2*PI*f*i/1000+phi*PI)+Random(-noise, noise);
	}
}

void DrawGraph(){
	 DeleteGraphPlot(PANEL, PANEL_GRAPH, -1, VAL_DELAYED_DRAW);
	 SetAxisScalingMode(PANEL, PANEL_GRAPH, VAL_LEFT_YAXIS, VAL_MANUAL, -15, 15);
	 SetAxisScalingMode(PANEL, PANEL_GRAPH, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, LENGTH);
	 PlotY(PANEL, PANEL_GRAPH, arrSin, LENGTH, VAL_DOUBLE, VAL_THIN_LINE, VAL_CONNECTED_POINTS, VAL_SOLID, 2, VAL_BLUE);
}

void CalculatePower(){
	I = A*A;
	
	for (int i=0; i<LENGTH; i++){
		arrPower[i]=(A*sin(2*PI*f*i/1000+phi*PI)+Random(-noise, noise))*(A*sin(2*PI*f*i/1000+phi*PI)+Random(-noise, noise));
	}
	
	 DeleteGraphPlot(PANEL, PANEL_Power, -1, VAL_DELAYED_DRAW);
	 SetAxisScalingMode(PANEL, PANEL_Power, VAL_LEFT_YAXIS, VAL_MANUAL, 0, 250);
	 SetAxisScalingMode(PANEL, PANEL_Power, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, LENGTH);
	 PlotY(PANEL, PANEL_Power, arrPower, LENGTH, VAL_DOUBLE, VAL_THIN_LINE, VAL_CONNECTED_POINTS, VAL_SOLID, 10, VAL_BLUE);
}

/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
    int error = 0;
    
    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "lab1-2.uir", PANEL));
    
	CreateSignal();
	DrawGraph();
	
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
int CVICALLBACK panelCB (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
    if (event == EVENT_CLOSE)
        QuitUserInterface (0);
    return 0;
}

int  CVICALLBACK Timer1(int panel, int control, int event, void *callbackData, int eventData1, int eventData2){
	if(event = EVENT_COMMIT)
	{
		CreateSignal();
		DrawGraph();
		CalculatePower();
	};
	return 0;
}

int  CVICALLBACK ChangePhi(int panel, int control, int event, void *callbackData, int eventData1, int eventData2){
	if(event = EVENT_COMMIT)
	{
		GetCtrlVal(PANEL, PANEL_Phi, &phi);
	};
	return 0;
}

int  CVICALLBACK ChangeAmplitude(int panel, int control, int event, void *callbackData, int eventData1, int eventData2){
	if(event = EVENT_COMMIT)
	{
		GetCtrlVal(PANEL, PANEL_Amplitude, &A);
	};
	return 0;
}

int  CVICALLBACK ChangeFrequence(int panel, int control, int event, void *callbackData, int eventData1, int eventData2){
	if(event = EVENT_COMMIT)
	{
		GetCtrlVal(PANEL, PANEL_Freq, &f);
	};
	return 0;
}

int  CVICALLBACK ChangeNoise(int panel, int control, int event, void *callbackData, int eventData1, int eventData2){
	if(event = EVENT_COMMIT)
	{
		GetCtrlVal(PANEL, PANEL_Noise, &noise);
	};
	return 0;
}

int  CVICALLBACK SaveGraph(int panel, int control, int event, void *callbackData, int eventData1, int eventData2){
	FILE *pF;
	if (event = EVENT_COMMIT){
		pF = fopen("data.txt", "w");
		for (int i=0; i<1000; i++)
		{
			fprintf(pF, "%i\t%f\n", i, arrSin[i]);   
		}
		fclose(pF);
	};
	return 0; 
}
