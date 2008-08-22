/*****************************************************************
 * Copyright (c) 2004-2008,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
 
// 
//		Includes
#include <stdio.h>
#include <StatusBar.h>
//
//		Status Bar:: Local includes
#include "StatusWindow.h"
#include "intf_global.h"
#include "debug.h"
//
// Globals
FILE *lfstatusw;
//
//		Status Bar::QuitRequested
bool BeCam_StatusWindow::QuitRequested()
{
	parent->statusWindow=NULL;
	return BWindow::QuitRequested();
}
//
//		Status Bar::MessageRecieved
void BeCam_StatusWindow::MessageReceived(BMessage* message)
{
	#ifdef DEBUG
	lfstatusw = fopen(INTF_LOGFILE,"a");	
	fprintf(lfstatusw,"STATUSWINDOW - Message received\n");
	fclose(lfstatusw);
	#endif
	switch(message->what)
	{
		case UPDATE_STAT:
		{
			const char *statusmsg;
			int32 count;
			statusmsg = message->FindString("statusmessage");
			message->FindInt32("count",&count);
			updateStatus(count,(char *)statusmsg);
		}
		default:
			BWindow::MessageReceived(message);
	}
}
//
// 		Status Bar::Constructor of the view
BeCam_StatusView::BeCam_StatusView(BRect r,float xPos, float yPos, uint32 maxpic, char *message):BView(r, "becam_status_view", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
	rgb_color bg_color=ui_color(B_PANEL_BACKGROUND_COLOR);
	SetViewColor(bg_color);
	//1. Create the animation
	CreateAnimation();
	//2. Create the statusbar
	BRect r;
	r.left = 5;
	r.top = 110;
	r.bottom = 126;
	r.right = 340;
	statusbar = new BStatusBar(r,"becam_status_bar");
	AddChild(statusbar);
	statusbar->SetBarHeight(10);
	statusbar->SetMaxValue((float)maxpic);
	strcpy(msg, message?message:"");
}
//
//		Status bar::Reset
void	BeCam_StatusView::reset(uint32 maxpics)
{
	statusbar->Reset();
	statusbar->SetMaxValue((float)maxpics);
}
//
//		Status Bar::Update Status		
void	BeCam_StatusView::updateStatus(uint32 delta, char *message)
{
	if(message)
	{
		strcpy(msg, message);
		SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
		FillRect(BRect(0, 0, 350, 30));
	}
	MovePenTo(10, statusbar->Bounds().bottom + 130);
	SetHighColor(0,0,0,255);
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	DrawString(msg);
	statusbar->Update((float)delta);
	Invalidate();
	Draw(Frame());
	
}
//		
//		Status Bar:: Draw the Status Bar
void	BeCam_StatusView::Draw(BRect rect)
{
	SetHighColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	FillRect(BRect(0, 0, 350, 30));
	MovePenTo(10, statusbar->Bounds().bottom + 130);
	SetHighColor(0,0,0,255);
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	DrawString(msg);
}
//
//		Status Bar:: Creation of the animation in the statusbar screen		
void	BeCam_StatusView::CreateAnimation()
{
	BRect r = Bounds();
	r.top += 5;
	r.bottom = 105;
	statusanimation = new Animation(r);
	statusanimation->SetImgFrontName(BString("img_mov_"));
	statusanimation->SetImgExtension(BString(".png"));
	AddChild(statusanimation);
}
//		
//		Status Bar:: Constructor of the Window
BeCam_StatusWindow::BeCam_StatusWindow(float xPos,float yPos,uint32 maxpics, char *message, BeCam_MainWindow *mainWindow):BWindow(BRect(xPos, yPos,WINDOW_WIDTH_STATUS + xPos,WINDOW_HEIGHT_STATUS + yPos), _T("PhotoGrabber download status"), B_TITLED_WINDOW, B_WILL_DRAW | B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE |B_NOT_RESIZABLE)
{
	parent = mainWindow;
	view=new BeCam_StatusView(Bounds(),xPos, yPos, maxpics, message);
	AddChild(view);
	view->statusanimation->StartAnimation();
}
//		
//		Status Bar:: Reset
void	BeCam_StatusWindow::reset(uint32 max)
{
	Lock();
	view->reset(max);
	Unlock();
}
//
//		Status Bar:: Update Status		
void	BeCam_StatusWindow::updateStatus(uint32 count, char *message)
{
	Lock();
	view->updateStatus(count, message);
	Unlock();
}



