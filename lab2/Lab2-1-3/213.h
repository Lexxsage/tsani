/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2008. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: panelCB */
#define  PANEL_READ0                      2
#define  PANEL_READ1                      3
#define  PANEL_READ2                      4
#define  PANEL_NUMAD                      5
#define  PANEL_TIMER                      6       /* callback function: callTimer */
#define  PANEL_LTIMER                     7       /* callback function: callLTimer */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK callLTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK callTimer(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
