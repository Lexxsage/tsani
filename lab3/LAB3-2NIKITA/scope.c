#include <formatio.h>
#include <niScope.h>
#include <toolbox.h>
#include "tsani.h"
#include "tsani_error.h"

// niScope_GetErrorMessage
//ViStatus _VI_FUNC niScope_GetErrorMessage (ViSession vi, 
//                                           ViStatus errorCode,
//                                           ViInt32 bufferSize,
//                                           ViChar errorMessage[]);
#define processError(error) \
	if(error != 0){ \
		niScope_GetErrorMessage (scope, error, TSANI_ERROR_MESSAGE_SIZE, TSANI_ERROR_MESSAGE_BUFFER); \
	} \
	else {\
		error = -1; \
	} \
	tsani_SetLastError(error);
	
static int scope_slot=7;
static ViSession scope;
static int scope_numSamples = -1;

static ViStatus scope_init0(void)
{
	scope = 0;
	return 0;
}

static ViStatus scope_init1(void)
{
	int error = 0;
	char buf[64];
	if(scope == 0) {
		Fmt(buf,"PXI1Slot%i",scope_slot);
		errChk(niScope_init (buf, NISCOPE_VAL_TRUE, NISCOPE_VAL_TRUE, &scope));
	}
	else
		error = 0;
Error:
	return error;
}

ViStatus tsani_scopeSlot(int slot)
{
	TRY;
	scope_slot = slot;
	errChk(scope_init1());

	FINALIZE;
}

ViStatus tsani_scopeClose()
{
	TRY;
	if(scope != 0) {
		errChk(niScope_close (scope));
	}
	scope_init0();

	FINALIZE;
}

ViStatus tsani_scopeStartRead(const char* ChanList, double wfm[], int numSamples)
{
	TRY;
	struct niScope_wfmInfo wfmInfo[8];
	int recordLength;
	ViInt32 numWfms;
	ViInt32 measWfmSize;
	ViReal64 actualSampleRate;
	assertChkMsg(scope_numSamples == numSamples, "количество выборок должно совпадать с зарезервированным");
	errChk(scope_init1());
	//errChk(niScope_SetAttributeViInt32(scope,VI_NULL,NISCOPE_ATTR_HORZ_MIN_NUM_PTS, numSamples));
	errChk(niScope_ActualRecordLength (scope, &recordLength));
	errChk(niScope_ActualNumWfms (scope, ChanList, &numWfms));
	errChk(niScope_ActualMeasWfmSize(scope, NISCOPE_VAL_NO_MEASUREMENT, &measWfmSize));
	errChk(niScope_SampleRate (scope, &actualSampleRate));

	errChk(niScope_Read (scope, ChanList, 1, numSamples, wfm, wfmInfo));

	FINALIZE;
}

ViStatus tsani_scopeStart(int numSamples)
{
	TRY;
	assertChkMsg(scope_numSamples == numSamples, "количество выборок должно совпадать с зарезервированным");
	errChk(scope_init1());
	// NISCOPE_ATTR_HORZ_MIN_NUM_PTS NISCOPE_ATTR_HORZ_RECORD_LENGTH
	//errChk(niScope_SetAttributeViInt32(scope,VI_NULL,NISCOPE_ATTR_HORZ_MIN_NUM_PTS, numSamples));
	errChk(niScope_InitiateAcquisition (scope));

	FINALIZE;
}

ViStatus tsani_scopeStartContinius(int numSamples, int divisor)
{
	TRY;
	errChk(scope_init1());
	// NISCOPE_ATTR_HORZ_MIN_NUM_PTS NISCOPE_ATTR_HORZ_RECORD_LENGTH
	errChk(niScope_SetAttributeViInt32(scope, VI_NULL, NISCOPE_ATTR_FETCH_RELATIVE_TO, NISCOPE_VAL_READ_POINTER));
	errChk(niScope_SetAttributeViInt32(scope, VI_NULL, NISCOPE_ATTR_SAMP_CLK_TIMEBASE_DIV, divisor));
	errChk(niScope_ConfigureTriggerSoftware(scope, 0.0, 0.0));
	errChk(niScope_SetAttributeViInt32(scope,VI_NULL,NISCOPE_ATTR_HORZ_MIN_NUM_PTS, numSamples));
	errChk(niScope_InitiateAcquisition (scope));

	FINALIZE;
}

