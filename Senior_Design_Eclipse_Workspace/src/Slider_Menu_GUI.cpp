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
	{ TEXT_CreateIndirect, "Set", GUI_ID_TEXT0, 290,  10, 80, 40, TEXT_CF_HCENTER },
	{ TEXT_CreateIndirect, "Real", GUI_ID_TEXT1, 380,  10, 80, 40, TEXT_CF_HCENTER},
	{ TEXT_CreateIndirect, "Enable", GUI_ID_TEXT4, 20,  10, 80, 30, TEXT_CF_HCENTER },
	{ TEXT_CreateIndirect, "Operation", GUI_ID_TEXT5, 110,  10, 170, 30, TEXT_CF_HCENTER },
	// down left status window
	{ TEXT_CreateIndirect, "       ", GUI_ID_TEXT2,  20, 210, 200, 30, TEXT_CF_LEFT },
	// down right warning window
	{ TEXT_CreateIndirect, "       ", GUI_ID_TEXT3, 230, 210, 250, 30, TEXT_CF_LEFT},
    //
	// EDIT BOXES. Show set and current positions
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT0, 290,  35, 80, 30, 0, 3 },
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT1, 380,  35, 80, 30, 0, 3 },
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT2, 290, 105, 80, 30, 0, 3 },
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT3, 380, 105, 80, 30, 0, 3 },
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT4, 290, 140, 80, 30, 0, 3 },
	{ EDIT_CreateIndirect, NULL, GUI_ID_EDIT5, 380, 140, 80, 30, 0, 3 },

	// BUTTONS
	{ BUTTON_CreateIndirect, "Z (cm)",  GUI_ID_BUTTON0, 20,  35, 80, 65 },
	{ BUTTON_CreateIndirect, "Home",      GUI_ID_BUTTON6, 110,  35,  80, 30 },
	{ BUTTON_CreateIndirect, "Fuel",      GUI_ID_BUTTON7, 200,  35,  80, 30 },
	{ BUTTON_CreateIndirect, "Down",         GUI_ID_BUTTON4,  110,  70,  80, 30},
	{ BUTTON_CreateIndirect, "Up",         GUI_ID_BUTTON5,  200, 70,  80, 30 },

	{ BUTTON_CreateIndirect, "Rotate", GUI_ID_BUTTON1, 20, 105, 80, 30 },
	{ BUTTON_CreateIndirect, "C.W.",     GUI_ID_BUTTON8,  110,  105,  80, 30 },
	{ BUTTON_CreateIndirect, "A.C.W.",     GUI_ID_BUTTON9,  200,  105,  80, 30 },

	{ BUTTON_CreateIndirect, "Claw",      GUI_ID_BUTTON2, 20, 140, 80, 30 },
	{ BUTTON_CreateIndirect, "Close",     GUI_ID_BUTTON10,  110, 140,  80, 30 },
	{ BUTTON_CreateIndirect, "Open",     GUI_ID_BUTTON11,  200,  140,  80, 30 },

	{ BUTTON_CreateIndirect, "Stop",      GUI_ID_BUTTON3, 110, 175, 170, 30 }
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
	motor -> enable();
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
		BUTTON_SetFont(hItem, &GUI_Font16B_ASCII);
