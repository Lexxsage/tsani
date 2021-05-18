#include <formatio.h>
//==============================================================================
//
// Title:       tsani
// Purpose:     A short description of the library.
//
// Created on:  03.04.2014 at 16:04:44 by UserName.
// Copyright:   UserOrg. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files

#include <utility.h>
#include <NIDAQmx.h>
#include <analysis.h>

#include <ansi_c.h>
#include <cvirte.h>     
#include <userint.h>
#include <string.h>
#include <toolbox.h>
#include "tsani.h"
#include "tsani_error.h"

//==============================================================================
// Constants

//==============================================================================
// Types

#define processError(error) \
	if(error != 0) \
		DAQmxGetErrorString(error, TSANI_ERROR_MESSAGE_BUFFER, TSANI_ERROR_MESSAGE_SIZE); \
	else \
	error = -1; \
	tsani_SetLastError(error);


//==============================================================================
// Static global variables

//==============================================================================
// Static functions

//==============================================================================
// Global variables
typedef struct PORT{
	int mask;      // -1 - слот свободен, 0-0xff - маска, бит рявняется 0 -чтение, 1 - запись
	TaskHandle to; // хэндлер таска вывода, 0 - таск не используется
	TaskHandle ti; // хэндлер таска ввода,  0 - таск не используется
}PORT;

#define PORT_SIZE 3
#define CACHE_SIZE 4
#define ANALOG_SIZEi 8
#define ANALOG_SIZEo 2

typedef struct CLK{
	unsigned int maskCLK;
	char sourceCLK[256];
	double frequency;
	unsigned int maskTRIG;
	char sourceTRIG[256];
	double level;
	unsigned int maskFetch;
}CLK;

typedef struct CACHE{
	int slot;
	PORT ports[PORT_SIZE][CACHE_SIZE]; // кэш тасков, активный кэш с 0 индексом
	TaskHandle analogto[ANALOG_SIZEo]; // таски аналогового вывода, 0 не используется
	TaskHandle analogti[ANALOG_SIZEi]; // таски аналогового ввода, 0 не используется
	int port_out[PORT_SIZE];           // кэш данных вывода
}CACHE;

#define REPEAT2(x) {x,x}
#define REPEAT3(x) {x,x,x}
#define REPEAT4(x) {x,x,x,x}
#define REPEAT8(x) {x,x,x,x,x,x,x,x}
#define CACHE_DEF {-1,0,0}

CACHE cache = { 2 , REPEAT3(REPEAT4(CACHE_DEF)), REPEAT2(0), REPEAT8(0), REPEAT3(-1)};

CLK anlgClock = 
{ 
	~0,
	"OnboardClock", 1000.0,
	~0,
	"",             0,
	0
};

CLK dacOpt = 
{ 
	~0,
	"OnboardClock", 1000.0,
	~0,
	"",             0,
	0
};

//==============================================================================
// Global functions

static void ErrorCheck (char *errMsg, int errVal, char *errString)

{
	char outputMsg[256];
	int response;
	Fmt (outputMsg, "%s (Error = %d).\n%s\nContinue? ", errMsg, errVal, errString);
	response = ConfirmPopup ("ErrorCheck", outputMsg);
	if (response == 0)
		exit (-1);
}

