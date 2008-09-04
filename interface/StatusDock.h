/*****************************************************************
 * Copyright (c) 2004-2008,	PhotoGrabber Team, Jan-Rixt Van Hoye *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/

//
#ifndef STATDOCK_H
#define STATDOCK_H
//
// Includes
#include <View.h>
#include <Window.h>
#include <StatusBar.h>
//
// Local includes
#include "intf_global.h"
#include "Animation.h"
#include "core_global.h"
#include "debug.h"
//
// defines
#define MODE_INIT			0
#define MODE_DOWNLOAD		1

//
//		StatusDock:: View
class StatusDock : public BView
{
public:
					StatusDock(BRect rect, const char* name, 
									uint32 resizeMask = B_FOLLOW_ALL,
									uint32 flags = B_WILL_DRAW);
					~StatusDock();
					
	void			UpdateStatus(float delta, char *message);
	virtual void	Draw(BRect rect);
	virtual void	MessageReceived(BMessage* message);
	virtual void	AttachedToWindow ();
	void			SetMaxStatusBar(float maximum);
	void			SetStatusMessage(const char* message);
	void			ShowChildren(int newModus);

private:
	BString			*statusMessage;
	BStatusBar		*statusbar;	
	Animation		*statusanimation;
	BBitmap			*connectBitmap;
	int				modus;
	void 			CreateAnimation();
	void 			CreateStatusBar();
	void			CreateConnectStatus();
};

#endif

