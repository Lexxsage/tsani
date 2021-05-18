#include "tsani.h"
#include <utility.h>
#include "avalon.h"  
#include "dac_adc.h"

int dac_init(void)
{  
	avalon_write(2,  0x00, 0x07); 
	
	return 0;
}	

int adc_init(void)
{ 
	avalon_write(2, 0x10, 0x07); 
	return 0;
}

int dac_adc_out(int dac_num, double data)
{
	check_value_double(&data, 0, 3.3);
	int value = (int)(data*255/3.3); 
	dac_adc_out_code(dac_num, value);
	
	return 0;
 }

int dac_adc_out_code(int dac_num, int code)
{ 
	int subaddress;
	if(dac_num == 1)
	{
		subaddress = 0x02; //VOUT1
	}
	else if(dac_num == 2)
	{
		subaddress = 0x03; //VOUT2
	}
	check_value_int(&code, 0, 255);
	avalon_write(2, subaddress, code);
	
	return 0;
}

int dac_adc_in(int adc_num, double* value)
{
	int temp = 0x03;
	int subaddress;
	if(adc_num == 1)
	{
		subaddress = 0x16; //VIN1
	}
	else if(adc_num == 2)
	{
		subaddress = 0x17; //VIN2
	}
	avalon_write(2, 0x12, 0x00);
	avalon_write(2, 0x13, 0x00); 
	avalon_write(2, 0x14, 0x00); 
	avalon_write(2, 0x11, 0x03);
	while((temp & 0x01) == 0x01)
	{
		Delay(0.1); 
	    avalon_read(2, 0x11, &temp); 
    }
	avalon_read(2, subaddress, &temp);
	*value = (double)(temp*(2.56/1024));
	
	return 0;
}

int dac_adc_in_code(int adc_num, int* value)
{
	int temp = 0x03;
	int subaddress;
	if(adc_num == 1)
	{
		subaddress = 0x16; //VIN1
	}
	else if(adc_num == 2)
	{
		subaddress = 0x17; //VIN2
	}
	avalon_write(2, 0x12, 0x00);
	avalon_write(2, 0x13, 0x00);
	avalon_write(2, 0x14, 0x00);   
	avalon_write(2, 0x11, 0x03);  
	while((temp & 0x01) == 0x01) 
	{
		Delay(0.1);
	    avalon_read(2, 0x11, &temp); 
    }
	avalon_read(2, subaddress, &temp);
	*value = temp;
	
	return 0;
}
