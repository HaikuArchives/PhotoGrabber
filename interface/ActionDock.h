/*****************************************************************
 * Copyright (c) 2004-2008,	PhotoGrabber-team,Jan-Rixt Van Hoye	 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
//
// File defenition
#ifndef ACTIONDOCK_H
#define ACTIONDOCK_H
//
//	Includes
#include <stdio.h>
#include <interface/View.h>
#include <app/Messenger.h>
//
//	Local includes
#include "intf_global.h"
#include "PictureLabelButton.h"
//
// Define
#define HIDE_ACTIONDOCK		'SACD'
#define	SHOW_ACTIONDOCK		'HACD'

//
//	Class ActionDock
class ActionDock : public BView
{
	public:
						ActionDock(BRect rect, const char* name, 
									uint32 resizeMask = B_FOLLOW_ALL,
									uint32 flags = B_WILL_DRAW);
						~ActionDock();
		virtual	void	MessageReceived(BMessage* message);
		virtual	void	AttachedToWindow();
		virtual	void	Draw(BRect frame);
				void	Hide();
				void	Show();
	
	private:
		PictureLabelButton			*downloadButton;
		PictureLabelButton			*deleteButton;
		// the camera interface
		//BMenuField			*becam_downloadPopup;
		//BPopUpMenu			*becam_downloadMenu;
		//BMenuItem 			*defaultPath;
		//BFilePanel 			*becam_selectdirpanel;
};

#endif
