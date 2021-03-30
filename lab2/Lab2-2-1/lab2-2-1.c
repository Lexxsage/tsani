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

#define dac_max_code 255
#define adc_max_code 1024
#define dac_max_volt 3.3 
#define adc_max_volt 2.56
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


double dac_code_to_voltage(int code)
{
	return (double)code/dac_max_code*dac_max_volt;
}
double adc_code_to_voltage(int code)
{
	return (double)code/adc_max_code*adc_max_volt;
}
int dac_voltage_to_code(double voltage)
{
	return (int)(voltage/dac_max_volt*dac_max_code);
}
int adc_voltage_to_code(double voltage)
{
	return (int)(voltage/adc_max_volt*adc_max_code);
}
void dac_init(void)
{
	write(2,0x00,0x07);
}
void adc_init(void)
{
	write(2,0x10,0x07);
}
void dac_out(int channel, int code)
{
	write(2, channel, code);
}
void adc_in(int channel,double *data)
{
	write(2,0x12,0xff);
	write(2,0x13,0xff); 
	write(2,0x14,0xff);//initialaze
	write(2,0x11,0x03);//start1 iack1
	int check = 0x03;
	while (!((check)&(0x01)==0x01))
	{
		check = read(2,0x11);
		Delay(0.1);
	}
    check = read(2,channel);
	*data = ((double)check/adc_max_code*adc_max_volt);
}

//==============================================================================
// Global functions

// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
    int error = 0;
    ni6251Slot(2); 	

    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "lab2-2-1.uir", PANEL));
	adc_init();
	dac_init();
    
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

int CVICALLBACK DAC (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2) {
	switch (event) {
		case EVENT_COMMIT:
			{
				int code;
				switch(control)
				{
					case PANEL_DAC_VOLT:
						{
							double data;
							GetCtrlVal(PANEL, PANEL_DAC_VOLT, &data);
							code = dac_voltage_to_code(data);
							SetCtrlVal(PANEL, PANEL_DAC_CODE, code);
							break;
						}
					case PANEL_DAC_CODE:
						{
							GetCtrlVal(PANEL, PANEL_DAC_CODE, &code);
							SetCtrlVal(PANEL, PANEL_DAC_VOLT, dac_code_to_voltage(code));
							break;
						};
				}
				dac_out(0x02,code);
				dac_out(0x03,code);
				break;
			}
		case EVENT_LEFT_CLICK:

	
			break;
	}
	return 0;
}

int CVICALLBACK ADC (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2) {
	switch (event) {
		case EVENT_COMMIT:
			{
				double data;
				adc_in(0x16,&data);
				SetCtrlVal(PANEL, PANEL_VIN1_VOLT, data);
				SetCtrlVal(PANEL, PANEL_Vin1_code, adc_voltage_to_code(data));
				adc_in(0x17,&data);
				SetCtrlVal(PANEL, PANEL_VIN2_VOLT, data);
				SetCtrlVal(PANEL, PANEL_Vin2_code, adc_voltage_to_code(data));
				break;
			}
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
