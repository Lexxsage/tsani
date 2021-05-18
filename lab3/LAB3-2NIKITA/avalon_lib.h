//==============================================================================
//
// Title:       avalon_lib2.h
// Purpose:     A short description of the interface.
//
// Created on:  19.09.2008 at 16:20:37 by Gennady Kuzin.
// Copyright:   NSU. All Rights Reserved.
//
//==============================================================================

#ifndef __avalon_lib2_H__
#define __avalon_lib2_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include "cvidef.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

int Declare_Your_Functions_Here (int x);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __avalon_lib2_H__ */
void avalonInit(void);
void input_Address(unsigned int data);
void outAddress(unsigned int *address);
int avalonRead(unsigned int address, unsigned int subaddress, unsigned int* data);
void avalonWrite(unsigned char address, unsigned short subaddress, unsigned short data);
void dac_init(void);
void adc_init(void);
void readADC(unsigned int *code);
void avalonWrite(unsigned char address, unsigned short subaddress, unsigned short data);
