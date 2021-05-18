#include "tsani.h"
//==============================================================================
//
// Title:       avalon_lib.c
// Purpose:     A short description of the implementation.
//
// Created on:  19.09.2008 at 16:07:46 by Gennady Kuzin.
// Copyright:   NSU. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

//#include "avalon_lib.h"

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables

//==============================================================================
// Global functions

/// HIFN  What does your function do?
/// HIPAR x/What inputs does your function expect?
/// HIRET What does your function return?
int Define_Your_Functions_Here (int x)
{
    return x;
}

void avalonInit(void) {
    portMask(0, 0xFF);
    portMask(1, 0x0F);
    portMask(2, 0x07);
    portOut(2, 0x06);
}

void input_Address(unsigned int data)
{
    unsigned char port0, port1;
    port0 = data & 0xFF;
    port1 = (data >> 8) & 0x0F;
    portOut(0, data & 0xFF);
    portOut(1, (data >> 8) & 0x0F);
}
void outAddress(unsigned int *address)
{
    unsigned char a1, a2;
    portIn(1, &a1);
    portIn(0, &a2);
    *address = (a1 << 8) + a2;
}

void avalonRead(unsigned int address, unsigned int subaddress, unsigned int* data) {
    unsigned int full_address;
    full_address = (subaddress << 3) | address;
    input_Address(full_address);
    portOut(2, 0x07);	//ALE 1;
    Delay(4);
    portOut(2, 0x06);	//ALE 0;
    portMask(0, 0x00);
    portMask(1, 0x00);
    portOut(2, 0x04);	//Read 0;
    Delay(6);
    outAddress(data);
    portOut(2, 0x06);	//Read 1;
    portMask(0, 0xFF);
    portMask(1, 0x0F);
}

void avalonWrite(unsigned char address, unsigned short subaddress, unsigned short data) {
    unsigned int full_address;
    full_address = (subaddress << 3) | address;
    unsigned char port0, port1;

    input_Address(full_address);
    portOut(2, 0x07);	//ALE 1;
    Delay(4);
    portOut(2, 0x06);	//ALE 0 ;
    input_Address(data);
    portOut(2, 0x02);	//Write 0;
    Delay(4);
    portOut(2, 0x06);	//Write 1;
}

void dac_init(void)
{
	avalonWrite(2,0x00,0x07);
}
void adc_init(void)
{
	avalonWrite(2,0x10,0x07);
}


void readADC(unsigned int *code)
{
    avalonWrite(0x02, 0x14, 0x00);
    avalonWrite(0x02, 0x10, 0x07);
    avalonWrite(0x02, 0x10, 0x03);
    unsigned int current_start = 0x01;
    avalonWrite(0x02, 0x11, 0x01);
    while((current_start & 0x01) != 0)
    {
        avalonRead(0x02, 0x11,&current_start);
    }
    avalonRead(0x02, 0x16, code);
}
