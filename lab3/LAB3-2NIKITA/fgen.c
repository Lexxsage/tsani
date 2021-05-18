#include <formatio.h>
#include <niFgen.h>
#include <toolbox.h>
#include "tsani.h"
#include "tsani_error.h"

#define processError(error) \
	if(error != 0) { \
		ViStatus errorCode; \
		niFgen_GetError (vifgen, &errorCode, TSANI_ERROR_MESSAGE_SIZE, TSANI_ERROR_MESSAGE_BUFFER); \
	} \
	else {\
		error = -1; \
	} \
	tsani_SetLastError(error);

static ViSession vifgen;
static int fgen_slot = 6;
static int fgen_started = 0;
static int fgen_mode = NIFGEN_VAL_OUTPUT_FUNC;
static ViInt32 fgen_freqlist = 0;

static ViStatus fgen_init0()
{
	fgen_started = 0;
	fgen_mode = NIFGEN_VAL_OUTPUT_FUNC;
	fgen_freqlist = 0;
	return 0;
}
static ViStatus fgen_init1()
{
	char buf[128];

	if(vifgen == 0) {
		tsani_fgenClose();
		fgen_init0();
		Fmt(buf,"PXI1Slot%i", fgen_slot);
		return niFgen_init (buf, VI_TRUE, VI_TRUE, &vifgen);
	}
	return 0;
}

static ViStatus fgen_mode1(ViInt32 outputMode)
{
	int error = 0;
	if(fgen_mode != outputMode) {
		errChk(tsani_fgenStop());
		if(fgen_freqlist != 0 && outputMode != NIFGEN_VAL_OUTPUT_FREQ_LIST) {
			errChk(niFgen_ClearFreqList (vifgen, fgen_freqlist));
			fgen_freqlist = 0;
		}
		errChk(niFgen_ConfigureOutputMode (vifgen, outputMode));
		fgen_mode = outputMode;
	}
Error:
	return error;
}

ViStatus tsani_fgenSlot(int slot)
{
	TRY;
	char buf[128];

	tsani_fgenClose();
	fgen_slot = slot;
	fgen_init0();
	Fmt(buf,"PXI1Slot%i",slot);
	error = niFgen_init (buf, VI_TRUE, VI_TRUE, &vifgen);
	fgen_started = 0;
	RETURN(error);
}

ViStatus tsani_fgenClose()
{
	int error = -1;
	if(vifgen == 0) {
		fgen_freqlist = 0;
		RETURN(0);
	}
	if(fgen_freqlist != 0) {
		errChk(niFgen_ClearFreqList (vifgen, fgen_freqlist));
		fgen_freqlist = 0;
	}
	error = niFgen_close(vifgen);
	vifgen = 0;
	fgen_started = 0;
	RETURN(error);
Error:
	vifgen = 0;
	fgen_started = 0;
	processError(error);
	RETURN(error);
}

ViStatus tsani_fgenGenerateFreqList(int FreqSteps, float FreqMin, float FreqMax,
	float StepDuration, double pFreqList[], double pTimeIntervalList[])
{
	TRY;
	int i;
	assertChk(FreqSteps >= 1);
	// TODO assertChk(FreqMin);
	// TODO assertChk(FreqMax);
	// TODO assertChk(StepDuration);
	assertChk(pFreqList != NULL);
	assertChk(pTimeIntervalList != NULL);
	if(FreqSteps == 1) {
		pTimeIntervalList[0] = StepDuration;
		pFreqList[0] = (FreqMin + FreqMax) / 2;
		RETURN(0);
	}

	for(i = 0; i < FreqSteps; i++) {
		pTimeIntervalList[i] = StepDuration;
		pFreqList[i] = FreqMin + (FreqMax - FreqMin) * i / (FreqSteps - 1 - 0);
	}
	
	FINALIZE;
}

static ViStatus fgenStart1(void)
{
	TRY;
	if(fgen_started == 0) {
		errChk(niFgen_ConfigureOutputEnabled (vifgen, "0", VI_TRUE));
		errChk(niFgen_InitiateGeneration (vifgen));
		fgen_started = 1;
	}
	else
		error = 0;
	return error;
	CATCH;
}