ViStatus tsani_scopeStatus()
{
	TRY;
	ViInt32 status;
	if(scope == 0)
		RETURN(SCOPE_UNKNOWN);
	errChk(niScope_AcquisitionStatus (scope, &status));
	switch(status) {
		case NISCOPE_VAL_ACQ_COMPLETE:
			RETURN(SCOPE_COMPLETE);
		case NISCOPE_VAL_ACQ_IN_PROGRESS:
			RETURN(SCOPE_IN_PROGRESS);
		default: //NISCOPE_VAL_ACQ_STATUS_UNKNOWN
			RETURN(SCOPE_UNKNOWN);
	}

	FINALIZE;
}

ViStatus tsani_scopeStop()
{
	TRY;
	errChk(niScope_Abort (scope));

	FINALIZE;
}

ViStatus tsani_scopeFetch(const char* channelList, double wfm[], int numSamples, int* numSamplesReaded )
{
	TRY;
	struct niScope_wfmInfo wfmInfo[8];
	errChk(scope_init1());
	errChk(niScope_Fetch (scope, channelList, 3.0, numSamples, wfm, wfmInfo));
	if(numSamplesReaded != NULL)
		*numSamplesReaded = wfmInfo[0].actualSamples;

	FINALIZE;
}

ViStatus tsani_scopeFrequency(const char* sourceCLK, double frequency, int numSamples)
{
	TRY;
	errChk(scope_init1());
	if(sourceCLK == NULL || *sourceCLK == '\0') {
		int recordLength;
		scope_numSamples = numSamples;
		errChk(niScope_ConfigureHorizontalTiming (scope, frequency, numSamples, 0.0, 1, NISCOPE_VAL_TRUE));
		errChk(niScope_ActualRecordLength (scope, &recordLength));
		recordLength = recordLength;
	}
	else {
		scope_numSamples = numSamples;
		errChk(niScope_ConfigureHorizontalTiming (scope, frequency, numSamples, 0.0, 1, NISCOPE_VAL_FALSE));
		errChk(niScope_SetAttributeViString(scope, VI_NULL,NISCOPE_ATTR_SAMP_CLK_TIMEBASE_SRC, sourceCLK));
		errChk(niScope_SetAttributeViReal64(scope, VI_NULL,NISCOPE_ATTR_SAMP_CLK_TIMEBASE_RATE, frequency));
	}

	FINALIZE;
}

ViStatus tsani_scopeGetFrequency(double* actualSampleRate)
{
	TRY;
	errChk(scope_init1());
	errChk(niScope_SampleRate (scope, actualSampleRate));

	FINALIZE;
}

ViStatus tsani_scopeVertical(const char* ChanList, float range, int impedance)
{
	TRY;
	errChk(scope_init1());
	errChk(niScope_ConfigureVertical (scope, ChanList, range, 0.0, NISCOPE_VAL_DC, 1, NISCOPE_VAL_TRUE));
	errChk(niScope_ConfigureChanCharacteristics (scope, ChanList, impedance, NISCOPE_VAL_BANDWIDTH_DEVICE_DEFAULT));
		
	FINALIZE;
}

ViStatus tsani_scopeTrigger(const char* triggerSource, float level, int slope)
{
	TRY;
	errChk(scope_init1());
	if(triggerSource == NULL || *triggerSource == '\0') {
		errChk(niScope_ConfigureTriggerImmediate (scope));
	}
	else {
		errChk(niScope_ConfigureTriggerEdge (scope, triggerSource, level, slope, NISCOPE_VAL_DC, 0.0, 0.0));
	}

	FINALIZE;
}

