/*****************************************************************
 * Copyright (c) 2004-2008,	PhotoGrabber-team, Jan-Rixt Van Hoye *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/

//
//	Local includes
#include "ActionDock.h"
#include "debug.h"
//
// Includes
#include <interface/Window.h>
//
//	Globals
FILE *lfactionbar;
//
//	ActionDock :: Constructor
ActionDock::ActionDock(BRect rect, const char* name, uint32 resize,uint32 flags)
: BView (rect, name, resize, flags | B_FRAME_EVENTS)
{	
	BFont font = be_plain_font;
	//font.SetFace(B_BOLD_FACE);
	font.SetSize(11);
    font_height fontHeight;
    SetFont(&font);
	// test
	int picLabelHeight = 60;
	int picLabelWidth = 64;
	BRect r = Bounds();
	r.left = rect.left + 10;
	r.top = r.top + ceil((rect.bottom - rect.top)/2) - picLabelHeight/2;
	r.right = r.left + picLabelHeight;
	r.bottom = r.top + picLabelWidth;
	// Create download button
	downloadButton = new PictureLabelButton(r, "download","Download",new BMessage(DOWN_BUTTON),B_FOLLOW_LEFT|B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW);
	AddChild(downloadButton);
	r.left = r.right + 10;
	r.right = r.left + picLabelWidth;
	// Create delete button
	deleteButton = new PictureLabelButton(r, "delete","Delete",new BMessage(DEL_BUTTON), B_FOLLOW_LEFT|B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW);
	AddChild(deleteButton);
}
//
//	ActionDock :: Destructor
ActionDock::~ActionDock()
{
	// Nothing yet
}
//						
// ActionDock :: MessageReceived
void ActionDock::MessageReceived(BMessage* message)
{
	#ifdef DEBUG
		lfactionbar = fopen(INTF_LOGFILE,"a");	
		fprintf(lfactionbar,"ANIMATION - Delay message received\n");
		fclose(lfactionbar);
	#endif
	
	switch(message->what)
	{
		default :
			BView::MessageReceived(message);	
			break;	
	}
}
//
//	ActionDock :: AttachedToWindow()
void ActionDock::AttachedToWindow()
{
	#ifdef DEBUG
		lfactionbar = fopen(INTF_LOGFILE,"a");	
		fprintf(lfactionbar,"ACTIONBAR - Attached to window\n");
		fclose(lfactionbar);
	#endif
	BView::AttachedToWindow();
	rgb_color color_background = {0x42, 0x42, 0x42, 0xff};
	//SetViewColor(color_background);
	SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );	
}
//
//	ActionDock :: Draw
void ActionDock::Draw(BRect frame)
{
	rgb_color color_border = {0x8b, 0x8b, 0x83, 0xff};
	//rgb_color color_border = {0x00, 0x00, 0x00, 0xff};
	BRect actionBarFrame = Bounds();
	BPoint startPoint,endPoint;
	startPoint.x = actionBarFrame.left;
	startPoint.y = actionBarFrame.top + 1;
	endPoint.x = actionBarFrame.right;
	endPoint.y = actionBarFrame.top + 1;
	SetDrawingMode( B_OP_ALPHA );
	SetHighColor(color_border);
	SetLowColor(color_border);
	SetPenSize(1);
	StrokeLine(startPoint,endPoint);
	SetDrawingMode( B_OP_COPY );
	BView::Draw(frame);
					
}
//
// ActionDock :: Hide
void ActionDock::Hide()
{	
	if (!Window() || Window()->IsActive() == false)
	{
		float height = Bounds().Height();
		BMessage *hideMessage = new BMessage(HIDE_ACTIONDOCK);
		hideMessage->AddFloat("height",height);
		Window()->PostMessage(hideMessage); 
	}
	BView::Hide();	
}
//
// ActionDock :: Show
void ActionDock::Show()
{
	if (!Window() || Window()->IsActive() == false)
	{
		float height = Bounds().bottom - Bounds().top;
		BMessage *showMessage = new BMessage(SHOW_ACTIONDOCK);
		showMessage->AddFloat("height",height);
		Window()->PostMessage(showMessage); 
	}
	BView::Show();
	
}
