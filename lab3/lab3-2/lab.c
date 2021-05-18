#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include <tsani.h>
#include "lab.h"
#include "toolbox.h"

#include "avalon.h"
#include "dac_adc.h"

#define DAC_LSB (3.3/255)
#define ADC_LSB (2.56/1023)


int DAC_transfer_func(int DACPanelHandle, double* data)
{
	int i;

	for(i = 0; i < 256; i++)
	{

		*(data + i) = i*DAC_LSB;
		*(data + i) = *(data + i)*sin((PI/2)*i/256) + Random(0,0.01);
		PlotStripChartPoint(DACPanelHandle, TabDAC_STRIPCHART, *(data + i));
		Delay(0.0001); 
	}
	
	return 0;
}

int ADC_transfer_func(int ADCPanelHandle, double* data)
{
	int i;
	double step = 2.56/1023;

	for(i = 0; i < 1024; i++)
	{
		*(data + i) = i*ADC_LSB;
		*(data + i) = *(data + i)*sin((PI/2)*i/1024) + Random(0,0.01); 
		
		PlotStripChartPoint(ADCPanelHandle, TabADC_STRIPCHART, *(data + i));
		Delay(0.00015); 
	}
	
	return 0;
}

int dac_calc_offset_error(double* data, double* value)
{
	*value = *data;
	
	return 0;
}

int adc_calc_offset_error(double* data, double* value)
{
	*value = *data;
	
	return 0;
}

int dac_calc_gain_error(double* data, double* value)
{
	*value = *(data + 255) - 3.3;
	
	return 0;
}

int adc_calc_gain_error(double* data, double* value)
{
	*value = *(data + 1023) - 2.56;
	
	return 0;
}

int dac_calc_integral_nonlinearity(double* data, double* value)
{
	int i;
	double data_line[256];
	double k, b; // y = k*x + b
	double difference;
	
	*value = 0;
	k =  (*(data + 255) - *data)/255;
	b = *data;
	for(i = 0; i < 256; i++)
	{
		data_line[i] = k*i + b;
		difference = fabs(data_line[i] - *(data + i));
		if(*value < difference) *value = difference;
	}
	
	return 0;
}

int adc_calc_integral_nonlinearity(double* data, double* value)
{
	int i;
	double data_line[1024];
	double k, b; // y = k*x + b
	double difference;
	
	*value = 0;
	k =  (*(data + 1023) - *data)/1023;
	b = *data;
	for(i = 0; i < 1024; i++)
	{
		data_line[i] = k*i + b;
		difference = fabs(data_line[i] - *(data + i));
		if(*value < difference) *value = difference;
	}
	
	return 0;
}

int dac_calc_differential_nonlinearity(double* data, double* value)
{
	int i;
	double difference;
	
	*value = 0;

	for(i = 1; i < 256; i++)
	{
		difference = fabs(fabs(*(data + i) - *(data + i - 1)) - DAC_LSB);
		if(*value < difference) *value = difference;
	}
	
	return 0;
}

int adc_calc_differential_nonlinearity(double* data, double* value)
{
	int i;
	double difference;
	
	*value = 0;

	for(i = 1; i < 1024; i++)
	{
		difference = fabs(fabs(*(data + i) - *(data + i - 1)) - ADC_LSB);
		if(*value < difference) *value = difference;
	}
	
	return 0;
}