static int cacheLineCreate(int port ,int idx, int mask)
{
	char buf[1024];
	int error = -1;
	cache.ports[port][idx].mask = mask;
	if(mask == 0) {
		// только чтение, не создаем таск на вывод
		Fmt(buf, "port%iR%i", port, mask);
		errChk( DAQmxCreateTask (buf, &cache.ports[port][idx].ti));
		Fmt(buf, "PXI1Slot%i/port%i", cache.slot, port);
		errChk( DAQmxCreateDIChan(cache.ports[port][idx].ti, buf, "r", DAQmx_Val_ChanForAllLines));
		cache.ports[port][idx].to = 0;
		// cache.port_out[port] = -1; не забываем выводимое значение при чтении
	}
	else if(mask == 0xff) {
		// только запись, не создаем таск на ввод
		Fmt(buf, "port%iW%i", port, mask);
		errChk( DAQmxCreateTask (buf, &cache.ports[port][idx].to));
		Fmt(buf, "PXI1Slot%i/port%i", cache.slot, port);
		errChk( DAQmxCreateDOChan(cache.ports[port][idx].to, buf, "w", DAQmx_Val_ChanForAllLines));
		cache.ports[port][idx].ti = 0;
	}
	else {
		char* ptr;
		int i;
		// создаем оба таска в соответствии с маской
		Fmt(buf, "port%iW%i", port, mask);
		errChk( DAQmxCreateTask (buf, &cache.ports[port][idx].to));
		ptr = buf;
		buf[0] = 0;
		for(i=0; i<8; i++) {
			if((mask & (1<<i)) !=0 ) {
				Fmt(ptr, "%sPXI1Slot%i/port%i/line%i", (buf[0]==0 ? "" : ","), cache.slot, port, i);
				while(*ptr)
					ptr++;
			}
		}
		errChk( DAQmxCreateDOChan(cache.ports[port][idx].to, buf, "w", DAQmx_Val_ChanForAllLines));

		Fmt(buf, "port%iR%i", port, mask);
		errChk( DAQmxCreateTask (buf, &cache.ports[port][idx].ti));
		ptr = buf;
		buf[0] = 0;
		for(i=0; i<8; i++) {
			if((mask & (1<<i)) ==0 ) {
				Fmt(ptr, "%sPXI1Slot%i/port%i/line%i", (buf[0]==0 ? "" : ","), cache.slot, port, i);
				while(*ptr)
					ptr++;
			}
		}
		errChk( DAQmxCreateDIChan(cache.ports[port][idx].ti, buf, "r", DAQmx_Val_ChanForAllLines));
	}
	return 0;
	Error:
	return error;
}

static int cacheLineClose(int port, int idx)
{
	int error = -1;
	cache.ports[port][idx].mask = -1;
	if(cache.ports[port][idx].to) {
		errChk( DAQmxStopTask(cache.ports[port][idx].to));
		errChk( DAQmxClearTask(cache.ports[port][idx].to));
		cache.ports[port][idx].to = 0;
	}
	if(cache.ports[port][idx].ti) {
		errChk( DAQmxStopTask(cache.ports[port][idx].ti));
		errChk( DAQmxClearTask(cache.ports[port][idx].ti));
		cache.ports[port][idx].ti = 0;
	}
	return 0;
	Error:
	return error;
}

static int cacheLineMove(int port, int idx)
{
	if(idx>0) {
		memmove(&cache.ports[port][1], &cache.ports[port][0], sizeof(PORT)*idx);
	}
	return 0;
}

static int cacheLineRotate(int port, int idx)
{
	int error = -1;
	PORT p;
	if(idx>0) {
		memmove(&p, &cache.ports[port][idx], sizeof(PORT));
		errChk( cacheLineMove(port, idx));
		memmove(&cache.ports[port][0], &p, sizeof(PORT));
	}
	return 0;
	Error:
	return error;
}

static int cacheLineStartTask(int port, int idx)
{
	int error = -1;
	if(cache.ports[port][idx].ti) {
		errChk( DAQmxStartTask(cache.ports[port][idx].ti));
	}
	// если есть данные для вывода выводим их
	if(cache.port_out[port] != -1 && cache.ports[port][0].mask != -1 && cache.ports[port][0].to != 0) {
		unsigned char data = cache.port_out[port];
		errChk( DAQmxWriteDigitalU8(cache.ports[port][0].to, 1, 1, 10, DAQmx_Val_GroupByChannel, &data, NULL, NULL));
	}
	return 0;
	Error:
	return error;
}

