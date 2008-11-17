/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
#ifndef MAINVIEW_H
#define MAINVIEW_H

// Includes
#include <View.h>
#include <Message.h>

class BeCam_MainView : public BView {

public:
					BeCam_MainView(BRect frame, char *name); 
	virtual	void	AttachedToWindow();
	virtual	void	Draw(BRect updateRect);
	void			MouseDown(BPoint where);
};

#endif 
