/*
****************************************************************
* Copyright (c) 2004-2007,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
#ifndef _CONFIGPLUGIN_H_
#define _CONFIGPLUGIN_H_
#include <string>
#include <Window.h>
#include <String.h>
#include <PopUpMenu.h>
#define CANCELPORTWINDOW 'CSW'

class configplugin : public BWindow
{
public:
	configplugin(BRect rect);
	virtual bool	QuitRequested();
	virtual void	MessageReceived(BMessage* message);
private:
	BView* endpointview;
	BPopUpMenu* menu;
};

#endif
