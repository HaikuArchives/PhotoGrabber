/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
#include "preferences.h"
#include <Application.h>
#include <iostream>
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
#define _T(x) (x)
//

const rgb_color BKG_GREY		= { 216, 216, 216, 0 };

configplugin::configplugin(BRect rect) :BWindow(rect, _T("Communication Properties"), B_MODAL_WINDOW,	B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_WILL_DRAW)
{
 	// Nothing yet
}

bool configplugin::QuitRequested()
{
	return(true);
}

void configplugin::MessageReceived(BMessage* message)
{
	switch(message->what)
	{
		
		default:
		{
			BWindow::MessageReceived(message);
			break;
		}
	}
}




