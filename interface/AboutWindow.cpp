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
AboutTabView::AboutTabView(BRect r):BView(r, "abouttabview", B_FOLLOW_ALL_SIDES, B_WILL_DRAW)
{
	rgb_color bg_color=ui_color(B_PANEL_BACKGROUND_COLOR);
	SetViewColor(bg_color);
	BTranslatorRoster *roster = BTranslatorRoster::Default();
	me_bitmap= BTranslationUtils::GetBitmap("about", roster);
}

//
//		AboutView::~BeCam_AboutView()
AboutTabView::~AboutTabView(void)
{
	if(me_bitmap)
		delete me_bitmap;	
}

//
//		AboutView :: Draw
void	AboutTabView::Draw(BRect rect)
{	
	MovePenTo(2,2);
	DrawBitmap(me_bitmap);
}
//
//		Aboutview :: Constructor
BeCam_AboutView::BeCam_AboutView(float xPos, float yPos):BView(BRect(0,0,WINDOW_WIDTH_ABOUT- 50 + xPos,WINDOW_HEIGHT_ABOUT - 100 + yPos), "aboutview", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
	rgb_color bg_color=ui_color(B_PANEL_BACKGROUND_COLOR);
	SetViewColor(bg_color);
	BRect r;
	r = Bounds();
	r.InsetBy(2,2);
	tabView = new BTabView(r,"tab_view");
	tabView->SetViewColor(bg_color);
	r = tabView->Bounds();
	r.bottom -=tabView->TabHeight();
	aboutTab = new BTab();
	aboutView = new AboutTabView(r);
	tabView->AddTab(aboutView,aboutTab);
	aboutTab->SetLabel("About");
	creditsTab = new BTab();
	creditsView = new BView(r, "creditstabview", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	creditsView->SetViewColor(bg_color);
	//
	creditsTextView = new BTextView(r, "credits", r.OffsetToCopy(0, 0).InsetByCopy(5, 5), B_FOLLOW_ALL); 
    creditsTextView->SetFlags(creditsTextView->Flags() | B_FRAME_EVENTS ); 
    creditsTextView->SetStylable(true); 
    creditsTextView->MakeEditable(false); 
    creditsTextView->SetWordWrap(true);
    BScrollView *creditsScroller = new BScrollView("creditsScroller", creditsTextView, B_FOLLOW_ALL, B_WILL_DRAW | B_FRAME_EVENTS, false, true, B_PLAIN_BORDER); 
    creditsView->AddChild(creditsScroller);
    //
    rgb_color darkgrey = { 100, 100, 100, 255 }; 
    rgb_color pgGreen = { 42, 131, 36, 255 }; 
    rgb_color pgOrange = { 255, 69, 0, 255 }; 
    //rgb_color pgYellow = { 255, 176, 0, 255 }; 
    //rgb_color linkBlue = { 80, 80, 200, 255 }; 
	//
	BFont font(be_bold_font); 
    font.SetSize(font.Size() + 4); 
    creditsTextView->SetFontAndColor(&font, B_FONT_ALL, &pgGreen); 
    creditsTextView->Insert("PhotoGrabber\n");
    creditsTextView->SetFontAndColor(&font, B_FONT_ALL, &pgOrange); 
    creditsTextView->Insert("Team Lead:\n"); 
    creditsTextView->SetFontAndColor(be_plain_font, B_FONT_ALL, &darkgrey);
	creditsTextView->Insert( 
                "Jan-Rixt Van Hoye\n" 
                "Luc Schrijvers\n"
                "Ramshanker V\n" 
                "Tim de Jong\n" 
                "\n");
    creditsTextView->SetFontAndColor(&font, B_FONT_ALL, &pgOrange); 
    creditsTextView->Insert("Developers:\n"); 
    creditsTextView->SetFontAndColor(be_plain_font, B_FONT_ALL, &darkgrey);
    creditsTextView->Insert( 
                "Luc Schrijvers\n"
                "Ramshanker V\n" 
                "Tim de Jong\n" 
                "\n");
    creditsTextView->SetFontAndColor(&font, B_FONT_ALL, &pgOrange);
    creditsTextView->Insert("Special Thanks To:\n");
    creditsTextView->SetFontAndColor(be_plain_font, B_FONT_ALL, &darkgrey);
    creditsTextView->Insert("Bernd Korz\n");  
	//
	tabView->AddTab(creditsView,creditsTab);
	creditsTab->SetLabel("Credits");
	AddChild(tabView);
}

//
//		AboutView::~BeCam_AboutView()
BeCam_AboutView::~BeCam_AboutView(void)
{
	delete tabView;
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
