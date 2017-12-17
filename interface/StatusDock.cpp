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
#include <LayoutBuilder.h>
#include <Bitmap.h>
#include <Button.h>
//
//		Status Bar:: Local includes
#include "StatusDock.h"
#include "intf_global.h"
#include "logger.h"

//
// 		StatusDock::Constructor of the view
StatusDock::StatusDock(const char* name)
  : BView(name, B_WILL_DRAW | B_FRAME_EVENTS),
    modus(MODE_INIT)
{
    SetExplicitMaxSize(BSize(B_SIZE_UNSET, 75));
    SetViewUIColor(B_PANEL_BACKGROUND_COLOR);
    BLayoutBuilder::Grid<>(this, B_USE_ITEM_SPACING, 0)
        .SetExplicitAlignment(BAlignment(B_ALIGN_USE_FULL_WIDTH, B_ALIGN_BOTTOM))
        .SetInsets(B_USE_WINDOW_INSETS)
        .Add(statusIcon = new IconView("statusicon"), 0, 0, 1, 4)
        .AddGlue(1, 0, 2)    
        .Add(statusMessage = new BStringView("statusmsg", NULL), 1, 1)
        .AddGlue(2,1)
        .Add(statusBar = new BStatusBar("statusbar"), 1, 2, 2)
    .End();
    statusIcon->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_VERTICAL_CENTER));
    statusMessage->SetExplicitAlignment(BAlignment(B_ALIGN_USE_FULL_WIDTH, B_ALIGN_VERTICAL_CENTER));
    statusMessage->SetExplicitPreferredSize(BSize(B_SIZE_UNLIMITED, B_SIZE_UNSET));
    statusBar->SetExplicitAlignment(BAlignment(B_ALIGN_USE_FULL_WIDTH, B_ALIGN_VERTICAL_CENTER));
    statusIcon->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT, B_ALIGN_VERTICAL_CENTER));
}
//
//	StatusDock :: Destructor
StatusDock::~StatusDock() {
    for (int i=0; i<3; i++)
        delete iconBitmaps[i];
}
//
//	StatusDock :: AttachedToWindow()
void 
StatusDock::AttachedToWindow() {
    LogDebug("STATUSDOCK - Attached to window.");
    if(Bounds().Height() >= 75) {
        iconBitmaps[MODE_DOWNLOAD] = BTranslationUtils::GetBitmap('PNG ', "download_64_on");
        iconBitmaps[MODE_CONNECTED] = BTranslationUtils::GetBitmap('PNG ', "cam_64_on");
        iconBitmaps[MODE_INIT] = BTranslationUtils::GetBitmap('PNG ', "cam_64_off");
    } else {
        iconBitmaps[MODE_DOWNLOAD] = BTranslationUtils::GetBitmap('PNG ', "download_48_on");
        iconBitmaps[MODE_CONNECTED] = BTranslationUtils::GetBitmap('PNG ', "cam_48_on");
        iconBitmaps[MODE_INIT] = BTranslationUtils::GetBitmap('PNG ', "cam_48_off");
    }
    statusIcon->SetIcon(iconBitmaps[modus]);
    SetMaxStatusBar(100);
    Layout(true);
}

//
//		StatusDock::MessageRecieved
void 
StatusDock::MessageReceived(BMessage* message) {
    switch(message->what) {
        case STATDOCK_SHOW: {
            int32 modus = message->FindInt32("modus");
            const char* statusmsg = message->FindString("statusmessage");
            float total = message->GetFloat("total", -1);
            SetMaxStatusBar(total);
            SetStatusMessage(statusmsg);
            Show(modus);
            break;
        }    
        case UPDATE_STAT: {
            const char *statusmsg = message->FindString("statusmessage");
            float count = message->FindFloat("count");
            UpdateStatus(count, statusmsg);
            break;
        }
        default:
            BView::MessageReceived(message);
    }
}
//
//		StatusDock::Update Status		
void
StatusDock::UpdateStatus(float delta, const char *message) {
    LogDebug("STATUSDOCK - Update status.");

    if (message)
        SetStatusMessage(message);
    if (delta > 0)
        statusBar->Update(delta);
    Invalidate();
}
//
//	StatusDock :: Show the needed controls
void	
StatusDock::Show(int newModus) {
    LogDebug("STATUSDOCK - Show Children().");
	
    if (modus == newModus) return;
    switch(newModus) {
        case MODE_CONNECTED:
        case MODE_INIT:
            LogDebug("STATUSDOCK - ShowChildren() - Init mode.");
            if (IsHidden()) 
                Show();
            break;
        case MODE_DOWNLOAD:
            LogDebug("STATUSDOCK - ShowChildren() - Download mode.");
            if (IsHidden())
                Show();
            break;
        case MODE_HIDDEN:
            LogDebug("STATUSDOCK - ShowChildren() - Hidden.");
            if (!IsHidden()) 
                Hide();
    }
    modus = newModus;
    statusIcon->SetIcon(iconBitmaps[modus]);
    Invalidate();
}

//
//	StatusDock :: Show
void 
StatusDock::Show() {
    LogDebug("STATUSDOCK - Show Dock.");
    BView::Show();
    Window()->PostMessage(STATDOCK_SHOWED);
}
//
//	StatusDock :: Hide
void StatusDock::Hide() {
    LogDebug("STATUSDOCK - Hide Dock.");
    BView::Hide();
    Window()->PostMessage(STATDOCK_SHOWED);
}
//
//	StatusDock :: SetMaxStatusBar
void 
StatusDock::SetMaxStatusBar(float maximum) {
    LogDebug("STATUSDOCK - Set max statusbar.");
    if (maximum == statusBar->MaxValue()) return;
    if (maximum >= 0) {
        statusBar->Reset();
        statusBar->SetMaxValue(maximum);
        if (statusBar->IsHidden()) {
            statusBar->Show();
            Window()->Layout(true);
        }
    } else {
        statusBar->Hide();
    }       
    statusBar->SetMaxValue(maximum);
}
//
//	StatusDock :: SetStatusMessage
void 
StatusDock::SetStatusMessage(const char* message) {
    statusMessage->SetText(message);
}