static int cacheLineStopTask(int port, int idx)
{
	int error = -1;
	if(cache.ports[port][idx].to) {
		errChk( DAQmxStopTask(cache.ports[port][idx].to));
	}
	if(cache.ports[port][idx].ti) {
		errChk( DAQmxStopTask(cache.ports[port][idx].ti));
	}
	return 0;
	Error:
	return error;
}

int tsani_portMask(int portN, unsigned char mask)
{
	TRY;
	int i;
	assertChkMsg(0 <= portN && portN < PORT_SIZE, "Порт вне диапазона");
	// ищем существующий слот с такойже маской или пустой
	for(i = 0; i < CACHE_SIZE; i++)
		if(cache.ports[portN][i].mask == mask || cache.ports[portN][i].mask == -1)
			break;
	if(i < CACHE_SIZE) {
		if(cache.ports[portN][i].mask == -1) {
			// найден пустой слот
			if(i == 0) {
				errChk( cacheLineCreate(portN, 0, mask));
				errChk( cacheLineStartTask(portN, 0));
			}
			else {
				errChk( cacheLineStopTask(portN, 0));
				errChk( cacheLineMove(portN, i));
				errChk( cacheLineCreate(portN, 0, mask));
				errChk( cacheLineStartTask(portN, 0));
			}
		}
		else {
			if(i > 0) {
				// такая маска уже существует, передвигаем её в активную позицию
				errChk( cacheLineStopTask(portN, 0));
				errChk( cacheLineRotate(portN, i));
				errChk( cacheLineStartTask(portN, 0));
			}
		}
	}
	else {
		// маска не найдена, осбождаем последний слот, сдвигаем, создаем маску
		errChk( cacheLineStopTask(portN, 0));
		errChk( cacheLineClose(portN, CACHE_SIZE-1));
		errChk( cacheLineMove(portN, CACHE_SIZE-1));
		errChk( cacheLineCreate(portN, 0, mask));
		errChk( cacheLineStartTask(portN, 0));
	}
	FINALIZE;
}

int tsani_portOut(int port, unsigned char data)
{
	TRY;
	assertChkMsg(0 <= port && port < PORT_SIZE, "Порт вне диапазона");
	cache.port_out[port] = data;
	if(cache.ports[port][0].mask != -1 && cache.ports[port][0].to != 0) {
		errChk( DAQmxWriteDigitalU8(cache.ports[port][0].to, 1, 1, 10, DAQmx_Val_GroupByChannel, &data, NULL, NULL));
	}
	FINALIZE;
}

int tsani_portIn(int port, unsigned char* data)
{
	TRY;
	uInt8 datar = 0;
	uInt8 dataw = 0;
	assertChkMsg(0 <= port && port < PORT_SIZE, "Порт вне диапазона");
	assertChkMsg(data != NULL, "NULL ptr");
	if(cache.ports[port][0].mask != -1 && cache.ports[port][0].ti != 0) {
		errChk( DAQmxReadDigitalU8(cache.ports[port][0].ti, 1,10,DAQmx_Val_GroupByChannel, &datar, 1, NULL, NULL));
	}
	if(cache.ports[port][0].mask != -1 && cache.ports[port][0].to != 0) {
		errChk( DAQmxReadDigitalU8(cache.ports[port][0].to, 1,10,DAQmx_Val_GroupByChannel, &dataw, 1, NULL, NULL));
	}
	*data = (cache.ports[port][0].mask & dataw) | ((~cache.ports[port][0].mask) & datar);
	FINALIZE;
}

static int portClose()
{
	int p, i;
	for(p = 0; p < PORT_SIZE; p++)
		for(i = 0; i < CACHE_SIZE; i++)
			cacheLineClose(p, i);
	return 0;
}

static int analogOCreate(int idx)
{
	int error = -1;
	char buf[256];
	if(cache.analogto[idx] != 0) {
		errChk( DAQmxClearTask(cache.analogto[idx]));
	}
	Fmt(buf, "analog%io", idx);
	errChk( DAQmxCreateTask (buf, &cache.analogto[idx]));
	Fmt(buf, "PXI1Slot%i/ao%i", cache.slot, idx);
	errChk( DAQmxCreateAOVoltageChan (cache.analogto[idx], buf, "", -10.0, 10.0, DAQmx_Val_Volts, ""));
	return 0;
	Error:
	return error;
}

