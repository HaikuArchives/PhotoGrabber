/*****************************************************************
 * Copyright (c) 2004-2008,	PhotoGrabber Team, Jan-Rixt Van Hoye *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
 
// 
//		Includes
#include <stdio.h>
#include <translation/TranslationUtils.h>
//
//		Status Bar:: Local includes
#include "StatusDock.h"
#include "intf_global.h"
#include "debug.h"
//
// Globals
FILE *lfstatusb;
//
// 		StatusDock::Constructor of the view
StatusDock::StatusDock(BRect rect, const char* name, uint32 resize,uint32 flags)
: BView (rect, name, resize, flags | B_FRAME_EVENTS)
{
	rgb_color bg_color=ui_color(B_PANEL_BACKGROUND_COLOR);
	SetViewColor(bg_color);
	//1. Create the animation
	CreateAnimation();
	//2. Create the statusbar
	CreateStatusBar();
	//3. Create the connection status screen
	CreateConnectStatus();
	//
	modus = MODE_INIT;
}
//
//	StatusDock :: Destructor
StatusDock::~StatusDock()
{
	// Nothing yet
}
//
//		StatusDock::MessageRecieved
void StatusDock::MessageReceived(BMessage* message)
{
	switch(message->what)
	{
		case UPDATE_STAT:
		{
			const char *statusmsg;
			float count;
			statusmsg = message->FindString("statusmessage");
			message->FindFloat("count",&count);
			UpdateStatus(count,(char *)statusmsg);
		}
		default:
			BView::MessageReceived(message);
	}
}

//
//		StatusDock::Update Status		
void	StatusDock::UpdateStatus(float delta, char *message)
{
	#ifdef DEBUG
		lfstatusb = fopen(INTF_LOGFILE,"a");	
		fprintf(lfstatusb,"STATUSDOCK - Update status\n");
		fclose(lfstatusb);
	#endif
	if(message)
		statusMessage = new BString(message);
	statusbar->Update(delta);
	Draw(Frame());
	
}
//		
//		StatusDock:: Draw the Status Bar
void	StatusDock::Draw(BRect rect)
{
	#ifdef DEBUG
		lfstatusb = fopen(INTF_LOGFILE,"a");	
		fprintf(lfstatusb,"STATUSDOCK - Draw()\n");
		fclose(lfstatusb);
	#endif
	rgb_color color_label = {0x00, 0x00, 0x00, 0xff};
	rgb_color color_border = {0x8b, 0x8b, 0x83, 0xff};
	SetDrawingMode( B_OP_ALPHA );
	SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	FillRect(Bounds());
	switch(modus)
	{
		case MODE_INIT:
		{
			#ifdef DEBUG
				lfstatusb = fopen(INTF_LOGFILE,"a");	
				fprintf(lfstatusb,"STATUSDOCK - Draw() - Init mode\n");
				fclose(lfstatusb);
			#endif
			BFont font = be_plain_font;
			font.SetSize(24);
			font.SetFace(B_BOLD_FACE);
    		font_height fontHeight;
    		SetFont(&font);
			GetFontHeight (&fontHeight);
			float fFontHeight = fontHeight.ascent + fontHeight.descent;
			float fStringWidth = StringWidth(statusMessage->String());
			int picHeight = 64;
			int picWidth = 64;
			int margin = 5;
			BRect statDockRect = Bounds();
			BRect picRect = statDockRect;
			picRect.left = picRect.left + (statDockRect.Width()/2) - (picWidth + margin + fStringWidth)/2;
			picRect.right = picRect.left + picWidth;
			picRect.top = statDockRect.top + ceil((statDockRect.Height())/2) -picHeight/2;
			picRect.bottom = picRect.top + picHeight;
			if(connectBitmap != NULL)
				DrawBitmap(connectBitmap,picRect);
			else
				SetHighColor(color_border);
				SetLowColor(color_border);
				StrokeRoundRect(picRect,2,2);
			if(statusMessage != NULL)
			{
				MovePenTo(statDockRect.Width()/2 - fStringWidth/2 + picRect.Width() + margin, statDockRect.top + ceil((statDockRect.Height())/2) + fFontHeight/2);
				SetHighColor(color_label);
				SetLowColor(color_label);
				DrawString(statusMessage->String());
			}
			break;
		}
		case MODE_DOWNLOAD:
		{
			#ifdef DEBUG
				lfstatusb = fopen(INTF_LOGFILE,"a");	
				fprintf(lfstatusb,"STATUSDOCK - Draw() - Download mode\n");
				fclose(lfstatusb);
			#endif
			if(statusMessage != NULL)
			{
				BFont font = be_plain_font;
				font.SetSize(12);
				font_height fontHeight;
    			SetFont(&font);
				MovePenTo(statusanimation->Frame().right + 10, statusbar->Frame().top - 5);
				SetHighColor(color_label);
				SetLowColor(color_label);
				DrawString(statusMessage->String());
			}
			break;
		}
	}
	SetDrawingMode(B_OP_COPY);
	BView::Draw(Bounds());
}
//
//		StatusDock:: Creation of the animation in the statusbar screen		
void	StatusDock::CreateAnimation()
{
	#ifdef DEBUG
		lfstatusb = fopen(INTF_LOGFILE,"a");	
		fprintf(lfstatusb,"STATUSDOCK - Create Animation\n");
		fclose(lfstatusb);
	#endif
	BRect r= Bounds();
	r.right = r.left + 252;
	statusanimation = new Animation(r);
	statusanimation->SetImgFrontName(BString("img_mov_"));
	statusanimation->SetImgExtension(BString(".png"));
	AddChild(statusanimation);
}
//
//		StatusDock:: Creation of the status bar	
void	StatusDock::CreateStatusBar()
{
	#ifdef DEBUG
		lfstatusb = fopen(INTF_LOGFILE,"a");	
		fprintf(lfstatusb,"STATUSDOCK - Create Statusbar\n");
		fclose(lfstatusb);
	#endif
	BRect r = Bounds();
	r.left = statusanimation->Bounds().right + 5;
	r.top = r.bottom - 40;
	r.bottom = r.bottom - 30;
	r.right -= 20;
	statusbar = new BStatusBar(r,"becam_status_bar");
	AddChild(statusbar);
	statusbar->SetBarHeight(10);
	statusbar->Hide();
}
//
//		StatusDock:: Create the connection status screen	
void	StatusDock::CreateConnectStatus()
{
	#ifdef DEBUG
		lfstatusb = fopen(INTF_LOGFILE,"a");	
		fprintf(lfstatusb,"STATUSDOCK - Create Connect status controls\n");
		fclose(lfstatusb);
	#endif
}
//
//	StatusDock :: Show the needed controls
void	StatusDock::ShowChildren(int newModus)
{
	#ifdef DEBUG
		lfstatusb = fopen(INTF_LOGFILE,"a");	
		fprintf(lfstatusb,"STATUSDOCK - Show Children()\n");
		fclose(lfstatusb);
	#endif
	if(modus == newModus)
		return;
	switch(newModus)
	{
		case MODE_INIT:
		{
			#ifdef DEBUG
				lfstatusb = fopen(INTF_LOGFILE,"a");	
				fprintf(lfstatusb,"STATUSDOCK - ShowChildren() - Init mode\n");
				fclose(lfstatusb);
			#endif
			statusanimation->Hide();
			statusbar->Hide();
			break;
		}
		case MODE_DOWNLOAD:
		{
			#ifdef DEBUG
				lfstatusb = fopen(INTF_LOGFILE,"a");	
				fprintf(lfstatusb,"STATUSDOCK - ShowChildren() - Download mode\n");
				fclose(lfstatusb);
			#endif
			statusanimation->Show();
			statusbar->Show();
			break;
		}
	}
	modus = newModus;
}
//
//	StatusDock :: AttachedToWindow()
void StatusDock::AttachedToWindow()
{
	#ifdef DEBUG
		lfstatusb = fopen(INTF_LOGFILE,"a");	
		fprintf(lfstatusb,"STATUSDOCK - Attached to window\n");
		fclose(lfstatusb);
	#endif
	SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
	connectBitmap = BTranslationUtils::GetBitmap('PNG ', "Camera");	
	statusanimation->Hide();
	statusanimation->StartAnimation();
	BView::AttachedToWindow();	
}
//
//	StatusDock :: SetMaxStatusBar
void StatusDock::SetMaxStatusBar(float maximum)
{
	#ifdef DEBUG
		lfstatusb = fopen(INTF_LOGFILE,"a");	
		fprintf(lfstatusb,"STATUSDOCK - Set max statusbar\n");
		fclose(lfstatusb);
	#endif
	statusbar->Reset();
	statusbar->SetMaxValue(maximum);
}
//
//	StatusDock :: SetStatusMessage
void StatusDock::SetStatusMessage(const char* message)
{
	statusMessage = new BString(message);
}
