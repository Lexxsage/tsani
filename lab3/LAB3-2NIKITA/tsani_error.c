#include <ansi_c.h>
#include <utility.h>
#include"tsani_error.h"
#include"tsani.h"

static ViStatus status = 0;
static char message[TSANI_ERROR_MESSAGE_SIZE];

ViStatus tsani_SetLastError(ViStatus _status)
{
	ViStatus prev = status;
	status = _status;
	return prev;
}

ViStatus tsani_GetLastError(void)
{
	return status;
}

char* tsani_GetLastErrorBuffer(void)
{
	return message;
}

ViStatus tsani_PrintLastError(void)
{
	ErrorPrintf("Error = %i, %s\n", status, message);
	return status;
}
