#ifndef __dacad_H__
#define __dacad_H__

#ifdef __cplusplus
    extern "C" {
#endif


#include "cvidef.h"

void dac_init(void);
void adc_init(void);
void dac_out(int,int);
void adc_in(int,double*);
//void code_check(int* ,int, int);
double dac_code_to_voltage(int);
double adc_code_to_voltage(int);
int dac_voltage_to_code(double);
int adc_voltage_to_code(double);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __dacad_H__ */