//		BUTTON_SetFrameColor(hItem,  GUI_ORANGE);

		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON5);
		BUTTON_SetFont(hItem, &GUI_Font16B_ASCII);
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON10);
		BUTTON_SetFont(hItem, &GUI_Font16B_ASCII);
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON11);
		BUTTON_SetFont(hItem, &GUI_Font16B_ASCII);
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON6);
		BUTTON_SetFont(hItem, &GUI_Font16B_ASCII);
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON7);
		BUTTON_SetFont(hItem, &GUI_Font16B_ASCII);
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON8);
		BUTTON_SetFont(hItem, &GUI_Font16B_ASCII);
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON9);
		BUTTON_SetFont(hItem, &GUI_Font16B_ASCII);

		// Edits
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT0);
		EDIT_SetFloatMode(hItem, 0.0,-999.0, 999.0, 2, 0);
		EDIT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT );
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT1);
		EDIT_SetFloatMode(hItem, 0.0,-999.0, 999.0, 2, 0);
		EDIT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT );

		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT2);
		EDIT_SetFloatMode(hItem, 0.0,-999.0, 999.0, 2, 0);
		EDIT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT );
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT3);
		EDIT_SetFloatMode(hItem, 0.0,-999, 999, 2, 0);
		EDIT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT );

		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT4);
		EDIT_SetFloatMode(hItem, 0.0,-99.0, 99.0, 2, 0);
		EDIT_SetTextAlign(hItem, GUI_TA_VCENTER | GUI_TA_RIGHT );
		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT5);
		EDIT_SetFloatMode(hItem, 0.0,-99.0, 99.0, 2, 0);
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
				TEXT_SetText(hItem, "Stopped");
			}
			// three enable buttons
			if (Id == GUI_ID_BUTTON1)
			{
				BSP_LED_Off(LED1);
				set_motor(AZIMUTHAL);
				TEXT_SetText(hItem, "Rotate Enabled");
			}
			if (Id == GUI_ID_BUTTON0)
			{
				set_motor(VERTICAL);
				TEXT_SetText(hItem, "Z Enabled");
			}
			if (Id == GUI_ID_BUTTON2)
			{
				set_motor(CLAW);
				TEXT_SetText(hItem, "Claw Enabled");
			}

			// buttons setting desired positions
			// vertical
			if (Id == GUI_ID_BUTTON6)
				// home, up a large distance to trigger the top limit switch
				motor_vertical.setDesiredPosition(-2000.0);
			if (Id == GUI_ID_BUTTON7)
				// fuel
				motor_vertical.setDesiredPosition(23.0);  //changed from 42.5 -DC
			if (Id == GUI_ID_BUTTON4)
				// down
				motor_vertical.increase();
			if (Id == GUI_ID_BUTTON5)
				// up
				motor_vertical.decrease();

			// claw
			if (Id == GUI_ID_BUTTON10)
				// close
				motor_claw.increase();
			if (Id == GUI_ID_BUTTON11)
				// open
				motor_claw.decrease();

			// rotate
			if (Id == GUI_ID_BUTTON8)
				// clockwise
				motor_azimuthal.increase();
			if (Id == GUI_ID_BUTTON9)
				// anticlockwise
				motor_azimuthal.decrease();

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

	if (!motor_claw.cor_establish)
		set_motor(CLAW);

	while (1)
	{
		GUI_Delay(10);

		// display set position
		// vertical
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT0);
		EDIT_SetFloatValue(hItem, motor_vertical.getDesiredPosition());

		// azimuthal
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT2);
		EDIT_SetFloatValue(hItem, motor_azimuthal.getDesiredPosition());

		// claw
		hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT4);
		EDIT_SetFloatValue(hItem, motor_claw.getDesiredPosition());

		// display enabled motor's real position
		/* Since the motor encoders share the same pins on the board,
		 * all motor encoders cannot update at the same time.  They can only
		 * update when their motor is enabled.
		 */
		if (motor_vertical.enabled())
		{
			// vertical
//			hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT0);
/*			EDIT_SetTextColor(hItem, EDIT_CI_DISABLED, GUI_RED);
			if (motor_vertical.enabled())
				EDIT_SetTextColor(hItem, EDIT_CI_DISABLED, GUI_RED);
			else
				EDIT_SetTextColor(hItem, EDIT_CI_DISABLED, GUI_RED);
*/
//			EDIT_SetFloatValue(hItem, motor_vertical.getDesiredPosition());
			hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT1);
			EDIT_SetFloatValue(hItem, motor_vertical.getPosition());
		}

		else if (motor_azimuthal.enabled())
		{
			// azimuthal
			hItem = WM_GetDialogItem(hDialogMain, GUI_ID_EDIT3);
			EDIT_SetFloatValue(hItem, motor_azimuthal.getPosition());
		}

		else if (motor_claw.enabled())
		{
			// claw
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
