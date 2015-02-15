#include "preferences.h"
#include <Application.h>
#include <iostream.h>
#include <stdio.h>
#include <StatusBar.h>
#include <TextControl.h>
#include <RadioButton.h>
#include <StringView.h>
#include <CheckBox.h>
#include <Button.h>
#include "PopUpMenu.h"
#include <Box.h>
#include "MenuItem.h"
#include "SerialPort.h"
#include <string>
#include "configplugin.h"
#include "MenuField.h"

const rgb_color BKG_GREY		= { 216, 216, 216, 0 };

configplugin::configplugin(BRect rect) :BWindow(rect, "Communication Properties", B_MODAL_WINDOW,	B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_WILL_DRAW)
{
 	LoadSettingsFromFile();
   	BSerialPort serial; 
   	BMenuItem* item;
   	char devName[B_OS_NAME_LENGTH]; 
 	menu = new BPopUpMenu("");
  	for (int i = 0; i < serial.CountDevices(); i++) 
   	{ 
  		serial.GetDeviceName(i,devName);
		item = new BMenuItem(devName, NULL);
		char* tempstring = new char[100];
		sprintf(tempstring, "%s", devName);
		if (serialportsetting.Compare(tempstring) == 0) 
			item->SetMarked(true);
		menu->AddItem(item);
	}
	BMenuField* popup = new BMenuField(BRect(10, 160, 150, 180), "port", "Serial Port :", menu);
	popup->SetDivider(be_plain_font->StringWidth("Serial Port :") + 5);

	speedview = new BView(BRect(0,0,500,500), "Communications View", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	speedview->SetViewColor(BKG_GREY);
	AddChild(speedview);

	BBox *box_L1S1 = new BBox(BRect(7, 8, 140, 232), "name", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_FRAME_EVENTS | B_NAVIGABLE_JUMP | B_WILL_DRAW, B_FANCY_BORDER);
	box_L1S1->SetLabel("Communications");
	speedview->AddChild(box_L1S1);
	box_L1S1->AddChild(popup);
	BView *radioButtonView_L2S11 = new BView(BRect(22, 43, 115, 156), "name", B_FOLLOW_LEFT|B_FOLLOW_TOP, B_NAVIGABLE|B_WILL_DRAW);
	radioButton_L2S111 = new BRadioButton(BRect(0, 0, 113, 0), "", "115200 bps", new BMessage(SPEED115200), B_FOLLOW_LEFT|B_FOLLOW_TOP, B_NAVIGABLE|B_WILL_DRAW);
	radioButtonView_L2S11->AddChild(radioButton_L2S111);
	radioButton_L2S112 = new BRadioButton(BRect(0, 22, 113, 23), "", "57600 bps", new BMessage(SPEED57600), B_FOLLOW_LEFT|B_FOLLOW_TOP, B_NAVIGABLE|B_WILL_DRAW);
	radioButtonView_L2S11->AddChild(radioButton_L2S112);
	radioButton_L2S113 = new BRadioButton(BRect(0, 44, 113, 46), "", "38400 bps", new BMessage(SPEED38400), B_FOLLOW_LEFT|B_FOLLOW_TOP, B_NAVIGABLE|B_WILL_DRAW);
	radioButtonView_L2S11->AddChild(radioButton_L2S113);
	radioButton_L2S114 = new BRadioButton(BRect(0, 66, 113, 69), "", "19200 bps", new BMessage(SPEED19200), B_FOLLOW_LEFT|B_FOLLOW_TOP, B_NAVIGABLE|B_WILL_DRAW);
	radioButtonView_L2S11->AddChild(radioButton_L2S114);
	radioButton_L2S115 = new BRadioButton(BRect(0, 88, 113, 92), "", "9600 bps", new BMessage(SPEED9600), B_FOLLOW_LEFT|B_FOLLOW_TOP, B_NAVIGABLE|B_WILL_DRAW);
	radioButtonView_L2S11->AddChild(radioButton_L2S115);
	radioButtonView_L2S11->SetViewColor(BKG_GREY);
	box_L1S1->AddChild(radioButtonView_L2S11);
	BStringView *stringView_L2S10 = new BStringView(BRect(22, 20, 92, 40), "name", "Port Speed", B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW);
	box_L1S1->AddChild(stringView_L2S10);
	BButton *button_L1S22 = new BButton(BRect(20, 190, 120, 215), "name", "OK", new BMessage(CANCELPORTWINDOW), B_FOLLOW_LEFT|B_FOLLOW_TOP, B_NAVIGABLE|B_WILL_DRAW);
	box_L1S1->AddChild(button_L1S22);
	switch(speed) 
	{
		case B_115200_BPS:
		{
			radioButton_L2S111->SetValue(1);
			break;
		}
		case B_57600_BPS:
		{
			radioButton_L2S112->SetValue(1);
			break;
		}
		case B_38400_BPS:
		{
			radioButton_L2S113->SetValue(1);
			break;
		}
		case B_19200_BPS:
		{
			radioButton_L2S114->SetValue(1);
			break;
		}
		case B_9600_BPS:
		{
			radioButton_L2S115->SetValue(1);
			break;
		}
		default:
		{
			radioButton_L2S111->SetValue(1);
			break;
		}
	}
}

bool configplugin::QuitRequested()
{
	return(true);
}

void configplugin::MessageReceived(BMessage* message)
{
	switch(message->what)
	{
		case CANCELPORTWINDOW:
		{
			serialportsetting.SetTo(menu->FindMarked()->Label());
			SaveSettingsToFile();
 			Quit();
			break;
		}
		case SPEED115200:
		{
 			speed = B_115200_BPS;
			break;
		}
		case SPEED57600:
		{
 			speed = B_57600_BPS;
			break;
		}
		case SPEED38400:
		{
 			speed = B_38400_BPS;
			break;
		}
		case SPEED19200:
		{
 			speed = B_19200_BPS;
			break;
		}
		case SPEED9600:
		{
 			speed = B_9600_BPS;
			break;
		}
		default:
		{
			BWindow::MessageReceived(message);
			break;
		}
	}
}