ViStatus tsani_fgenStart(int WaveFormType, double Amplitude,
	int ListSize, double pFreqList[], double pTimeIntervalList[])
{
	TRY;
	fgen_init1();
	errChk(tsani_fgenStop());
	if(1) {
		ViInt32 maximumNumberOfFreqLists;
		ViInt32 minimumFrequencyListLength;
		ViInt32 maximumFrequencyListLength;
		ViReal64 minimumFrequencyListDuration;
		ViReal64 maximumFrequencyListDuration;
		ViReal64 frequencyListDurationQuantum;
		niFgen_QueryFreqListCapabilities (vifgen,
			&maximumNumberOfFreqLists,
			&minimumFrequencyListLength,
			&maximumFrequencyListLength,
			&minimumFrequencyListDuration,
			&maximumFrequencyListDuration,
			&frequencyListDurationQuantum);
		error = error;
	}

	errChk(fgen_mode1(NIFGEN_VAL_OUTPUT_FREQ_LIST));
	if(fgen_freqlist != 0) {
		errChk(niFgen_ClearFreqList (vifgen, fgen_freqlist));
		fgen_freqlist = 0;
	}
	errChk(niFgen_CreateFreqList (vifgen, WaveFormType,
		ListSize, pFreqList, pTimeIntervalList, &fgen_freqlist));
	errChk(niFgen_ConfigureFreqList (vifgen, "0", fgen_freqlist, Amplitude, 0, 0));
	//errChk(niFgen_ExportSignal (vifgen, NIFGEN_VAL_SYNC_OUT, "", ""));
	//errChk(niFgen_ConfigureTriggerMode (vifgen, "0", NIFGEN_VAL_SINGLE));
	//errChk(niFgen_ExportSignal (vifgen, NIFGEN_VAL_STARTED_EVENT, "", "PFI0"));
	//errChk(niFgen_RouteSignalOut (vifgen, "0", NIFGEN_VAL_MARKER, NIFGEN_VAL_PFI_0));

	errChk(fgenStart1());
	FINALIZE;
}

ViStatus tsani_fgenExportTrig(ViConstString outputTerminal)
{
	TRY;
	int is_started;
	errChk(fgen_init1());
	is_started = fgen_started;
	errChk(tsani_fgenStop());
	errChk(niFgen_ExportSignal (vifgen, NIFGEN_VAL_SYNC_OUT, "", ""));
	//errChk(niFgen_ConfigureTriggerMode (vifgen, "0", NIFGEN_VAL_SINGLE));
	errChk(niFgen_ExportSignal (vifgen, NIFGEN_VAL_STARTED_EVENT, "", ""));
	errChk(niFgen_ExportSignal (vifgen, NIFGEN_VAL_STARTED_EVENT, "", outputTerminal));
	if(is_started)
		errChk(fgenStart1());

	FINALIZE;
}

ViStatus tsani_fgenStop(void)
{
	TRY;
	errChk(fgen_init1());
	if(fgen_started) {
		errChk(niFgen_ConfigureOutputEnabled (vifgen, "0", VI_FALSE));
		errChk(niFgen_AbortGeneration (vifgen));
	}
	fgen_started = 0;
	FINALIZE;
}

ViStatus tsani_fgenStartStandartWaveForm(float Amplitude, float Frequency, int WaveFormType)
{
	TRY;
	errChk(fgen_init1());
	
	errChk(fgen_mode1(NIFGEN_VAL_OUTPUT_FUNC));
	errChk(niFgen_ConfigureStandardWaveform (vifgen, "0", WaveFormType, Amplitude, 0, Frequency, 0));
	errChk(fgenStart1());
	FINALIZE;
}

ViStatus tsani_fgenFrequency(float NewFrequency)
{
	TRY;
	fgen_init1();
	if(fgen_started)
		errChk(niFgen_ConfigureFrequency(vifgen, "0", NewFrequency));
	else
		error = 0;
	FINALIZE;
}

ViStatus tsani_fgenAmplitude(float NewAmplitude)
{
	TRY;
	int is_started = fgen_started;
	errChk(fgen_init1());
	if(is_started)
		errChk(tsani_fgenStop());
	errChk(niFgen_ConfigureAmplitude(vifgen, "0", NewAmplitude));
	if(is_started)
		errChk(fgenStart1());

	FINALIZE;
}
