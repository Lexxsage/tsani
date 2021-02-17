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
#define  PANEL_GRAPH                      2
#define  PANEL_Amplitude                  3       /* callback function: ChangeAmplitude */
#define  PANEL_Freq                       4       /* callback function: ChangeFrequence */
#define  PANEL_Phi                        5       /* callback function: ChangePhi */
#define  PANEL_Noise                      6       /* callback function: ChangeNoise */
#define  PANEL_Save                       7       /* callback function: SaveGraph */
#define  PANEL_Power                      8
#define  PANEL_Timer                      9       /* callback function: Timer1 */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK ChangeAmplitude(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ChangeFrequence(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ChangeNoise(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ChangePhi(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SaveGraph(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Timer1(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
