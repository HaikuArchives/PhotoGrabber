/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
#ifndef STATDOCK_H
#define STATDOCK_H
//
// Includes
#include <View.h>
#include <StringView.h>
#include <Invoker.h>
#include <Window.h>
#include <StatusBar.h>
//
// Local includes
#include "intf_global.h"
#include "core_global.h"
#include "IconView.h"
//
// defines
typedef enum {
    MODE_HIDDEN = -1,
    MODE_INIT,
    MODE_DOWNLOAD,
    MODE_CONNECTED,
} Modus;

#define SD_MIN_BITMAP_HEIGHT    48
#define SD_MIN_BITMAP_WIDTH		48
//
#define STATDOCK_SHOW			'SDSH'
#define STATDOCK_HIDE			'SDHI'
#define STATDOCK_SHOWED			'SDSD'
#define STATDOCK_HIDDEN			'SDHD'


//
//		StatusDock:: View
class StatusDock : public BView, BInvoker
{
public:
                                    StatusDock(const char* name);
                                    ~StatusDock();
					
    virtual void                    AttachedToWindow ();
    void                            UpdateStatus(float delta, const char *message);
    virtual void                    MessageReceived(BMessage* message);
    void                            Show(int newModus);
    void                            Show();
    void                            Hide();
    void                            SetMaxStatusBar(float maximum);
    void                            SetStatusMessage(const char* message);

private:
    BStringView*                    statusMessage;
    IconView*	                    statusIcon;
    BStatusBar*                     statusBar;	
    BBitmap*                        iconBitmaps[3];
    int                             modus;
    void                            CreateStatusBar();
};

#endif

