#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include <tsani.h>
#include "lab.h"
#include "toolbox.h"

#include "avalon.h"
#include "dac_adc.h"
#include "functions.h"


int ADC_transfer_func(int ADCPanelHandle, int* data)
{
	int i;
	
	for(i = 0; i < NADCPOINTS; i++)
	{
		analogOut(0, (2.56/(NADCPOINTS-1) * i));
		dac_adc_in_code(1, (data + i));
	}
	
	return 0;
}

int adc_calc_offset_error(int* data, double* value)
{
	int i = 0;
	while(*(data + i) == 0) i++;

	*value = -ADC_LSB*i + *data;
	
	return 0;
}


int adc_calc_gain_error(int* data, double* value)
{
	int i;
	double full_scale_error, offset_error;
	adc_calc_offset_error(data, &offset_error);
	for(i = 0; i < NADCPOINTS; i++)
	{
		if(*(data + i) == 1023) break;
	}
	full_scale_error = 2.56*(1 - ((double)i/(NADCPOINTS-1)));
	*value = full_scale_error - offset_error;
	
	return 0;
}

int adc_calc_integral_nonlinearity(int* data, double* value)
{
	int i;
	double data_line[NADCPOINTS];
	double k, b; // y = k*x + b
	int difference;
	
	*value = 0;
	k =  (double)(*(data + (NADCPOINTS-1)) - *data)/NADCPOINTS;
	b = *data;
	
	for(i = 0; i < NADCPOINTS; i++)
	{
		data_line[i] = k*i + b; 

		difference = fabs(data_line[i] - *(data + i));
		if(*value < difference) *value = difference;
	}
	
	return 0;
}

int adc_calc_differential_nonlinearity(int* data, double* value)
{
	int i;
	double difference;
	
	*value = 0;

	for(i = 1; i < NADCPOINTS; i++)
	{
		difference = fabs(fabs(*(data + i) - *(data + i - 1)) - 1);
		if(*value < difference) *value = difference;
	}
	
	return 0;
}
