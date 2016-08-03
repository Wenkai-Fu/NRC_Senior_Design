// Adapted from SEGGER's DIALOG_SliderColor.c; see LICENSE.

#include <stddef.h>
#include "GUI.h"
#include "DIALOG.h"
#include "main.h"

#define RECOMMENDED_MEMORY (1024L * 15)

extern Motor motor;
extern Encoder encoder;

/*********************************************************************
 * Function description
 *   This table conatins the info required to create the dialog.
 *   It has been created manually, but could also be created by a GUI-builder.
 */
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
{ FRAMEWIN_CreateIndirect, "Motor Control", 0, 0, 0, 480, 272, 0 },
{ TEXT_CreateIndirect, "Position", 0, 250, 20, 80, 20, TEXT_CF_LEFT },
{ TEXT_CreateIndirect, "Desired Position", 0, 250, 40, 80, 20, TEXT_CF_LEFT },
{ TEXT_CreateIndirect, "Pos Error", 0, 250, 60, 80, 20, TEXT_CF_LEFT },
{ TEXT_CreateIndirect, "Encoder Dir", 0, 250, 80, 80, 20, TEXT_CF_LEFT },
{ TEXT_CreateIndirect, "Motor Dir", 0, 250, 100, 80, 20, TEXT_CF_LEFT },
{ TEXT_CreateIndirect, "Pos Error", 0, 250, 120, 80, 20, TEXT_CF_LEFT },
{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT0, 410, 20, 45, 20, 0, 0 },
{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT1, 410, 40, 45, 20, 0, 3 },
{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT2, 410, 60, 45, 20, 0, 3 },
{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT3, 410, 80, 45, 20, 0, 3 },
{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT4, 410, 100, 45, 20, 0, 3 },
{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT5, 410, 120, 45, 20, 0, 3 },
{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT6, 360, 20, 45, 20, 0, 3 },
{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT7, 360, 40, 45, 20, 0, 3 },
{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT8, 360, 60, 45, 20, 0, 3 },
{ BUTTON_CreateIndirect, "Forward", GUI_ID_BUTTON4, 10, 170, 80, 30 },
{ BUTTON_CreateIndirect, "Reverse", GUI_ID_BUTTON5, 10, 210, 80, 30 },
{ BUTTON_CreateIndirect, "Azimuthal", GUI_ID_BUTTON0, 110, 200, 80, 40 },
{ BUTTON_CreateIndirect, "Vertical", GUI_ID_BUTTON1, 200, 200, 80, 40 },
{ BUTTON_CreateIndirect, "Claw", GUI_ID_BUTTON2, 290, 200, 80, 40 },
{ BUTTON_CreateIndirect, "Stop", GUI_ID_BUTTON3, 380, 200, 80, 40 },
{ BUTTON_CreateIndirect, "Azim Inc", GUI_ID_BUTTON6, 20, 100, 100, 40 },
{ BUTTON_CreateIndirect, "Vert Inc", GUI_ID_BUTTON7, 140, 100, 100, 40 },
{ PROGBAR_CreateIndirect, "", GUI_ID_PROGBAR0, 90, 160, 100, 18 },
{ SPINBOX_CreateIndirect, NULL, GUI_ID_SPINBOX0, 20, 25, 100, 60, 0, 0, 0 },
{ SPINBOX_CreateIndirect, NULL, GUI_ID_SPINBOX1, 140, 25, 100, 60, 0, 0, 0 },

};

// Dialog handles
WM_HWIN _hDialogMain;

//----------------------------------------------------------------------------//
static void _SetProgbarValue(int Id, I32 Value)
{
	WM_HWIN hItem;
	hItem = WM_GetDialogItem(_hDialogMain, Id);
	PROGBAR_SetValue(hItem, Value);
}