static int analogOClose(int idx)
{
	int error = -1;
	if(cache.analogto[idx] != 0) {
		errChk( DAQmxClearTask(cache.analogto[idx]));
        cache.analogto[idx] = 0;
	}
	return 0;
	Error:
	return error;
}

static int analogICreate(int idx)
{
	char buf[256];
	int error = -1;
	if(cache.analogti[idx] != 0) {
		DAQmxClearTask(cache.analogti[idx]);
	}
	Fmt(buf, "analog%iI", idx);
	errChk( DAQmxCreateTask (buf, &cache.analogti[idx]));
	Fmt(buf, "PXI1Slot%i/ai%i", cache.slot, idx);
	errChk( DAQmxCreateAIVoltageChan (cache.analogti[idx], buf, "", DAQmx_Val_RSE, -10.0, 10.0, DAQmx_Val_Volts, ""));
	return 0;
	Error:
	return error;
}

static int analogIClose(int idx)
{
	int error = -1;
	if(cache.analogti[idx] != 0) {
		errChk( DAQmxClearTask(cache.analogti[idx]));
        cache.analogti[idx] = 0;
	}
	return 0;
	Error:
	return error;
}

int tsani_analogOut(int idx, double val)
{
	TRY;
	assertChkMsg(0 <= idx && idx < ANALOG_SIZEo, "вне диапазона");
	if(cache.analogto[idx] == 0)
		errChk( analogOCreate(idx));
	errChk( DAQmxWriteAnalogF64 (cache.analogto[idx], 1, 1, 1.0, DAQmx_Val_GroupByChannel, &val, NULL, NULL));
	FINALIZE;
}

int tsani_analogIn(int idx, double* val)
{
	TRY;
	assertChkMsg(0 <= idx && idx < ANALOG_SIZEi, "вне диапазона");
	assertChk(val != NULL);
	if(cache.analogti[idx] == 0)
		errChk( analogICreate(idx));
	errChk( DAQmxReadAnalogF64 (cache.analogti[idx], 1, 1.0, DAQmx_Val_GroupByChannel, val, 1, NULL, NULL));
	FINALIZE;
}

int tsani_analogInClk(const char source[], double frequency)
{
	//assert(MIN_FREQ < frequency && frequency < MAX_FREQ);
	if(source != NULL)
		Fmt(anlgClock.sourceCLK, "%s", source);
	else
		anlgClock.sourceCLK[0] = 0;
	anlgClock.frequency = frequency;
	anlgClock.maskCLK   = ~0;
	RETURN(0);
}

int tsani_analogInTrigger(const char source[], double level)
{
	int i, any = 0;
	if(source != NULL)
		Fmt(anlgClock.sourceTRIG, "%s", source);
	else
		anlgClock.sourceTRIG[0] = 0;
	anlgClock.level    = level;
	anlgClock.maskTRIG = ~0;
	RETURN(0);
}

