/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// File defenition
#ifndef PICTURELABELBUTTON_H
#define PICTURELABELBUTTON_H
//
//	Includes
#include <stdio.h>
#include <interface/Control.h>
#include <app/Messenger.h>
#include <String.h>
//
//	Local includes
#include "intf_global.h"
#include "debug.h"
//
// defines
#define PLB_MIN_BITMAP_HEIGHT	48
#define PLB_MIN_BITMAP_WIDTH	48
//
//	Class PictureButton
class PictureLabelButton : public BControl
{
	public:
						PictureLabelButton(BRect rect, 
									const char* name,
									const char *label,
									BBitmap *bitmapOn,
									BBitmap *bitmapOff,
									BMessage *message, 
									uint32 resizeMask = B_FOLLOW_ALL,
									uint32 flags = B_WILL_DRAW | B_FRAME_EVENTS);
						~PictureLabelButton();
		virtual	void	MessageReceived(BMessage* message);
		virtual	void	AttachedToWindow();
		virtual void	KeyDown(const char *bytes, int32 numBytes);
		virtual	void	Draw(BRect frame);
		virtual void	MouseUp(BPoint point);
		virtual void	MouseDown(BPoint point);
							
	private:
		float			buttonHeight;
		float			buttonWidth;
		float			bitmapHeight;
		BBitmap			*buttonBitmapOn;
		BBitmap			*buttonBitmapOff;
		BHandler		*buttonTargetHandler;
};

#endif
