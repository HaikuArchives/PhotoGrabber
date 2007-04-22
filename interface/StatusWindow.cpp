/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
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
BeCam_StatusView::BeCam_StatusView(BRect r,float xPos, float yPos, uint32 maxpic, char *message):BView(r/*BRect(0,0,xPos + WINDOW_WIDTH_STATUS - 150 ,yPos + WINDOW_HEIGHT_STATUS - 80)*/, "becam_status_view", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
	rgb_color bg_color=ui_color(B_PANEL_BACKGROUND_COLOR);
	SetViewColor(bg_color);
	statusbar = new BStatusBar(BRect(5,30,340,46),"becam_status_bar");
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
	MovePenTo(10, 20);
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
	//FillRect(rect);
	MovePenTo(10, 20);
	SetHighColor(0,0,0,255);
	SetLowColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	DrawString(msg);
}
//		
//		Status Bar:: Constructor of the Window
#ifdef _ZETA_OS_
BeCam_StatusWindow::BeCam_StatusWindow(float xPos,float yPos,uint32 maxpics, char *message, BeCam_MainWindow *mainWindow):BWindow(BRect(xPos, yPos,WINDOW_WIDTH_STATUS + xPos,WINDOW_HEIGHT_STATUS + yPos), _T("PhotoGrabber download status"), B_TITLED_WINDOW, B_WILL_DRAW | B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE |B_NOT_RESIZABLE)
#else
BeCam_StatusWindow::BeCam_StatusWindow(float xPos,float yPos,uint32 maxpics, char *message, BeCam_MainWindow *mainWindow):BWindow(BRect(xPos, yPos,WINDOW_WIDTH_STATUS + xPos,WINDOW_HEIGHT_STATUS + yPos), "PhotoGrabber download status", B_TITLED_WINDOW, B_WILL_DRAW | B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE |B_NOT_RESIZABLE)
#endif
{
	parent = mainWindow;
	view=new BeCam_StatusView(Bounds(),xPos, yPos, maxpics, message);
	AddChild(view);
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
