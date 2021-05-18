//==============================================================================
//
// Title:       LAB-3-2
// Purpose:     A short description of the application.
//
// Created on:  22.08.2008 at 18:47:49 by Gennady Kuzin.
// Copyright:   NSU. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include <tsani.h>
#include "lab3_2.h"
#include "toolbox.h"
//#include "f:\?????\avalib (1).h"
//#include "f:\?????\dacad (1).h"
#include "avalon_lib.h"
//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

static int panelHandle;

	//==============================================================================
// Global functions



#include <stdio.h>
#include <stdlib.h>
double realcodeACP[26501];
double Source_3[26501];
double perfectACPvolt[26501];
double LSB=2.56/1024;
double ideal_to_1023=2.5575-2.56/1024/2;
double zero=0.0;
double scale=0.0;
double real_to_1023=0.0;
double integral[26501];
double max=0.0;
double quant_error=0.5;
int up_polka=0.0;
int down_polka=0.0;
void Write_data_to_file(){
    FILE * pCODE;
    pCODE=fopen("C:\\tmp\\TSANILab3-2\\AnalogcodeACP.txt","w");
    for(int i = 0;i<5;i++){
        fprintf(pCODE,"%f\n",2.02);
    }
    fclose(pCODE);
}


void Read_data_from_file(){
    FILE* pFcode;
    FILE* pFvolt;

    pFvolt=fopen("volt.txt","r"); // real code
    pFcode=fopen("realcodeACP.txt","r"); // real code

    for (int i=0;i<26501;i++){
        float temp_code;
        float t;
        fscanf(pFcode,"%f",&temp_code);
        realcodeACP[i]=temp_code;
        fscanf(pFvolt,"%f",&t);
        perfectACPvolt[i]=t;
      //printf("%f\n",realcodeACP[i]);
    }

    fclose(pFcode);
    fclose(pFvolt);
	PlotXY(panelHandle,PANEL_GRAPH,perfectACPvolt,realcodeACP,26501,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_NO_POINT,VAL_SOLID,1,VAL_RED);
	//PlotXY(panelHandle,PANEL_GRAPH,Source_3,Input,26501,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_NO_POINT,VAL_SOLID,1,VAL_GREEN);
	

}


void Zerroerror(){
    int j=0;
    do{
       j++;
    } while (realcodeACP[j]!=1);
    down_polka=j;
    zero= perfectACPvolt[j] -0.5*LSB;
    //printf("%lf\n",zero/LSB);
	SetCtrlVal(panelHandle,PANEL_zero,zero/LSB); 

}

void without_zero(){
    for (int i=0; i<26501;i++){
        perfectACPvolt[i]=perfectACPvolt[i]- zero;
		  //Source_3[i]=perfectACPvolt[i]- zero;
        //printf("%lf\n",perfectACPvolt[i]);
	//PlotXY(panelHandle,PANEL_GRAPH,Source_3,realcodeACP,26501,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_NO_POINT,VAL_SOLID,1,VAL_GREEN); 
    }
}

void Scale_error(){
    int j=0;
    do{
        j++;
        //printf("%lf\n",realcodeACP[j]);
    } while (realcodeACP[j]!=1023);
    up_polka=j;
    scale=(ideal_to_1023 - perfectACPvolt[j]);
    real_to_1023 =perfectACPvolt[j];
    //double scale=(ideal_to_1023 - perfectACPvolt[j+1]);
 // printf("%f\n",scale/LSB);
   // printf("%f\n",ideal_to_1023);
   // printf("%f\n",perfectACPvolt[j]);
	SetCtrlVal(panelHandle,PANEL_scale,scale/LSB);
}

void without_scale(){
    for (int i=574;i<26501;i++){
        perfectACPvolt[i]=perfectACPvolt[i]*(ideal_to_1023/real_to_1023);
       // printf("%f\n",perfectACPvolt[i]);
		 
    }
	PlotXY(panelHandle,PANEL_GRAPH,perfectACPvolt,realcodeACP,26501,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_NO_POINT,VAL_SOLID,1,VAL_GREEN);
}
void integral_error(){
    for (int i=down_polka;i<up_polka+1;i++){
        integral[i]=(perfectACPvolt[i]-LSB*realcodeACP[i])/LSB; //+quant_error;
        /*if(integral[i]<0){
            integral[i]=(-1)*integral[i];
        } */
        if(integral[i]>max){
            max =integral[i];
        }

        //printf("%f\n", integral[i]);
    }
    //printf("%f\n", max);
	SetCtrlVal(panelHandle,PANEL_int,max);
	PlotXY(panelHandle,PANEL_GRAPH_2,perfectACPvolt,integral,26501,VAL_DOUBLE,VAL_DOUBLE,VAL_THIN_LINE,VAL_NO_POINT,VAL_SOLID,1,VAL_RED);
}

 

/*void ADC_inp()
{
	for (int i=0;i<8192;i++){
		X[i]=ADC_LSB*i/8; //volt
		Source[i]=i/8;
		analogOut(0, X[i]); //DAC PXI
		adc_in(0x16, &Data[i]);// volt ACD AVALON
		//avalon_write(2, 0x16, Source[i]);
		//analogOut(0, Data[i]); //code
		//Data[i]*=ADC_LSB; // acd volt
		Data_code[i]=adc_voltage_to_code(Data[i]);
		
	}
	PlotXY(panelHandle,PANEL_GRAPH, X, Source, 8192,VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE,VAL_NO_POINT,VAL_SOLID,1,VAL_RED);
	PlotXY(panelHandle,PANEL_GRAPH, X, Data_code,8192,VAL_DOUBLE, VAL_INTEGER, VAL_THIN_LINE,VAL_NO_POINT,VAL_SOLID,1,VAL_GREEN);
	
}  */



//	SetCtrlVal(panelHandle,PANEL_INTERR,max);

/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
    int error = 0;
    
    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "lab3_2.uir", PANEL));
//	avalonInit();
//	adc_init();
//	dac_init();
	
    Read_data_from_file();
   Zerroerror();
    without_zero();
   Scale_error();
    without_scale();
    integral_error();
 //   Write_data_to_file();
//	SetCtrlVal(panelHandle,PANEL_SCERR_LSB,4.0);
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
