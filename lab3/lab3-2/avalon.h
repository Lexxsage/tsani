#ifndef __avalon_H__
#define __avalon_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include "cvidef.h"
		
int check_value_int(int* value, int min, int max);
int check_value_double(double* value, double min, double max);
int avalon_init(void);
int avalon_write(int address, int subaddress, int value);
int avalon_read(int address, int subaddress, int* value);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __avalon_H__ */
