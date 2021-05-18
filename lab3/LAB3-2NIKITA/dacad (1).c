#include <utility.h>
#include "tsani.h" 
#include "f:\�����\avalib (1).h"
#include "f:\�����\avalib (1).h"
#define dac_max_code 255
#define adc_max_code 1024
#define dac_max_volt 3.3 
#define adc_max_volt 2.56
/*void data_check(void *data,double max, double min)
{
	if (sizeof(data)==sizeof(int))
	{
		if ((int*)(data) > max)
		{
			(int*)(data)=max; 
		}
		else if((int*)(data) < min)
		{
			(int*)(data)=min; 
		}
	}
}*/
/*void code_check(int *code,int min, int max)
{
	if (*code > max)
	{
		*code = max;
	}
	else if (*code < min)
	{
		*code = min;
	}
}*/
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
	avalon_write(2,0x00,0x07);//Регистр управления
}
void adc_init(void)
{
	avalon_write(2,0x10,0x07);//00111(регистр управления ацп)
}
void dac_out(int channel, int code)
{
	avalon_write(2, channel, code);
}
void adc_in(int channel,double *data)
{
	avalon_write(2,0x12,0xff);//Регистр начального адреса
	avalon_write(2,0x13,0xff); //Регистр конечного адреса
	avalon_write(2,0x14,0xff);//initialaze,Регистр таймера
	avalon_write(2,0x11,0x03);//start1 iack1(11)
	int check = 0x03;
	while (!((check)&(0x01)==0x01))
	{
		avalon_read(2,0x11,&check);
		Delay(0.1);
	}
	avalon_read(2,channel,&check);
	*data = ((double)check/adc_max_code*adc_max_volt);
}

