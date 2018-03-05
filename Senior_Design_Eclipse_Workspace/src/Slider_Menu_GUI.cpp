// Adapted from SEGGER's DIALOG_SliderColor.c; see LICENSE.

#include <stddef.h>
#include "GUI.h"
#include "DIALOG.h"
#include "main.h"

#define RECOMMENDED_MEMORY (1024L * 15)

extern Motor *motor, motor_azimuthal, motor_vertical, motor_claw;

/*********************************************************************
 * Function description
 *   This table conatins the info required to create the dialog.
 *   It has been created manually, but could also be created by a GUI-builder.
 */
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
	{ FRAMEWIN_CreateIndirect, "Motor Control. If anything goes wrong, PRESS STOP!", 0, 0, 0, 480, 272, 0 },
	//
	// TEXT BOXES
	{ TEXT_CreateIndirect, "Desired", GUI_ID_TEXT0, 240,  30, 120, 40, TEXT_CF_LEFT },
	{ TEXT_CreateIndirect, "Current", GUI_ID_TEXT1, 360,  30, 120, 40, TEXT_CF_LEFT },
	{ TEXT_CreateIndirect, "       ", GUI_ID_TEXT2,  20, 221, 200, 30, TEXT_CF_LEFT },
	{ TEXT_CreateIndirect, "       ", GUI_ID_TEXT3, 230, 221, 250, 30, TEXT_CF_LEFT},
    //
	// EDIT BOXES
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT0, 240,  60, 100, 30, 0, 3 },
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT1, 360,  60, 100, 30, 0, 3 },
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT2, 240, 100, 10, 30, 0, 3 },
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT3, 260, 100, 200, 30, 0, 3 },
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT4, 240, 140, 100, 30, 0, 3 },
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT5, 360, 140, 100, 30, 0, 3 },
	//
	// BUTTONS
	{ BUTTON_CreateIndirect, "z (cm)",  GUI_ID_BUTTON0, 120,  60, 100, 30 },
	{ BUTTON_CreateIndirect, "Azimuthal", GUI_ID_BUTTON1, 120, 100, 100, 30 },
	{ BUTTON_CreateIndirect, "Claw",      GUI_ID_BUTTON2, 120, 140, 100, 30 },
	{ BUTTON_CreateIndirect, "Stop",      GUI_ID_BUTTON3, 120, 180, 340, 30 },
	{ BUTTON_CreateIndirect, "down",         GUI_ID_BUTTON4,  20,  41,  80, 80 },
	{ BUTTON_CreateIndirect, "up",         GUI_ID_BUTTON5,  20, 131,  80, 80 },
	{ BUTTON_CreateIndirect, "Home",      GUI_ID_BUTTON6, 120,  20,  45, 30 },
	{ BUTTON_CreateIndirect, "Fuel",      GUI_ID_BUTTON7, 170,  20,  45, 30 }
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

	//motor count not getting updated enough? -DC
	motor->getCount();
	hDlg = pMsg->hWin;
	switch (pMsg->MsgId)
	{

	case WM_INIT_DIALOG:

		// Buttons
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON4);
		BUTTON_SetFont(hItem, &GUI_Font24_ASCII);
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON5);
		BUTTON_SetFont(hItem, &GUI_Font24_ASCII);
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON6);
		BUTTON_SetFont(hItem, &GUI_Font16_ASCII);
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON7);
		BUTTON_SetFont(hItem, &GUI_Font16_ASCII);

		// Edits
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT0);
		EDIT_SetFloatMode(hItem, 0.0,-999.0, 999.0, 2, 0);
		EDIT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT );
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT1);
		EDIT_SetFloatMode(hItem, 0.0,-999.0, 999.0, 3, 0);
		EDIT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT );

		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT2);
		EDIT_SetFloatMode(hItem, 0.0,-999.0, 999.0, 2, 0);
		EDIT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT );
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT3);
		EDIT_SetFloatMode(hItem, 0.0,-2.0e9, 2.0e9, 0, 0);
		EDIT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT );

		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT4);
		EDIT_SetFloatMode(hItem, 0.0,-999.0, 999.0, 2, 0);
		EDIT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT );
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT5);
		EDIT_SetFloatMode(hItem, 0.0,-999.0, 999.0, 2, 0);
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
			if (Id == GUI_ID_BUTTON1)
			{
				BSP_LED_Off(LED1);
				set_motor(AZIMUTHAL);
				TEXT_SetText(hItem, "Azimuthal enabled.");
			}
			if (Id == GUI_ID_BUTTON0)
			{
				set_motor(VERTICAL);
				TEXT_SetText(hItem, "Vertical enabled.");
			}
			if (Id == GUI_ID_BUTTON6)
			{
				set_motor(VERTICAL);
				TEXT_SetText(hItem, "Going home.");
				motor -> gohome();
			}
			if (Id == GUI_ID_BUTTON7)
			{
				set_motor(VERTICAL);
				TEXT_SetText(hItem, "Going to fuel.");
				motor->setDesiredPosition(23.0);  //changed from 42.5 -DC
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

		/* Since each motor encoder shares the same pins on the board,
		 * all motor encoders cannot update at the same time.  They can only
		 * update when their motor is enabled.
		 */
		if (motor_vertical.enabled())
		{
			// vertical
			hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT0);
/*			EDIT_SetTextColor(hItem, EDIT_CI_DISABLED, GUI_RED);
			if (motor_vertical.enabled())
				EDIT_SetTextColor(hItem, EDIT_CI_DISABLED, GUI_RED);
			else
				EDIT_SetTextColor(hItem, EDIT_CI_DISABLED, GUI_RED);
*/
			EDIT_SetFloatValue(hItem, motor_vertical.getDesiredPosition());
			hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT1);
			EDIT_SetFloatValue(hItem, motor_vertical.getPosition());

			// print raw count of z motor
			hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT3);
			EDIT_SetFloatValue(hItem, motor_vertical.getCount());
		}

		else if (motor_azimuthal.enabled())
		{
			// azimuthal
			hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT2);
			EDIT_SetFloatValue(hItem, motor_azimuthal.getDesiredPosition());
//			hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT3);
//			EDIT_SetFloatValue(hItem, motor_azimuthal.getPosition());
		}

		else if (motor_claw.enabled())
		{
			// claw
			hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT4);
			EDIT_SetFloatValue(hItem, motor_claw.getDesiredPosition());
			hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT5);
			EDIT_SetFloatValue(hItem, motor_claw.getPosition());
		}
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_TEXT3);
		TEXT_SetText(hItem, "");
		if (motor_vertical.get_bot_ls())
			TEXT_SetText(hItem, "bottom limit switch!");
		else if (motor_vertical.get_top_ls())
			TEXT_SetText(hItem, "top limit switch!");
		else if (motor_claw.get_claw_ls())
			TEXT_SetText(hItem, "claw limit switch!");
		// + other warnings, etc.

	}
}
