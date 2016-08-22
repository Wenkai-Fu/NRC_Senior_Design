// Adapted from SEGGER's DIALOG_SliderColor.c; see LICENSE.

#include <stddef.h>
#include "GUI.h"
#include "DIALOG.h"
#include "main.h"

#define RECOMMENDED_MEMORY (1024L * 15)

extern Motor *motor, motor_azimuthal, motor_vertical, motor_claw;
extern bool limit_switch;

/*********************************************************************
 * Function description
 *   This table conatins the info required to create the dialog.
 *   It has been created manually, but could also be created by a GUI-builder.
 */
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
	{ FRAMEWIN_CreateIndirect, "Motor Control", 0, 0, 0, 480, 272, 0 },
	//
	// TEXT BOXES
	{ TEXT_CreateIndirect, "Desired", GUI_ID_TEXT0, 240,  30, 120, 40, TEXT_CF_LEFT },
	{ TEXT_CreateIndirect, "Current", GUI_ID_TEXT1, 360,  30, 120, 40, TEXT_CF_LEFT },
	{ TEXT_CreateIndirect, "       ", GUI_ID_TEXT2,  20, 221, 220, 30, TEXT_CF_LEFT },
	{ TEXT_CreateIndirect, "       ", GUI_ID_TEXT3, 260, 221, 200, 30, TEXT_CF_RIGHT },
    //
	// EDIT BOXES
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT0, 240,  60, 100, 30, 0, 3 },
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT1, 360,  60, 100, 30, 0, 3 },
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT2, 240, 100, 100, 30, 0, 3 },
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT3, 360, 100, 100, 30, 0, 3 },
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT4, 240, 140, 100, 30, 0, 3 },
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT5, 360, 140, 100, 30, 0, 3 },
	//
	// BUTTONS
	{ BUTTON_CreateIndirect, "Azimuthal", GUI_ID_BUTTON0, 120,  60, 100, 30 },
	{ BUTTON_CreateIndirect, "Vertical",  GUI_ID_BUTTON1, 120, 100, 100, 30 },
	{ BUTTON_CreateIndirect, "Claw",      GUI_ID_BUTTON2, 120, 140, 100, 30 },
	{ BUTTON_CreateIndirect, "Stop",      GUI_ID_BUTTON3, 120, 180, 340, 30 },
	{ BUTTON_CreateIndirect, "+",         GUI_ID_BUTTON4,  20,  41,  80, 80 },
	{ BUTTON_CreateIndirect, "-",         GUI_ID_BUTTON5,  20, 131,  80, 80 },
};

// Dialog handles
WM_HWIN _hDialogMain;


static void disable_all()
{
	motor_vertical.disable();
	motor_azimuthal.disable();
	motor_claw.disable();
}

const int VERTICAL = 0, AZIMUTHAL = 1, CLAW = 2;
static void set_motor(const int key)
{
	disable_all();
	if (key == VERTICAL)
		motor = &motor_vertical;
	else if (key == AZIMUTHAL)
		motor = &motor_azimuthal;
	else if (key == CLAW)
		motor = &motor_claw;
	else
		;
	motor->enable();
}

//----------------------------------------------------------------------------//
static void _cbCallback(WM_MESSAGE * pMsg)
{
	WM_HWIN hDlg;
	WM_HWIN hItem;
	int NCode;
	int Id;

	hDlg = pMsg->hWin;
	switch (pMsg->MsgId)
	{

	case WM_INIT_DIALOG:

		// Buttons
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON4);
		BUTTON_SetFont(hItem, &GUI_FontD36x48);
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON5);
		BUTTON_SetFont(hItem, &GUI_FontD36x48);

		// Edits
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT0);
		EDIT_SetFloatMode(hItem, 0.0,  0.0, 360.0, 2, 0);
		EDIT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT );
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT1);
		EDIT_SetFloatMode(hItem, 0.0,  0.0, 360.0, 2, 0);
		EDIT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT );

		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT2);
		EDIT_SetFloatMode(hItem, 0.0,  0.0, 45.0, 2, 0);
		EDIT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT );
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT3);
		EDIT_SetFloatMode(hItem, 0.0,  0.0, 45.0, 2, 0);
		EDIT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT );

		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT4);
		EDIT_SetFloatMode(hItem, 0.0,  0.0, 1.0, 2, 0);
		EDIT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT );
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT5);
		EDIT_SetFloatMode(hItem, 0.0,  0.0, 1.0, 2, 0);
		EDIT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT );

		hItem = WM_GetDialogItem(hDlg, GUI_ID_TEXT3);
		TEXT_SetTextColor(hItem, GUI_RED);

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
		NCode = pMsg->Data.v;              // Notification code

		switch (NCode)
		{

		case WM_NOTIFICATION_RELEASED:
			hItem = WM_GetDialogItem(hDlg, GUI_ID_TEXT2);
			if (Id == GUI_ID_OK)
			{
				// OK Button
				GUI_EndDialog(hDlg, 0);
			}
			if (Id == GUI_ID_BUTTON3)
			{
				// Stop Button
				BSP_LED_On(LED1);
				disable_all();
				TEXT_SetText(hItem, "Stopped.");
			}
			if (Id == GUI_ID_BUTTON0)
			{
				BSP_LED_Off(LED1);
				set_motor(AZIMUTHAL);
				TEXT_SetText(hItem, "Azimuthal enabled.");
			}
			if (Id == GUI_ID_BUTTON1)
			{
				set_motor(VERTICAL);
				TEXT_SetText(hItem, "Vertical enabled.");
			}
			if (Id == GUI_ID_BUTTON2)
			{
				set_motor(CLAW);
				TEXT_SetText(hItem, "Claw enabled.");
			}
			if (Id == GUI_ID_BUTTON4)
			{
				motor->increase();
			}
			if (Id == GUI_ID_BUTTON5)
			{
				motor->decrease();
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

	// Set the font.  Built-in fonts are listed here:
	//    https://www.segger.com/emwin-fonts.html
	// It seems that fonts must be set for each widget type.
	//GUI_SetDefaultFont(&GUI_FontComic18B_ASCII);

	BUTTON_SetDefaultFont(&GUI_Font20B_ASCII);
	EDIT_SetDefaultFont(&GUI_Font20B_ASCII);
	TEXT_SetDefaultFont(&GUI_Font20B_ASCII);

	// Set text alignment
	EDIT_SetDefaultTextAlign(GUI_TA_VCENTER);

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

		// azimuthal
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT0);
		EDIT_SetFloatValue(hItem, motor_azimuthal.getDesiredPosition());
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT1);
		EDIT_SetFloatValue(hItem, motor_azimuthal.getPosition());

		// vertical
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT2);
		EDIT_SetFloatValue(hItem, motor_vertical.getDesiredPosition());
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT3);
		EDIT_SetFloatValue(hItem, motor_vertical.getPosition());

		// claw
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT4);
		EDIT_SetFloatValue(hItem, motor_claw.getDesiredPosition());
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT5);
		EDIT_SetFloatValue(hItem, motor_claw.getPosError());

		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_TEXT3);
		TEXT_SetText(hItem, "");
		if (limit_switch)
		{
			TEXT_SetText(hItem, "Limit switch engaged!");
		}
		// + other warnings, etc.

	}
}
