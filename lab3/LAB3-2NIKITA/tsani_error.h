#ifndef __tsani_error_H__
#define __tsani_error_H__

#ifdef __cplusplus
    extern "C" {
#endif

//==============================================================================
// Include files

#include <cvidef.h>
#include <visatype.h>

//==============================================================================
// Constants

//==============================================================================
// Types

#define assertChk(exp) \
	if (error = (exp), !error) \
	{ \
		Fmt(TSANI_ERROR_MESSAGE_BUFFER, "ASSERT ERORR: %s, line %i: %s", __FILE__, __LINE__, #exp); \
		goto Error; \
	} else

#define assertChkMsg(exp, msg) \
	if (error = (exp), !error) \
	{ \
		Fmt(TSANI_ERROR_MESSAGE_BUFFER, "ASSERT ERORR: %s, line %i: %s: %s", __FILE__, __LINE__, #exp, msg); \
		goto Error; \
	} else

#define TRY 	int error = -1

#define RETURN(val) do{ tsani_SetLastError(val); return (val); } while(0)

#define CATCH \
	Error: \
	processError(error); \
	return error

#define FINALIZE \
		RETURN(error); \
		CATCH

//==============================================================================
// External variables

//==============================================================================
// Global functions

#define TSANI_ERROR_MESSAGE_SIZE 1024
#define TSANI_ERROR_MESSAGE_BUFFER tsani_GetLastErrorBuffer()
		
ViStatus tsani_SetLastError(ViStatus status);
char* tsani_GetLastErrorBuffer(void);
ViStatus tsani_GetLastError(void);

#ifdef __cplusplus
    }
#endif

#endif  /* ndef __tsani_error_H__ */
