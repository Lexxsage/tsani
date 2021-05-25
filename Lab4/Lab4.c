//==============================================================================
//
// Title:       Lab4
// Purpose:     A short description of the application.
//
// Created on:  05.09.2008 at 17:32:18 by Gennady Kuzin.
// Copyright:   NSU. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include <tsani.h>
#include "Lab4.h"
#include "toolbox.h"
//#include "i2c_lib.h"
DELAY = 0.5 ;
//==============================================================================
// Constants
char result[9];
//==============================================================================
// Types
 void i2c_init(){
  	portMask(0,0x7F);
	portMask(1,0x00);
	portOut(0,0xFF);
	portOut(1,0);
//SDAin  
}
 
void Write_to_i2c(int sda, int scl) {
  int data=(sda<<3)|(scl<<4)| 0x07;//SDAout,SCLout
  portOut(0,data);
}


void Read_to_i2c(int *sda, int *scl) {
  unsigned char data1,data2;
  portIn(0,&data1);
  portIn(0,&data2);
  *sda=(data1>>7)&0x01;//SDAIn
  *scl=(data2)&0x01;//SCLIn
  
}


void Start(){
  Write_to_i2c(1,1);
  Delay(DELAY);
  Write_to_i2c(0,1);
  Delay(DELAY);
  Write_to_i2c(0,0);
  Delay(DELAY);
}

void Stop(){
  Write_to_i2c(0,0);
  Delay(DELAY);
  Write_to_i2c(0,1);
  Delay(DELAY);
  Write_to_i2c(1,1);
  Delay(DELAY);
}



int i2c_sda(){
  unsigned char data;
  portIn(0,&data);
  return (data>>7)&0x01;
}

void SendBit(int Bit){
  Write_to_i2c(i2c_sda(),0);
  Delay(DELAY);
  Write_to_i2c(Bit,0);
  Delay(DELAY);
  Write_to_i2c(Bit,1);
  Delay(DELAY);
  Write_to_i2c(Bit,0);
  Delay(DELAY);
}

void SendByte(int Byte){
  for(int i = 0; i<8; i++)
  SendBit((Byte >> (7-i))&0x01);
  
}

int ReadBit(){
  int Bit;
  Write_to_i2c(1,0);
  
  Write_to_i2c(1,1);
  
  Bit=i2c_sda();
  Write_to_i2c(1,0);
  
  return Bit;
}

void ReadByte (int* Byte){
  int data=0;
  for(int i=0; i<8;i++)
  { 
    data=data|(ReadBit()<<(7-i));
  }
  *Byte=data;
}


void getAck(){ 
  int Bit;
  Bit=ReadBit();
  Delay(DELAY); 
  if(Bit !=0) printf("%-20s","NOT_ACKNOWLEDG");
  
}


void sendNotAck(){
  SendBit(1); 
}

void sendAck(){
   SendBit(0);
}

void Write_transaction(int data,int adr, int subadr){
  adr=adr<<1;
  Start();
  SendByte(adr);
  
  getAck();
   
  SendByte(subadr);
   
  getAck();
   
  SendByte(data);
   
  getAck();
   
  Stop();
}

void Read_transaction(int* data,int adr, int subadr, int n){
  adr=adr<<1;
  Start();
   
  SendByte(adr);
   
  getAck();
  
  SendByte(subadr);
 
  getAck();
  
  Stop();
  Start();
  adr=adr|0x01;
  SendByte(adr);
  
  getAck();
   
  for (int i=0;i<n;i++){
    ReadByte(data+i);
    if(i !=(n-1)) sendAck();
  }
  sendNotAck();
  Stop();
}

////áÅÃÓÙÀß ÅÄÈÍÈÖÀ
void running_unit(){
  
	static  int i=1;
	if(i<129)
  	{
    	Write_transaction(i,0,0);
		i*=2;
  	}
	else
		i = 1;
}

//Ñ×ÈÒÛÂÀÍÈÅ ÈÌÅÍÈ
void read_name(){
  
  result[8]=0;
  int data=0;
  for (int i=0;i<8;i++)
  {
    Read_transaction(&data,0,(0x08+i),1);
    result[i]=(char)data;
	//printf("%c", result[i]);
  }
}

//Ñ×ÈÒÛÂÀÍÈÅ ÒÅÌÏÅÐÀÒÓÐÛ
double Read_tempeature(){
  double value=0;
  int temp,b;
  int data=0;
  int Byte1=0;
  int Byte2=0;
  int adr=0x28;	
  adr=(adr<<1)|0x01;
  Start();
  SendByte(adr);
  getAck();
  ReadByte(&Byte1);
  sendAck();
  ReadByte(&Byte2);
  data=((Byte1>>7)&0x01);
  if (data==1)
  {
    b=Byte1;
    temp=b|0xFF00;
    data=temp;
  }
  if (data==0)
  {
    data=Byte1;
  }
  Byte2=(Byte2>>6)&0x03;
  value=(double)data+Byte2*0.25;
  sendNotAck();
  Stop();
  return value;
} 


//==============================================================================
// Static global variables

static int panelHandle;


int date = 0x01;
int flag = 0;
int switch_on =0;
int zero=0;
double temper[1000];
int idx = 0;
double X[1000];

double Temperature;
char name[9];
int main (int argc, char *argv[])
{
    int error = 0;
      ni6251Slot(2);
	
    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "Lab4.uir", PANEL));
	//////////////////////////
    i2c_init();
	//////////////////////////
	SetAxisScalingMode(panelHandle, PANEL_STRIPCHART, VAL_LEFT_YAXIS, VAL_AUTOSCALE, -10.00, 35.00);
	SetAxisScalingMode(panelHandle, PANEL_STRIPCHART, VAL_BOTTOM_XAXIS, VAL_MANUAL, 0, 10.00);
    /* display the panel and run the user interface */
    errChk (DisplayPanel (panelHandle));
    errChk (RunUserInterface ());

Error:
    /* clean up */
    DiscardPanel (panelHandle);
	ni6251Close();
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

int CVICALLBACK timer_light (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2) {
	switch (event) {
		case EVENT_TIMER_TICK:
			if(flag){
				running_unit();
			}
			break;
		case EVENT_DISCARD:

			break;
	}
	return 0;
}

int CVICALLBACK timerGraph (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2) {
	switch (event) {
		case EVENT_TIMER_TICK:
			if(switch_on){
				PlotStripChartPoint(panelHandle, PANEL_STRIPCHART,  Read_tempeature());
			}
			
			break;
		case EVENT_DISCARD:

			break;
	}
	return 0;
}

int CVICALLBACK getTemper (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2) {
	switch (event) {
		case EVENT_COMMIT:
			PlotStripChartPoint(panelHandle, PANEL_STRIPCHART,  Read_tempeature());
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

int CVICALLBACK getname (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2) {
	switch (event) {
		case EVENT_COMMIT:
			read_name(); 
			SetCtrlVal(panelHandle,PANEL_STRING, result);
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

int CVICALLBACK start_lights (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2) {
	switch (event) {
		case EVENT_COMMIT:
			flag = !flag;
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

int CVICALLBACK Switch (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2) {
	switch (event) {
		case EVENT_COMMIT:
			switch_on = !switch_on;
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
