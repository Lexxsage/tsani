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
#define  PANEL_ADC_BUTTON_2               2       /* callback function: ADC_button */
#define  PANEL_ADC_BUTTON                 3       /* callback function: ADC_button */
#define  PANEL_GRAPH2                     4
#define  PANEL_GRAPH                      5
#define  PANEL_TEXTMSG                    6
#define  PANEL_TextMessage2               7
#define  PANEL_TEXTMSG_3                  8
#define  PANEL_TextMessage4               9
#define  PANEL_NUMERIC_4                  10
#define  PANEL_NUMERIC_3                  11
#define  PANEL_NUMERIC_2                  12
#define  PANEL_NUMERIC                    13
#define  PANEL_GRAPH3                     14


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK ADC_button(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
