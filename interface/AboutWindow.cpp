/*****************************************************************
* Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
* All rights reserved.											 *
* Distributed under the terms of the MIT License.                *
 *****************************************************************/
// 
//		Local Includes
#include "AboutWindow.h"

//
//		Aboutview :: Constructor
BeCam_AboutView::BeCam_AboutView(float xPos, float yPos):BView(BRect(0,0,WINDOW_WIDTH_ABOUT- 50 + xPos,WINDOW_HEIGHT_ABOUT - 100 + yPos), "aboutview", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
	rgb_color bg_color=ui_color(B_PANEL_BACKGROUND_COLOR);
	SetViewColor(bg_color);
	
	BTranslatorRoster *roster = BTranslatorRoster::Default();
	me_bitmap= BTranslationUtils::GetBitmap("about", roster);
	//if((me_bitmap=new BBitmap(BRect(0.0, 0.0, 99.0, 99.0), B_RGB32)))
	//{
		//me_bitmap->SetBits((const void *)my_bitmap, (long int)sizeof(my_bitmap), 0, B_RGB32);
	//}
}

//
//		AboutView::~BeCam_AboutView()
BeCam_AboutView::~BeCam_AboutView(void)
{
	if(me_bitmap)
	{
		delete me_bitmap;
	}
}

//
//		AboutView :: Draw
void	BeCam_AboutView::Draw(BRect rect)
{
	//int32 		cnt;
	//rgb_color	black_color={0, 0, 0, 0xff};
	//rgb_color	white_color={255, 255, 255, 0xff};
	//rgb_color	bg_color=ui_color(B_PANEL_BACKGROUND_COLOR);


	//SetHighColor(black_color);
	//SetLowColor(bg_color);
	
	//MovePenTo(10, 43);
	//SetFontSize(25);
	//DrawString(_T(VERSIONNAME));
	//MovePenTo(300, 43);
	//DrawString(VERSIONNUMBER);
	
	//MovePenTo(10, 63);	
	//SetFontSize(12);
	//DrawString(_T(AUTHOR));
	
	//MovePenTo(10, 88);
	//SetFontSize(10);
	//DrawString(_T(DESCRIPTION));
	
	MovePenTo(0,0);
	DrawBitmap(me_bitmap);


	//		Draw film top edge
	//SetHighColor(black_color);
	//FillRect(BRect(105, 5, 395, 13));

	//		Draw film edge
	//SetHighColor(white_color);
	//for(cnt=0; cnt<22; cnt++)
	//{
	//	FillRect(BRect(111+(cnt*14), 7, 111+(cnt*14)+4, 11));
	//}


	//		Draw film bottom edge
	//SetHighColor(black_color);
	//FillRect(BRect(105, 96, 395, 104));
	
	//		Draw film edge
	//SetHighColor(white_color);
	//for(cnt=0; cnt<22; cnt++)
	//{
	//	FillRect(BRect(111+(cnt*14), 98, 111+(cnt*14)+4, 102));
	//}
}

//
//		AboutWindow :: Constructor
BeCam_AboutWindow::BeCam_AboutWindow(float xPos,float yPos,BeCam_MainWindow *mainWindow) : BWindow(BRect(xPos,yPos,xPos + WINDOW_WIDTH_ABOUT,yPos + WINDOW_HEIGHT_ABOUT), _T("About PhotoGrabber"), B_DOCUMENT_WINDOW, B_WILL_DRAW | B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE | B_NOT_RESIZABLE)
{
	parent = mainWindow;
	view = new BeCam_AboutView(xPos,yPos);
	AddChild(view);
}

//
//		AboutWin :: QuitRequested
bool BeCam_AboutWindow::QuitRequested()
{
	parent->aboutWindow=NULL;
	return BWindow::QuitRequested();
}

//
//		AboutWindow :: MessageRecieved
void BeCam_AboutWindow::MessageReceived(BMessage* message)
{
	switch(message->what)
	{
		default:
			BWindow::MessageReceived(message);
	}
}
