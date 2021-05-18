#ifndef __functions_H__
#define __functions_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include "cvidef.h"

#define NADCPOINTS 1024
#define ADC_LSB (2.56/1023)

int ADC_transfer_func(int ADCPanelHandle, int* data);
int adc_calc_offset_error(int* data, double* value);
int adc_calc_gain_error(int* data, double* value);
int adc_calc_integral_nonlinearity(int* data, double* value);
int adc_calc_differential_nonlinearity(int* data, double* value);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __functions_H__ */
