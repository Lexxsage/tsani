   //==============================================================================
//
// Title:       tsani
// Purpose:     A short description of the interface.
//
// Created on:  03.04.2014 at 16:04:44 by UserName.
// Copyright:   UserOrg. All Rights Reserved.
//
//==============================================================================

#ifndef __tsani_H__
#define __tsani_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include <cvidef.h>
#include <visatype.h>
#include <utility.h>

//==============================================================================
// Constants

//==============================================================================
// Types

//==============================================================================
// External variables

//==============================================================================
// Global functions

ViStatus tsani_GetLastError(void);
char* tsani_GetLastErrorBuffer(void);
#define tsani_wrap(fCall)  (((tsani_##fCall < 0)? ErrorPrintf("Error = %i, %s\n", tsani_GetLastError(), tsani_GetLastErrorBuffer()),Breakpoint(), 0 : 0), tsani_GetLastError() )
					  
int tsani_portMask(int portN, unsigned char mask);
int tsani_portOut(int portN, unsigned char data);
int tsani_portIn(int portN, unsigned char* data);
#define portMask(portN, mask) tsani_wrap(portMask(portN, mask))
#define portOut(portN, data)  tsani_wrap(portOut(portN, data))
#define portIn(portN, data)   tsani_wrap(portIn(portN, data))

int tsani_analogOut(int idx, double val);
int tsani_analogIn(int idx, double* val);
#define   analogOut(idx, val)                 tsani_wrap(analogOut(idx, val))
#define   analogIn(idx, val)                  tsani_wrap(analogIn(idx, val))

int tsani_analogInClk(const char source[], double frequency);
int tsani_analogOutClk(const char source[], double frequency);
#define   analogInClk(source, frequency)      tsani_wrap(analogInClk(source, frequency))
#define   analogOutClk(source, frequency)     tsani_wrap(analogOutClk(source, frequency))

int tsani_analogWrite(int idx, double warray[], int size);
int tsani_analogWriteEx(int idx, double warray[], int size, double timeout);
#define   analogWrite(idx, warray, size)                 tsani_wrap(analogWrite(idx, warray, size))
#define   analogWriteEx(idx, warray, size, timeout)      tsani_wrap(analogWriteEx(idx, warray, size, timeout))

int tsani_analogRead(int idx, double warray[], int size, int* read);
int tsani_analogReadEx(int idx, double warray[], int size, int* read, double timeout);
#define   analogRead(idx, warray, size, read)            tsani_wrap(analogRead(idx, warray, size, read))
#define   analogReadEx(idx, warray, size, read, timeout) tsani_wrap(analogReadEx(idx, warray, size, read, timeout))

int tsani_analogInTrigger(const char source[], double level);
int tsani_analogOutTrigger(const char source[], double level);
#define   analogInTrigger(source, level)                      tsani_wrap(analogInTrigger(source, level))
#define   analogOutTrigger(source, level)                     tsani_wrap(analogOutTrigger(source, level))

int tsani_analogFetchEx(int idx, double warray[], int size, int* read, double timeout);
int tsani_analogFetchStatus(int idx);
#define   analogFetchEx(idx, warray, size, read, timeout) tsani_wrap(analogFetchEx(idx, warray, size, read, timeout))
#define   analogFetchStatus(idx)                          tsani_wrap(analogFetchStatus(idx))

int tsani_analogInStop(int idx);
int tsani_analogOutStop(int idx);
#define   analogInStop(idx)   tsani_wrap(tsani_analogInStop(idx))
#define   analogOutStop(idx)  tsani_wrap(tsani_analogOutStop(idx))
	
int tsani_ni6251Slot(int slot);
int tsani_ni6251Close(void);
#define   ni6251Slot(slot)     tsani_wrap(ni6251Slot(slot))
#define   ni6251Close()        tsani_wrap(ni6251Close())

#define FGEN_SINE                     (1L)     
#define FGEN_SQUARE                   (2L)
#define FGEN_TRIANGLE                 (3L)
#define FGEN_RAMP_UP                  (4L)
#define FGEN_RAMP_DOWN                (5L)
#define FGEN_VAL_WFM_DC               (6L)
#define FGEN_NOISE                    (101L)

ViStatus tsani_fgenSlot(int slot);
#define        fgenSlot(slot) tsani_wrap(fgenSlot(slot))
ViStatus tsani_fgenClose(void);
#define        fgenClose() tsani_wrap(fgenClose())

