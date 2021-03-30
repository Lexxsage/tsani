//==============================================================================
//
// Title:       lab2-2-1
// Purpose:     A short description of the application.
//
// Created on:  01.01.2008 at 1:54:10 by Gennady Kuzin.
// Copyright:   NSU. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include <tsani.h>
#include "lab2-2-1.h"
#include "toolbox.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables
int address, subaddress, data;
int value;
static int panelHandle;

//==============================================================================
// Static functions

//==============================================================================
// Global variables

void init(int address, int subaddress)
{
	portMask(0, 0xFF);			  
	portMask(1, 0xFF);			  
	portMask(2, 0x07);			 
	portOut(2, 0x06);			
	
	int a = (subaddress<<3)|address;  //form address  
	
	unsigned char a0;
	unsigned char a1; 
	
	a0 = (a&0xFF);
	a1 = (a>>8)&0xFF;    //formed address
	
	portOut(0, a0);
	portOut(1, a1); // write address on shina 
	
	portOut(2, 0x07);   	//write in ALE 1 
	portOut(2, 0x06);      //write in ALE 0	
} 

void write (int address, int subaddress, int data){
	init(address, subaddress);
	
	portMask(0, 0xff);
	portMask(1, 0xff);
	
	unsigned char w0,w1;
	
	w0 = (data & 0xff);
	w1 = (data >>8) & 0xff;
	
	portOut(0, w0);
	portOut(1, w1);
	
	portOut(2, 0x02);
	portOut(2, 0x06);
	
}

int read (int address, int subaddress)
{   
	init(address, subaddress); 
	
	portOut(2, 0x04); 	  //write 0 in read
	portMask(0, 0x00);   
	portMask(1, 0x00);     
	Delay(0.006);		// wait
	
	unsigned char r0;
	unsigned char r1;
	
	portIn(0, &r0);		//read data from shina 
	portIn(1, &r1); 	 
	
    //write in read 1 
	portOut(2, 0x06); 
	portMask(0, 0xFF);   //  change in do write 
	portMask(1, 0xFF);   //  AD in write mode
	
	return (r0 | (r1<<8)); 
}
void WriteDAC(double U)
{
		int Code;
		Code=256/3.3*U;
		if (Code<0) U=0;
		if (Code>255) U=255;
		write(2,2,Code);
}
void Snat()
{
	int reg;
	reg = read(2, 0x11);
	reg = reg|0x2;
	write(2, 0x11, reg);
}

int ACP()
{
	int codeACP;
	unsigned char r=0x00;
	unsigned char s=0x00;
	
	write(2, 0x10, 5); //?????.

	//????????
	write(2, 0x12, 0);  //???.
	write(2, 0x13, 0);  //?????.
	write(2, 0x14, 0);   //??????  
	
	Snat();
	Delay(2);
	write(2, 0x11, 1);   //????????? ????? Start ? IACK ? ????????? ??????
	
	// ????? ????? ???????? ??????   
	
	while (s!=0x20)
	{
		portIn(2, &r);
		s = r&0x20;
	}  
	Delay(1);
	Snat();  
	codeACP=read(2, 0x16);
	return codeACP; 
	
}
void Prer()
{
	int reg;
	reg = read(2, 0x11);
	reg = reg|0x4;
	write(2, 0x11, reg);
}


//==============================================================================
// Global functions

/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
    int error = 0;
    ni6251Slot(2); 	

    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "lab2-2-1.uir", PANEL));
    
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


int  CVICALLBACK DoWrite(int panel, int control, int event, void *callbackData, int eventData1, int eventData2){
	switch(event){
		case EVENT_COMMIT:
			GetCtrlVal(PANEL, PANEL_ADDRESS, & address);
			GetCtrlVal(PANEL, PANEL_SUBADDRESS, & subaddress);
			GetCtrlVal(PANEL, PANEL_DATA, & data);
			
			write(address, subaddress, data);
			break;
	}
	return 0;
}

int  CVICALLBACK DoRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2){
	switch (event) 
	{
		case EVENT_COMMIT: 
			GetCtrlVal(PANEL,PANEL_ADDRESS_2, &address);  
			GetCtrlVal(PANEL,PANEL_SUBADDRESS_2, &subaddress);
			value= read(address,subaddress); 
			SetCtrlVal(PANEL, PANEL_DATA_2, value); 
	}
	return 0;
}

int  CVICALLBACK adc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2){
	 int codeACP;
	switch (event) 
	{
		case EVENT_COMMIT:
			
			double U;
			codeACP=ACP();
			U = codeACP*2.56/1024;
			SetCtrlVal(PANEL, PANEL_DATA, U);
			break;
	}
	return 0;

}

int  CVICALLBACK prep(int panel, int control, int event, void *callbackData, int eventData1, int eventData2){
	 switch (event) 
	{
		case EVENT_COMMIT:
			Prer();
	}
	return 0;
}

int  CVICALLBACK snat(int panel, int control, int event, void *callbackData, int eventData1, int eventData2){
	 switch (event) 
	{
		case EVENT_COMMIT:
			Snat();
	}
	return 0;
}