//----------------------------------------------------------------------------//
static void _cbCallback(WM_MESSAGE * pMsg)
{
	WM_HWIN hDlg;
	EDIT_Handle hEdit;
	WM_HWIN hItem;
	int NCode;
	int Id;

	hDlg = pMsg->hWin;
	switch (pMsg->MsgId)
	{
	case WM_INIT_DIALOG:

		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SPINBOX0);
		SPINBOX_SetSkin(hItem, SPINBOX_SKIN_FLEX);
		hEdit = SPINBOX_GetEditHandle(hItem);
		EDIT_SetFloatMode(hEdit, 5.0, -100.0, 100.0, 0, 0);

		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SPINBOX1);
		SPINBOX_SetSkin(hItem, SPINBOX_SKIN_FLEX);
		hEdit = SPINBOX_GetEditHandle(hItem);
		EDIT_SetFloatMode(hEdit, 5.0, -100.0, 100.0, 0, 0);

		/*      SPINBOX_SetEditMode(hItem, SPINBOX_EM_EDIT);
		 SPINBOX_SetRange(hItem, 0, 100);*/

		//
		// Init progress bars
		//
		hItem = WM_GetDialogItem(hDlg, GUI_ID_PROGBAR0);
		WIDGET_SetEffect(hItem, &WIDGET_Effect_3D);
		_SetProgbarValue(GUI_ID_PROGBAR0, 0);

		//
		// Init edit widgets
		//
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT0);
		EDIT_SetFloatMode(hItem, 0.0, -999.0, 999.0, 2, 0);
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT1);
		EDIT_SetFloatMode(hItem, 0.0, -999.0, 999.0, 2, 0);
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT2);
		EDIT_SetFloatMode(hItem, 0.0, -999.0, 999.0, 2, 0);
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT3);
		EDIT_SetFloatMode(hItem, 0.0, -999.0, 999.0, 2, 0);
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT4);
		EDIT_SetFloatMode(hItem, 0.0, -999.0, 999.0, 2, 0);
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT5);
		EDIT_SetFloatMode(hItem, 0.0, -999.0, 999.0, 2, 0);
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT6);
		EDIT_SetFloatMode(hItem, 0.0, -999.0, 999.0, 2, 0);
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT7);
		EDIT_SetFloatMode(hItem, 0.0, -999.0, 999.0, 2, 0);
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT8);
		EDIT_SetFloatMode(hItem, 0.0, -999.0, 999.0, 2, 0);

		break;
	case WM_KEY:
		switch (((WM_KEY_INFO*) (pMsg->Data.p))->Key)
		{
		case GUI_KEY_ESCAPE:
			;
			break;
		case GUI_KEY_ENTER:
			GUI_EndDialog(hDlg, 0);
			break;
		}
		break;
	case WM_NOTIFY_PARENT:
		Id = WM_GetId(pMsg->hWinSrc);      // Id of widget
		NCode = pMsg->Data.v;                 // Notification code

		switch (NCode)
		{
		case WM_NOTIFICATION_RELEASED:      // React only if released
			if (Id == GUI_ID_OK)
			{            // OK Button
				GUI_EndDialog(hDlg, 0);
			}
			if (Id == GUI_ID_BUTTON3)
			{        // Stop Button
				motor.setDuty(Azimuthal_Motor, 0);
				motor.setDuty(Vertical_Motor, 0);
				motor.setDuty(Claw_Motor, 0);

			}
			if (Id == GUI_ID_BUTTON0)
			{       // Azimuthal Button
				motor.setEnable(Azimuthal_Motor, true);
				encoder.setCount(encoder.getCount());	// Resets the count to 0 everytime
			}
			if (Id == GUI_ID_BUTTON1)
			{        // Vertical Button
				motor.setEnable(Vertical_Motor, true);
				encoder.setCount(VerticalCount);
				DeltaVerticalCount = VerticalCount;
			}
			if (Id == GUI_ID_BUTTON2)
			{        // Claw Button
				motor.setEnable(Claw_Motor, true);
				encoder.setCount(ClawCount);
			}
			if (Id == GUI_ID_BUTTON4)
			{        // Forward Button
				motor.setDirection(false);
			}
			if (Id == GUI_ID_BUTTON5)
			{        // Reverse Button
				motor.setDirection(true);
			}
			if (Id == GUI_ID_BUTTON6)
			{        // Azimuthal Inc Button
				motor.setEnable(Azimuthal_Motor, true);
			}
			if (Id == GUI_ID_BUTTON7)
			{        // Vertical Inc Button
				motor.setEnable(Vertical_Motor, true);
			}
			break;
		case WM_NOTIFICATION_VALUE_CHANGED: // Value has changed
			if (Id == GUI_ID_SPINBOX0)
			{
				encoder.setDesiredPosition(Azimuthal_Encoder,
						SPINBOX_GetValue(pMsg->hWinSrc));
				hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SPINBOX0);
			}
			if (Id == GUI_ID_SPINBOX1)
			{
				encoder.setDesiredPosition(Vertical_Encoder,
						SPINBOX_GetValue(pMsg->hWinSrc));
				hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SPINBOX1);
			}

			break;
		}
		break;
	default:
		WM_DefaultProc(pMsg);
	}
}

//----------------------------------------------------------------------------//
void MainTask(void)
{
	WM_HWIN hDialogMain;
	WM_HWIN hItem;

	// Use memory devices on all windows to avoid flicker
	WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();

	// Check if recommended memory for the sample is available
	if (GUI_ALLOC_GetNumFreeBytes() < RECOMMENDED_MEMORY)
	{
		GUI_ErrorOut("Not enough memory available.");
		return;
	}
	hDialogMain = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate),
			_cbCallback, WM_HBKWIN, 0, 0);

	while (1)
	{
		GUI_Delay(10);

		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT0);
		EDIT_SetFloatValue(hItem, encoder.getPosition(Vertical_Encoder));
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT1);
		EDIT_SetFloatValue(hItem, encoder.getDesiredPosition(Vertical_Encoder));
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT2);
		EDIT_SetFloatValue(hItem, encoder.getPosError(Vertical_Encoder));
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT3);
		EDIT_SetFloatValue(hItem, encoder.getDirection());
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT4);
		EDIT_SetFloatValue(hItem, motor.getDirection());
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT5);
		EDIT_SetFloatValue(hItem, encoder.getPosError(Vertical_Encoder));
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT6);
		EDIT_SetFloatValue(hItem, encoder.getPosition(Azimuthal_Encoder));
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT7);
		EDIT_SetFloatValue(hItem, encoder.getDesiredPosition(Azimuthal_Encoder));
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT8);
		EDIT_SetFloatValue(hItem, encoder.getPosError(Azimuthal_Encoder));
	}
}
