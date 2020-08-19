/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// 		System includes
// 
//		Local Includes
#include "AboutWindow.h"
#include <Application.h>
#include <Roster.h>
//
//		Aboutview :: Constructor
AboutBitmapView::AboutBitmapView(BRect r)
  : BView(r, "aboutbitmapview", B_FOLLOW_ALL_SIDES, B_WILL_DRAW)
{
    rgb_color bg_color=ui_color(B_PANEL_BACKGROUND_COLOR);
    SetViewColor(bg_color);
    BTranslatorRoster *roster = BTranslatorRoster::Default();
    about_bitmap= BTranslationUtils::GetBitmap("about", roster);
}

//
//		AboutBitmapView::~BeCam_AboutBitmapView()
AboutBitmapView::~AboutBitmapView(void) {
    if (about_bitmap)
	delete about_bitmap;	
}

//
//		AboutView :: Draw
void	AboutBitmapView::Draw(BRect rect) {	
    rgb_color color_border = {0x8b, 0x8b, 0x83, 0xff};
    MovePenTo(0,0);
    DrawBitmap(about_bitmap);
    SetPenSize(1);
    SetHighColor(color_border);
    SetLowColor(color_border);
    StrokeLine(BPoint(0,75),BPoint(400,75),B_SOLID_HIGH);
}
//
//		Aboutview :: Constructor
BeCam_AboutView::BeCam_AboutView(BRect rect)
  : BView(rect, "aboutview", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
    rgb_color bg_color=ui_color(B_PANEL_BACKGROUND_COLOR);
    SetViewColor(bg_color);
    BRect r;
    r = Bounds();
    r.bottom = r.top + 75;
    aboutBitmapView = new AboutBitmapView(r);
    AddChild(aboutBitmapView);
    r = Bounds();
    r.top += 76 ;
    creditsView = new BView(r, "creditsview", B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
    creditsView->SetViewColor(bg_color);
    //
    r= creditsView->Bounds();
    r.right -= B_V_SCROLL_BAR_WIDTH;
    creditsTextView = new BTextView(r, "credits", r.OffsetToCopy(0, 0).InsetByCopy(5, 5), B_FOLLOW_ALL); 
    creditsTextView->SetFlags(creditsTextView->Flags() | B_FRAME_EVENTS );
    creditsTextView->SetStylable(true); 
    creditsTextView->MakeEditable(false); 
    creditsTextView->SetWordWrap(true);
    BScrollView *creditsScroller = new BScrollView("creditsScroller", creditsTextView, B_FOLLOW_ALL, B_WILL_DRAW | B_FRAME_EVENTS, false, true, B_FANCY_BORDER); 
    creditsView->AddChild(creditsScroller);
    //
    rgb_color pgDarkgrey = { 100, 100, 100, 255 }; 
    rgb_color pgGreen = { 42, 131, 36, 255 }; 
    rgb_color pgOrange = { 255, 69, 0, 255 };
    rgb_color pgYellow = { 255, 176, 0, 255 };
    rgb_color pgBlue = { 80, 80, 200, 255 }; 
    //
    // Get version info
    app_info appInfo;
    version_info versionInfo;
    BAppFileInfo appFileInfo;
    BFile appFile;

    char version[20] = "\t\t0.0.0\n";

    if (be_app->GetAppInfo(&appInfo) == B_OK &&
            appFile.SetTo(&appInfo.ref, B_READ_ONLY) == B_OK &&
            appFile.InitCheck() == B_OK &&
            appFileInfo.SetTo(&appFile) == B_OK &&
            appFileInfo.GetVersionInfo(&versionInfo, B_APP_VERSION_KIND) == B_OK)
        snprintf(version, sizeof(version), "\t\t%d.%d.%d\n",versionInfo.major, versionInfo.middle, versionInfo.minor);

    BFont font(be_bold_font); 
    font.SetSize(font.Size() + 4); 
    creditsTextView->SetFontAndColor(&font, B_FONT_ALL, &pgGreen); 
    creditsTextView->Insert("PhotoGrabber\n");
    creditsTextView->SetFontAndColor(be_plain_font, B_FONT_ALL, &pgBlue);
    creditsTextView->Insert("\thttps://github.com/HaikuArchives/PhotoGrabber\n\n");
    creditsTextView->SetFontAndColor(&font, B_FONT_ALL, &pgOrange);
    creditsTextView->Insert(_T("\tVersion:\n")); 
    creditsTextView->SetFontAndColor(be_plain_font, B_FONT_ALL, &pgDarkgrey);
    creditsTextView->Insert(version);
    creditsTextView->SetFontAndColor(&font, B_FONT_ALL, &pgOrange); 
    creditsTextView->Insert(_T("\tTeam Lead:\n")); 
    creditsTextView->SetFontAndColor(be_plain_font, B_FONT_ALL, &pgDarkgrey);
    creditsTextView->Insert( "\t\tJan-Rixt Van Hoye\n"); 
    creditsTextView->SetFontAndColor(&font, B_FONT_ALL, &pgOrange); 
    creditsTextView->Insert(_T("\tDevelopers:\n")); 
    creditsTextView->SetFontAndColor(be_plain_font, B_FONT_ALL, &pgDarkgrey);
    creditsTextView->Insert( 
                "\t\tJan-Rixt Van Hoye\n"
                "\t\tLuc Schrijvers\n"
                "\t\tRamshanker V\n" 
                "\t\tTim de Jong\n"
				"\t\tKai Niessen\n");
    creditsTextView->SetFontAndColor(&font, B_FONT_ALL, &pgOrange);
    creditsTextView->Insert(_T("\tSpecial Thanks To:\n"));
    creditsTextView->SetFontAndColor(be_plain_font, B_FONT_ALL, &pgDarkgrey);
    creditsTextView->Insert(
    			"\t\tBernd Korz\n"
    			"\t\tFrancois Revol\n");
    // Copyrights
    font.SetSize(be_bold_font->Size() + 4);
	font.SetFace(B_BOLD_FACE);
	creditsTextView->SetFontAndColor(&font, B_FONT_ALL, &pgGreen);
	creditsTextView->Insert("\nCopyrights\n\n"); 
    // libexif copyright
	font.SetFace(B_BOLD_FACE | B_ITALIC_FACE);
    creditsTextView->SetFontAndColor(&font, B_FONT_ALL, &pgYellow);
    creditsTextView->Insert("\tlibexif\n");
    creditsTextView->SetFontAndColor(be_plain_font, B_FONT_ALL, &pgDarkgrey);
    creditsTextView->Insert("\t\tCopyright " B_UTF8_COPYRIGHT " 2001 Lutz Müller\n\t\t<lutz@users.sourceforge.net>\n"); 
	AddChild(creditsView);
}

//
//		AboutView::~BeCam_AboutView()
BeCam_AboutView::~BeCam_AboutView(void)
{
		//nothing yet
}
//
//		AboutWindow :: Constructor
BeCam_AboutWindow::BeCam_AboutWindow(float xPos,float yPos,BeCam_MainWindow *mainWindow) : BWindow(BRect(xPos,yPos,xPos + WINDOW_WIDTH_ABOUT,yPos + WINDOW_HEIGHT_ABOUT), _T("About PhotoGrabber"), B_TITLED_WINDOW, B_WILL_DRAW | B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE | B_NOT_RESIZABLE)
{
	parent = mainWindow;
	view = new BeCam_AboutView(Bounds());
	AddChild(view);
}

//
//		AboutWin :: QuitRequested
bool BeCam_AboutWindow::QuitRequested()
{
	parent->aboutWindow=NULL;
	return BWindow::QuitRequested();
}