int tsani_analogReadEx(int idx, double warray[], int size, int* read, double timeout)
{
	TRY;
	assertChkMsg(0 <= idx && idx < ANALOG_SIZEi, "вне диапазона");
	assertChk(warray != NULL);
	assertChk(size > 0);
	if(cache.analogti[idx] == 0)
		errChk( analogICreate(idx));
	if(anlgClock.maskCLK & (1<<idx)) {
		errChk( DAQmxCfgSampClkTiming (cache.analogti[idx], anlgClock.sourceCLK, anlgClock.frequency, DAQmx_Val_Rising, DAQmx_Val_ContSamps, size));
		anlgClock.maskCLK &= ~(1<<idx);
	}
	if(anlgClock.maskTRIG & (1<<idx)) {
		if (!CompareStrings(anlgClock.sourceTRIG, 0, "", 0, 0))
			errChk( DAQmxDisableStartTrig (cache.analogti[idx]) );
		else
			errChk( DAQmxCfgAnlgEdgeStartTrig (cache.analogti[idx], anlgClock.sourceTRIG, DAQmx_Val_RisingSlope, anlgClock.level));
		anlgClock.maskTRIG &= ~(1<<idx);
	}
	//DAQmxStartTask (cache.analogti[idx]);
	{   int rcnt = 0;
		int cnt =0;
		int readed =0;
		while(cnt < size) {
			errChk( DAQmxReadAnalogF64 (cache.analogti[idx], size, timeout, 
				DAQmx_Val_GroupByChannel, warray + cnt, size - cnt, &readed, NULL));
			cnt += readed;
			//ProcessSystemEvents();
			rcnt++;
		}
		if(read != NULL)
			*read = size;
		if(rcnt>1)
			rcnt = 0;
	}
	errChk( DAQmxStopTask (cache.analogti[idx]));
	FINALIZE;
}

int tsani_analogFetchEx(int idx, double warray[], int size, int* read, double timeout)
{
	TRY;
	assertChkMsg(0 <= idx && idx < ANALOG_SIZEi, "вне диапазона");
	assertChk(warray != NULL);
	assertChk(size > 0);
	if(anlgClock.maskFetch == 0) {
		if(cache.analogti[idx] == 0)
			errChk( analogICreate(idx));
		if(anlgClock.maskCLK & (1<<idx)) {
			errChk( DAQmxCfgSampClkTiming (cache.analogti[idx], anlgClock.sourceCLK, anlgClock.frequency, DAQmx_Val_Rising, DAQmx_Val_ContSamps, anlgClock.frequency));
			anlgClock.maskCLK &= ~(1<<idx);
		}
		if(anlgClock.maskTRIG & (1<<idx)) {
			if (!CompareStrings(anlgClock.sourceTRIG, 0, "", 0, 0))
				errChk( DAQmxDisableStartTrig (cache.analogti[idx]) );
			else
				errChk( DAQmxCfgAnlgEdgeStartTrig (cache.analogti[idx], anlgClock.sourceTRIG, DAQmx_Val_RisingSlope, anlgClock.level));
			anlgClock.maskTRIG &= ~(1<<idx);
		}
		anlgClock.maskFetch = (1<<idx);
	}
	else {
		assert(anlgClock.maskFetch == (1<<idx));
	}
	//DAQmxStartTask (cache.analogti[idx]);
	{   int rcnt = 0;
		int cnt =0;
		int readed =0;
		while(cnt < size) {
			errChk( DAQmxReadAnalogF64 (cache.analogti[idx], size, timeout, 
				DAQmx_Val_GroupByChannel, warray + cnt, size - cnt, &readed, NULL));
			cnt += readed;
			//ProcessSystemEvents();
			rcnt++;
		}
		if(read != NULL)
			*read = size;
		if(rcnt>1)
			rcnt = 0;
	}
	FINALIZE;
}

int tsani_analogFetchStatus(int idx)
{
	RETURN (anlgClock.maskFetch != 0);
}

int tsani_analogInStop(int idx)
{
	TRY;
	assertChkMsg(0 <= idx && idx < ANALOG_SIZEi, "вне дипазона");
	assertChk(anlgClock.maskFetch == (1<<idx));
	errChk( DAQmxStopTask (cache.analogti[idx]));
	anlgClock.maskFetch = 0;
	FINALIZE;
}	

int tsani_analogRead(int idx, double warray[], int size, int* read)
{
	return tsani_analogReadEx(idx, warray, size, read, 10.0);
}

int tsani_analogOutClk(const char source[], double frequency)
{
	//assert(MIN_FREQ < frequency && frequency < MAX_FREQ);
	if(source != NULL)
		Fmt(dacOpt.sourceCLK, "%s", source);
	else
		dacOpt.sourceCLK[0] = 0;
	dacOpt.frequency = frequency;
	dacOpt.maskCLK   = ~0;
	RETURN(0);
}

