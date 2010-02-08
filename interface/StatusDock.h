/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
#ifndef STATDOCK_H
#define STATDOCK_H
//
// Includes
#include <View.h>
#include <Invoker.h>
#include <Window.h>
#include <StatusBar.h>
//
// Local includes
#include "intf_global.h"
#include "core_global.h"
//
// defines
#define MODE_INIT				0
#define MODE_DOWNLOAD			1
#define MODE_CONNECTED			2
#define SD_MIN_BITMAP_HEIGHT	48
#define SD_MIN_BITMAP_WIDTH		48
//
#define STATDOCK_SHOW			'SDSH'
#define STATDOCK_HIDE			'SDHI'
#define STATDOCK_SHOWED			'SDSD'
#define STATDOCK_HIDED			'SDHD'


//
//		StatusDock:: View
class StatusDock : public BView, BInvoker
{
public:
					StatusDock(BRect rect, const char* name, 
									uint32 resizeMask = B_FOLLOW_ALL,
									uint32 flags = B_WILL_DRAW);
					~StatusDock();
					
	void			UpdateStatus(float delta, const char *message);
	virtual void	Draw(BRect rect);
	virtual void	MessageReceived(BMessage* message);
	virtual void	AttachedToWindow ();
	void			Show();
	void			Hide();
	void			SetMaxStatusBar(float maximum);
	void			SetStatusMessage(const char* message);
	void			ShowChildren(int newModus);

private:
	BString			*statusMessage;
	BStatusBar		*statusbar;	
	BBitmap			*connectBitmapOn;
	BBitmap			*connectBitmapOff;
	BBitmap			*downloadBitmap;
	int				modus;
	void 			CreateStatusBar();
};

#endif