ViStatus tsani_fgenStartStandartWaveForm(float Amplitude, float Frequency, int WaveFormType);
#define        fgenStartStandartWaveForm(Amplitude, Frequency, WaveFormType) tsani_wrap(fgenStartStandartWaveForm(Amplitude, Frequency, WaveFormType))
ViStatus tsani_fgenGenerateFreqList(int FreqSteps, float FreqMin, float FreqMax,
	float StepDuration, double pFreqList[], double pTimeIntervalList[]);
#define        fgenGenerateFreqList(FreqSteps, FreqMin, FreqMax, StepDuration, pFreqList, pTimeIntervalList) tsani_wrap(fgenGenerateFreqList(FreqSteps, FreqMin, FreqMax, StepDuration, pFreqList, pTimeIntervalList))
ViStatus tsani_fgenExportTrig(ViConstString outputTerminal);
#define        fgenExportTrig(outputTerminal) tsani_wrap(fgenExportTrig(outputTerminal))

ViStatus tsani_fgenStart(int WaveFormType, double Amplitude,
	int ListSize, double pFreqList[], double pTimeIntervalList[]);
#define        fgenStart(WaveFormType, Amplitude, ListSize, pFreqList, pTimeIntervalList) tsani_wrap(fgenStart(WaveFormType, Amplitude, ListSize, pFreqList, pTimeIntervalList))
ViStatus tsani_fgenStop(void);
#define        fgenStop() tsani_wrap(fgenStop())

ViStatus tsani_fgenFrequency(float NewFrequency);
#define        fgenFrequency(NewFrequency) tsani_wrap(fgenFrequency(NewFrequency))
ViStatus tsani_fgenAmplitude(float NewAmplitude);
#define        fgenAmplitude(NewAmplitude) tsani_wrap(fgenAmplitude(NewAmplitude))

ViStatus tsani_scopeSlot(int slot);
#define        scopeSlot(slot) tsani_wrap(scopeSlot(slot))
ViStatus tsani_scopeClose(void);
#define        scopeClose() tsani_wrap(scopeClose())
ViStatus tsani_scopeFrequency(const char* sourceCLK, double frequency, int numSamples);
#define        scopeFrequency(sourceCLK, frequency, numSamples) tsani_wrap(scopeFrequency(sourceCLK, frequency, numSamples))
ViStatus tsani_scopeGetFrequency(double* frequency);
#define        scopeGetFrequency(frequency) tsani_wrap(scopeGetFrequency(frequency))

#define SCOPE_1_MEG_OHM 1000000.0
#define SCOPE_50_OHM    50.0

ViStatus tsani_scopeVertical(const char* ChanList, float range, int impedance);
#define        scopeVertical(ChanList, range, impedance) tsani_wrap(scopeVertical(ChanList, range, impedance))

#define SCOPE_POSITIVE                                   (1L)
#define SCOPE_NEGATIVE                                   (0L)

ViStatus tsani_scopeTrigger(const char* triggerSource, float level, int slope);
#define        scopeTrigger(triggerSource, level, slope) tsani_wrap(scopeTrigger(triggerSource, level, slope))
ViStatus tsani_scopeStart(int numSamples);
#define        scopeStart(numSamples) tsani_wrap(scopeStart(numSamples))
ViStatus tsani_scopeStartContinius(int numSamples, int divisor);
#define        scopeStartContinius(numSamples, divisor) tsani_wrap(scopeStartContinius(numSamples, divisor))

#define SCOPE_COMPLETE 1
#define SCOPE_IN_PROGRESS 0
#define SCOPE_UNKNOWN -1 // error

ViStatus tsani_scopeStatus(void);
#define        scopeStatus() tsani_wrap(scopeStatus())
ViStatus tsani_scopeStop(void);
#define        scopeStop() tsani_wrap(scopeStop())
ViStatus tsani_scopeStartRead(const char* ChanList, double wfm[], int numSamples);
#define        scopeStartRead(ChanList, wfm, numSamples) tsani_wrap(scopeStartRead(ChanList, wfm, numSamples))
ViStatus tsani_scopeFetch(const char* ChanList, double wfm[], int numSamples, int* numSamplesReaded);
#define        scopeFetch(ChanList, wfm, numSamples, numSamplesReaded) tsani_wrap(scopeFetch(ChanList, wfm, numSamples, numSamplesReaded))
//ViStatus scopeOutput(int channel, int enable);




#ifdef __cplusplus
    }
#endif

#endif  /* ndef __tsani_H__ */
