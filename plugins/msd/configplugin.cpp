/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
 
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
#include <string>
#include "configplugin.h"
#include "MenuField.h"

//
// ZETA locale
#ifdef _ZETA_OS_
#include <locale/Locale.h>
#else
#define _T (x) (x)
#endif
//

const rgb_color BKG_GREY		= { 216, 216, 216, 0 };

configplugin::configplugin(BRect rect) :BWindow(rect, _T("Communication Properties"), B_MODAL_WINDOW,	B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_WILL_DRAW)
{
 	/*LoadSettingsFromFile();
 	BMenuItem *type1,*type2;
 	menu = new BPopUpMenu("");
 	//
 	//printf("PTP - %d\n",eptype);
 	
	type1 = new BMenuItem("1", NULL);
	if(globalEndpoint == 1)
		type1->SetMarked(true);
	menu->AddItem(type1);
	type2 = new BMenuItem("2", NULL);
	if(globalEndpoint == 2)
		type2->SetMarked(true);
	menu->AddItem(type2);
		
	
	BMenuField* popup = new BMenuField(BRect(22, 40, 150, 180), "port", _T("Type:"), menu);
	popup->SetDivider(be_plain_font->StringWidth(_T("Endpoint :")) + 5);
	
	endpointview = new BView(BRect(0,0,500,500), _T("Communications View"), B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	endpointview->SetViewColor(BKG_GREY);
	AddChild(endpointview);

	BBox *box_L1S1 = new BBox(BRect(7, 8, 140, 232), "name", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_FRAME_EVENTS | B_NAVIGABLE_JUMP | B_WILL_DRAW, B_FANCY_BORDER);
	box_L1S1->SetLabel(_T("Communications"));
	endpointview->AddChild(box_L1S1);
	BStringView *stringView_L2S10 = new BStringView(BRect(22, 20, 92, 40), "name", _T("Endpoint:"), B_FOLLOW_LEFT|B_FOLLOW_TOP, B_WILL_DRAW);
	box_L1S1->AddChild(stringView_L2S10);
	box_L1S1->AddChild(popup);
	BButton *button_L1S22 = new BButton(BRect(20, 190, 120, 215), "name", _T("OK"), new BMessage(CANCELPORTWINDOW), B_FOLLOW_LEFT|B_FOLLOW_TOP, B_NAVIGABLE|B_WILL_DRAW);
	box_L1S1->AddChild(button_L1S22);*/
}

bool configplugin::QuitRequested()
{
	return(true);
}

void configplugin::MessageReceived(BMessage* message)
{
	switch(message->what)
	{
		/*case CANCELPORTWINDOW:
		{
			//printf("PTP - %s\n", menu->FindMarked()->Label());
			if(!strcmp(menu->FindMarked()->Label(),"1"))
				globalEndpoint = 1;
			else
				globalEndpoint = 2;
			SaveSettingsToFile();
 			Quit();
			break;
		}*/ 
		default:
		{
			BWindow::MessageReceived(message);
			break;
		}
	}
}




