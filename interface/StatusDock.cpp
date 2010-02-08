/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
// 
//		Includes
#include <stdio.h>
#include <translation/TranslationUtils.h>
#include <Bitmap.h>
//
//		Status Bar:: Local includes
#include "StatusDock.h"
#include "intf_global.h"
#include "logger.h"

//
// 		StatusDock::Constructor of the view
StatusDock::StatusDock(BRect rect, const char* name, uint32 resize,uint32 flags)
: BView (rect, name, resize, flags | B_FRAME_EVENTS)
{
	rgb_color bg_color=ui_color(B_PANEL_BACKGROUND_COLOR);
	SetViewColor(bg_color);
	//
	if(Bounds().Height() >= 75)
	{
		downloadBitmap = BTranslationUtils::GetBitmap('PNG ', "download_64_on");
		connectBitmapOn = BTranslationUtils::GetBitmap('PNG ', "cam_64_on");
		connectBitmapOff = BTranslationUtils::GetBitmap('PNG ', "cam_64_off");
	}
	else
	{
		downloadBitmap = BTranslationUtils::GetBitmap('PNG ', "download_48_on");
		connectBitmapOn = BTranslationUtils::GetBitmap('PNG ', "cam_48_on");
		connectBitmapOff = BTranslationUtils::GetBitmap('PNG ', "cam_48_off");
	}
	// 1.Create the statusbar
	CreateStatusBar();
	modus = MODE_INIT;
}
//
//	StatusDock :: Destructor
StatusDock::~StatusDock()
{
	delete(statusbar);
	delete(downloadBitmap);
	delete(connectBitmapOn);
	delete(connectBitmapOff);
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
void	StatusDock::UpdateStatus(float delta, const char *message)
{
	LogDebug("STATUSDOCK - Update status.");
	
	if(message)
		SetStatusMessage(message);
	if(delta > 0)
		statusbar->Update(delta);
	Draw(Bounds());
	
}
//		
//		StatusDock:: Draw the Status Bar
void	StatusDock::Draw(BRect rect)
{
	LogDebug("STATUSDOCK - Draw().");
	
	rgb_color color_label = {0x00, 0x00, 0x00, 0xff};
	rgb_color color_border = {0x8b, 0x8b, 0x83, 0xff};
	BFont font = be_plain_font;
	font_height fontHeight;
	float fFontHeight = 0;
	float fStringWidth = 0;
	BRect statDockRect;
	BRect picRect;
	//
	BView::Draw(rect);
	//
	SetDrawingMode( B_OP_ALPHA );
	SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	FillRect(rect);
	//
	SetFont(&font);
	GetFontHeight (&fontHeight);
	fFontHeight = fontHeight.ascent + fontHeight.descent;
	fStringWidth = StringWidth(statusMessage->String());
	//
	float picHeight = SD_MIN_BITMAP_HEIGHT;
	float picWidth = SD_MIN_BITMAP_WIDTH;
	int margin = 15;
	statDockRect = rect;
	picRect = statDockRect;
	//
	switch(modus)
	{
		case MODE_INIT:
		{
			if(connectBitmapOff != NULL)
			{
				picWidth = connectBitmapOff->Bounds().Width();
				picHeight = connectBitmapOff->Bounds().Height();
			}
			picRect.left = margin;
			picRect.right = picRect.left + picWidth;
			picRect.top = statDockRect.top + ceil((statDockRect.Height())/2) -picHeight/2;
			picRect.bottom = picRect.top + picHeight;
			if(connectBitmapOff != NULL)
				DrawBitmap(connectBitmapOff,picRect);
			else
			{
				SetHighColor(color_border);
				SetLowColor(color_border);
				StrokeRoundRect(picRect,2,2);
			}
			if(statusMessage != NULL)
			{
				MovePenTo(2 * margin + picWidth, statDockRect.top + ceil((statDockRect.Height())/2) + fFontHeight/2);
				SetHighColor(color_label);
				SetLowColor(color_label);
				DrawString(statusMessage->String());
			}
			break;
		}
		case MODE_CONNECTED:
		{
			if(connectBitmapOn != NULL)
			{
				picWidth = connectBitmapOn->Bounds().Width();
				picHeight = connectBitmapOn->Bounds().Height();
			}
			picRect.left = margin;
			picRect.right = picRect.left + picWidth;
			picRect.top = statDockRect.top + ceil((statDockRect.Height())/2) -picHeight/2;
			picRect.bottom = picRect.top + picHeight;
			if(connectBitmapOn != NULL)
				DrawBitmap(connectBitmapOn,picRect);
			else
			{
				SetHighColor(color_border);
				SetLowColor(color_border);
				StrokeRoundRect(picRect,2,2);
			}
			if(statusMessage != NULL)
			{
				MovePenTo(2 * margin + picWidth, statDockRect.top + ceil((statDockRect.Height())/2) + fFontHeight/2);
				SetHighColor(color_label);
				SetLowColor(color_label);
				DrawString(statusMessage->String());
			}
			break;
		}
		case MODE_DOWNLOAD:
		{
			if(downloadBitmap != NULL)
			{
				picWidth = downloadBitmap->Bounds().Width();
				picHeight = downloadBitmap->Bounds().Height();
			}
			picRect.left = margin;
			picRect.right = picRect.left + picWidth;
			picRect.top = statDockRect.top + ceil((statDockRect.Height())/2) -picHeight/2;
			picRect.bottom = picRect.top + picHeight;
			if(downloadBitmap != NULL)
				DrawBitmap(downloadBitmap,picRect);
			else
			{
				SetHighColor(color_border);
				SetLowColor(color_border);
				StrokeRoundRect(picRect,2,2);
			}
			break;
		}
	}
	SetDrawingMode(B_OP_COPY);
}
//
//		StatusDock:: Creation of the status bar	
void	StatusDock::CreateStatusBar()
{
	LogDebug("STATUSDOCK - Create Statusbar.");
	
	int margin = 15;
	BRect r = Bounds();
	float downBitMapWidth = 0;
	float downBitMapHeight = 0;
	if(downloadBitmap != NULL)
	{
		downBitMapWidth = downloadBitmap->Bounds().Width();
		downBitMapHeight = downloadBitmap->Bounds().Height();
	}
	else
	{
		downBitMapWidth = SD_MIN_BITMAP_WIDTH;
		downBitMapHeight = SD_MIN_BITMAP_HEIGHT;
	}
	r.left = 2 * margin + downBitMapWidth;
	r.right -= 2 * margin;
	r.top = r.top + downBitMapHeight/2;
	statusbar = new BStatusBar(r,"becam_status_bar");
	AddChild(statusbar);
	statusbar->SetBarHeight(10);
}
//
//	StatusDock :: Show the needed controls
void	StatusDock::ShowChildren(int newModus)
{
	LogDebug("STATUSDOCK - Show Children().");
	
	if(modus == newModus)
		return;
	switch(newModus)
	{
		case MODE_INIT:
		{
			LogDebug("STATUSDOCK - ShowChildren() - Init mode.");
			
			statusbar->Hide();
			break;
		}
		case MODE_DOWNLOAD:
		{
			LogDebug("STATUSDOCK - ShowChildren() - Download mode.");
			
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
	LogDebug("STATUSDOCK - Attached to window.");
	
	BView::AttachedToWindow();
	SetTarget(Window());
	SetViewColor(B_TRANSPARENT_COLOR);
	statusbar->Hide();
}
//
//	StatusDock :: Show
void StatusDock::Show()
{
	LogDebug("STATUSDOCK - Show Dock.");
	
	BView::Show();
	Invoke(new BMessage(STATDOCK_SHOWED));
}
//
//	StatusDock :: Hide
void StatusDock::Hide()
{
	LogDebug("STATUSDOCK - Hide Dock.");
	
	BView::Hide();
	Invoke(new BMessage(STATDOCK_HIDED));
}
//
//	StatusDock :: SetMaxStatusBar
void StatusDock::SetMaxStatusBar(float maximum)
{
	LogDebug("STATUSDOCK - Set max statusbar.");
	
	statusbar->Reset();
	statusbar->SetMaxValue(maximum);
}
//
//	StatusDock :: SetStatusMessage
void StatusDock::SetStatusMessage(const char* message)
{
	statusMessage = new BString(message);
	statusbar->SetText(statusMessage->String());
}
