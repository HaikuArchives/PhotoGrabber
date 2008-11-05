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
#include <interface/Button.h>
#include <interface/TextControl.h>
#include <app/Messenger.h>
#include <FilePanel.h>
#include <Path.h>
//
//	Local includes
#include "intf_global.h"
#include "PictureLabelButton.h"
//
// Define
#define HIDE_ACTIONDOCK		'SACD'
#define	SHOW_ACTIONDOCK		'HACD'
#define	DEL_BUTTON 			'DELB'
#define	DOWN_BUTTON 		'DOWN'
#define	NEXT_BUTTON 		'NBTN'
#define	PREVIOUS_BUTTON		'BBTN'
#define	ACCEPT_BUTTON		'ABTN'
#define	CANCEL_BUTTON		'CBTN'
#define	SELECT_PATHMENU		'SPME'
#define OPEN_FILE_PANEL		'OFPA'

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
	
	private:
		PictureLabelButton			*downloadButton;
		PictureLabelButton			*deleteButton;
		PictureLabelButton			*nextButton;
		PictureLabelButton			*previousButton;
		PictureLabelButton			*acceptButton;
		PictureLabelButton			*cancelButton;
		BButton						*selectButton;
		BTextControl				*defaultPath;
		BFilePanel 					*selectDirPanel;
};

#endif
