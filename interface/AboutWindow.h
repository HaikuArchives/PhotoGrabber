/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 			*
 * All rights reserved.											 						*
 * Distributed under the terms of the MIT License.               			*
 *****************************************************************/
 //
// File defenition
#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H
//
//		Includes
#include <stdio.h>
#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <TranslationUtils.h>
#include <TranslatorRoster.h>
#include <TabView.h>
#include <TextView.h>

//
//		Local includes
#include "intf_global.h"
#include "MainWindow.h"

//
// 	External variables
extern unsigned char my_bitmap[30000];
//
//	View of the About tab
class AboutTabView : public BView
{
	public:
					AboutTabView(BRect r);
					~AboutTabView(void);
	virtual void	Draw(BRect rect);

private:
		BBitmap		*me_bitmap;
};
//
//		About View class
class BeCam_AboutView : public BView
{
	public:
					BeCam_AboutView(float xPos,float yPos);
					~BeCam_AboutView(void);

private:
		BTabView		*tabView;
		BTab			*aboutTab;
		AboutTabView 	*aboutView;
		BTab			*creditsTab;
		BView 			*creditsView;
		BTextView		*creditsTextView;
};

//
//		About window Class
class BeCam_AboutWindow : public BWindow
{
	public:
						BeCam_AboutWindow(float xPos,float yPos, class BeCam_MainWindow *mainWindow);
		virtual bool	QuitRequested();
		virtual void	MessageReceived(BMessage* message);

	private:
		BeCam_AboutView					*view;
		class BeCam_MainWindow			*parent;
};

#define ABOUT_WINDOW				1
#define WINDOW_HEIGHT_ABOUT			245
#define	WINDOW_WIDTH_ABOUT			485

#endif