int tsani_analogOutTrigger(const char source[], double level)
{
	int i, any = 0;
	if(source != NULL)
		Fmt(dacOpt.sourceTRIG, "%s", source);
	else
		dacOpt.sourceTRIG[0] = 0;
	dacOpt.level    = level;
	dacOpt.maskTRIG = ~0;
	RETURN(0);
}


int tsani_analogWriteEx(int idx, double warray[], int size, double timeout)
{
	TRY;
	assertChkMsg(0 <= idx && idx < ANALOG_SIZEo, "вне диапазона");
	assertChk(warray != NULL);
	assertChk(size > 0);
	if(cache.analogto[idx] == 0)
		errChk( analogOCreate(idx));
	if(dacOpt.maskFetch == (1<<idx))
		tsani_analogOutStop(idx);
	if(dacOpt.maskCLK & (1<<idx)) {
		errChk( DAQmxCfgSampClkTiming (cache.analogto[idx], dacOpt.sourceCLK, dacOpt.frequency, DAQmx_Val_Rising, DAQmx_Val_ContSamps, size));
		dacOpt.maskCLK &= ~(1<<idx);
	}
	if(dacOpt.maskTRIG & (1<<idx)) {
		if (!CompareStrings(dacOpt.sourceTRIG, 0, "", 0, 0))
			errChk( DAQmxDisableStartTrig (cache.analogto[idx]) );
		else
			errChk( DAQmxCfgAnlgEdgeStartTrig (cache.analogto[idx], dacOpt.sourceTRIG, DAQmx_Val_RisingSlope, dacOpt.level));
		dacOpt.maskTRIG &= ~(1<<idx);
	}
	//DAQmxStartTask (cache.analogti[idx]);
	{   int rcnt = 0;
		int cnt =0;
		int written =0;
		while(cnt < size) {
			errChk( DAQmxWriteAnalogF64 (cache.analogto[idx], size - cnt, 1, timeout, 
				DAQmx_Val_GroupByChannel, warray + cnt, &written, NULL));
			cnt += written;
			//ProcessSystemEvents();
			rcnt++;
		}
		//*read = size;
		if(rcnt>1)
			rcnt = 0;
	}
	dacOpt.maskFetch = (1<<idx);
	//errChk( DAQmxStopTask (cache.analogto[idx]));
	FINALIZE;
}

int tsani_analogWrite(int idx, double warray[], int size)
{
	return tsani_analogWriteEx(idx, warray, size, 10.0);
}

int tsani_analogOutStop(int idx)
{
	TRY;
	assertChkMsg(0 <= idx && idx < ANALOG_SIZEo, "вне диапазона");
	assertChk(dacOpt.maskFetch == (1<<idx));
	errChk( DAQmxStopTask (cache.analogto[idx]));
	dacOpt.maskFetch = 0;
	FINALIZE;
}	

int tsani_ni6251Slot(int slot)
{
	cache.slot = slot;
	RETURN(0);
}

int tsani_ni6251Close(void)
{
	TRY;
	int i;
	for(i = 0; i < ANALOG_SIZEo; i++) {
		errChk( analogOClose(i));
	}
	for(i = 0; i < ANALOG_SIZEi; i++) {
		errChk( analogIClose(i));
	}
	errChk( portClose());
	FINALIZE;
}


/// HIFN  What does your function do?
/// HIPAR x/What inputs does your function expect?
/// HIRET What does your function return?
int Your_Functions_Here (int x)
{
    return x;
}

//==============================================================================
// DLL main entry-point functions

int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            if (InitCVIRTE (hinstDLL, 0, 0) == 0)
                return 0;     /* out of memory */
            break;
        case DLL_PROCESS_DETACH:
            CloseCVIRTE ();
            break;
    }
    
    return 1;
}

int __stdcall DllEntryPoint (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    /* Included for compatibility with Borland */

    return DllMain (hinstDLL, fdwReason, lpvReserved);
}
