/*****************************************************************
 * Copyright (c) 2004-2008,	PhotoGrabber-team,Jan-Rixt Van Hoye	 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
//
// File defenition
#ifndef PICTURELABELBUTTON_H
#define PICTURELABELBUTTON_H
//
//	Includes
#include <stdio.h>
#include <interface/View.h>
#include <app/Messenger.h>
#include <String.h>
//
//	Local includes
#include "intf_global.h"
#include "debug.h"
//
//	Class PictureButton
class PictureLabelButton : public BView
{
	public:
						PictureLabelButton(BRect rect, 
									const char* name,
									const char *label,
									BMessage *message, 
									uint32 resizeMask = B_FOLLOW_ALL,
									uint32 flags = B_WILL_DRAW);
						~PictureLabelButton();
		virtual	void	MessageReceived(BMessage* message);
		virtual	void	AttachedToWindow();
		virtual	void	Draw(BRect frame);
		virtual void	MouseUp(BPoint point);
		virtual void	MouseDown(BPoint point);
		void			SetTarget(BHandler* target);
							
	private:
		float			buttonHeight;
		float			buttonWidth;
		BBitmap			*buttonBitmap;
		BString			*buttonLabel;
		BMessage		*buttonMessage;
		BHandler		*buttonTargetHandler;
};

#endif
