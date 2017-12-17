/*  bdcpConfig.cpp */


#include "bdcpConfig.h"
#include <TextView.h>
#include <View.h>
#include <TranslationKit.h>
#include <string.h>
#include <Roster.h>
#include <File.h>
#include <Resources.h>
#include "bdcpPTP.h"
#include "BitmapView.h"
#include "Bitmap.h"

PTPCnfWindow::PTPCnfWindow(BPoint middle)
	: BWindow(BRect(middle.x - 160, middle.y - 54, middle.x + 160, middle.y + 54), "Configure BDCP Plugin", B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE )
{
    BitmapView* CamView = new BitmapView('BBMP', 1, "camera");
    BSize size(CamView->PreferredSize());
    ResizeTo(size.width, size.height);
    middle -= BPoint(size.width / 2, size.height / 2);
    MoveTo(middle);
    CamView->ResizeToPreferred();
    CamView->SetViewColor(100,100,80);
    AddChild(CamView);
}

bool PTPCnfWindow::QuitRequested() {
    return true;
}
