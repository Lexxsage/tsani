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
#define  PANEL_SUBADDRESS_2               2
#define  PANEL_DAC_CODE                   3       /* callback function: DAC */
#define  PANEL_DAC_VOLT                   4       /* callback function: DAC */
#define  PANEL_Vin2_code                  5
#define  PANEL_Vin1_code                  6
#define  PANEL_ADDRESS_2                  7
#define  PANEL_VIN2_VOLT                  8
#define  PANEL_DATA_2                     9
#define  PANEL_VIN1_VOLT                  10
#define  PANEL_SUBADDRESS                 11
#define  PANEL_ADDRESS                    12
#define  PANEL_DATA                       13
#define  PANEL_COMMANDBUTTON_2            14      /* callback function: DoRead */
#define  PANEL_COMMANDBUTTON              15      /* callback function: DoWrite */
#define  PANEL_ADC                        16      /* callback function: ADC */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK ADC(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DAC(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DoRead(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DoWrite(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
