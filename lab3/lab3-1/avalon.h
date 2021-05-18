//==============================================================================
//
// Title:       avalon.c.h
// Purpose:     A short description of the interface.
//
// Created on:  01.01.2008 at 0:55:34 by Gennady Kuzin.
// Copyright:   NSU. All Rights Reserved.
//
//==============================================================================

#ifndef __avalonc_H__
#define __avalonc_H__

#ifdef __cplusplus
    extern "C" {
#endif

#include "cvidef.h"
void init(void);
void add(unsigned char address, unsigned int subaddress, unsigned int* adr);
void divide(unsigned int address, unsigned char *adr0, unsigned char* adr1);
void write(unsigned int address, unsigned int data);
void read(unsigned int address, unsigned int* data);
void code(unsigned char code);
void volt(double v);
void acp(double *data);



#ifdef __cplusplus
    }
#endif

#endif  /* ndef __avalonc_H__ */
