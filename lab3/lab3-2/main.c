#include <analysis.h>
#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include <tsani.h>
#include "lab.h"
#include "toolbox.h"

#include "avalon.h"
#include "dac_adc.h"
#include "functions.h"

static int panelHandle;

int main(int argc, char *argv[])
{
    int error = 0;
	ni6251Slot(2);
	avalon_init();
	adc_init();
    
    /* initialize and load resources */
    nullChk (InitCVIRTE (0, argv, 0));
    errChk (panelHandle = LoadPanel (0, "lab.uir", PANEL));
    
    /* display the panel and run the user interface */
    errChk (DisplayPanel (panelHandle));
    errChk (RunUserInterface ());

Error:
    /* clean up */
    DiscardPanel (panelHandle);
	ni6251Close();
    return 0;
}


int CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2)
{
    if (event == EVENT_CLOSE)
        QuitUserInterface (0);
    return 0;
}

int CVICALLBACK ADC_button(int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_COMMIT)
	{
		int ADCPanelHandle, i, start, end;
		int data[NADCPOINTS];
		double value;
		double u[NADCPOINTS], difference[NADCPOINTS],INL[NADCPOINTS];
		double u_calibrated[NADCPOINTS], ADC_LSB_calibrated;
		FILE *pF;
		
		DeleteGraphPlot(panelHandle,PANEL_GRAPH , -1, VAL_IMMEDIATE_DRAW);
		DeleteGraphPlot(panelHandle,PANEL_GRAPH2, -1, VAL_IMMEDIATE_DRAW);
		SetCtrlVal(panelHandle, PANEL_NUMERIC, 0.0);
		SetCtrlVal(panelHandle, PANEL_NUMERIC_2, 0.0);
		SetCtrlVal(panelHandle, PANEL_NUMERIC_3, 0.0);
		SetCtrlVal(panelHandle, PANEL_NUMERIC_4, 0.0);
		
		switch (control) 
		{
			case  PANEL_ADC_BUTTON:
				ADC_transfer_func(panelHandle, &data[0]);
				pF = fopen("ADC.txt", "w");
				for(i = 0; i < NADCPOINTS; i++)
				{
					fprintf(pF, "%i\n", data[i]);
					u[i] = 2.56/(NADCPOINTS-1) * i;
				}
				fclose(pF);
				break;
			case  PANEL_ADC_BUTTON_2:
				pF = fopen("ADC.txt", "r");
				for(i = 0; i < NADCPOINTS; i++)
				{
					fscanf(pF, "%i", &data[i]);
					u[i] = 2.56/(NADCPOINTS-1) * i;
				}
				fclose(pF);
				break;
		}
		
		PlotXY(panelHandle,  PANEL_GRAPH, u, data, NADCPOINTS, VAL_DOUBLE, VAL_INTEGER, VAL_SCATTER, VAL_SIMPLE_DOT, VAL_SOLID, 1, VAL_GREEN);


		difference[0] = 0;
		for(i = 1; i < NADCPOINTS; i++)
		{
			difference[i] = *(data + i) - *(data + i - 1) - 1024/NADCPOINTS;
		}
		pF = fopen("ADC.txt", "r");
				for(i = 0; i < NADCPOINTS; i++)
				{
					fscanf(pF, "%i", &data[i]);
					u[i] = 2.56/(NADCPOINTS-1) * i;
				}
				fclose(pF);
		PlotXY(panelHandle, PANEL_GRAPH2, u, difference, NADCPOINTS, VAL_DOUBLE, VAL_DOUBLE, VAL_SCATTER, VAL_SIMPLE_DOT, VAL_SOLID, 1, VAL_RED);

	
		if(*data == 0)
		{
			for(start = 0; start < (NADCPOINTS-1); start++) if(*(data + start) != 0) break;
			start--;
		}
		else start = 0;
		start = *(data + start);
		
		for(end = 0; end < (NADCPOINTS-1); end++) if(*(data + end) == 1023) break;
		end = *(data + end);
		
		ADC_LSB_calibrated = 2.56/(double)(end-start);

		for(i = 0; i < NADCPOINTS; i++) u_calibrated[i] = *(data + i)*ADC_LSB_calibrated + start*ADC_LSB;
		pF = fopen("debug.txt", "w");
				for(i = 0; i < NADCPOINTS; i++)
				{
					fprintf(pF, "%i\n", *(data + i));
				}
				fclose(pF);
		PlotXY(panelHandle, PANEL_GRAPH, u_calibrated, data, NADCPOINTS, VAL_DOUBLE, VAL_INTEGER, VAL_SCATTER, VAL_SIMPLE_DOT, VAL_SOLID, 1, VAL_RED);
	
	    adc_calc_offset_error(&data[0], &value);
		SetCtrlVal(panelHandle, PANEL_NUMERIC, (value/ADC_LSB));
		adc_calc_gain_error(&data[0], &value);
		SetCtrlVal(panelHandle, PANEL_NUMERIC_2, (value/ADC_LSB));
		adc_calc_integral_nonlinearity(&data[0], &value);
		SetCtrlVal(panelHandle,PANEL_NUMERIC_3, (value));
		adc_calc_differential_nonlinearity(&data[0], &value);
		SetCtrlVal(panelHandle,PANEL_NUMERIC_4, (value));
	}
	return 0;
}
