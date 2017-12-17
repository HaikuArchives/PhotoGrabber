/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// File definition
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//
// local Includes
#include "intf_global.h"
#include "PicItem.h"
#include "GridView.h"
#include "AboutWindow.h"
#include "ConfigWindow.h"
#include "settings.h"
#include "StatusDock.h"
//
//		Includes
#include <Window.h>
#include <ScrollView.h>
#include <Button.h>
#include <MenuBar.h>
#include <Menu.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <TextControl.h>
#include <FilePanel.h>
#include <Alert.h>
#include <PopUpMenu.h>
#include <interface/Bitmap.h>
#include <app/Looper.h>
#include <Path.h>
//
//	Struct 
struct items_data {
	BeCam_MainWindow            *window;
	entry_ref                   downloadDir;
	void                        (*Debug)(const char *,...);
};

//		Main window class
class BeCam_MainWindow : public BWindow {
public:
								BeCam_MainWindow(BLooper *syscore,int devtype);
	virtual bool				QuitRequested();
	virtual void				MessageReceived(BMessage* message);
	// Other interface windows
	class BeCam_AboutWindow		*aboutWindow;
	class BeCam_ConfigWindow	*configWindow;
	// system core looper
	BLooper						*systemcore;
		
private:
	bool						becam_connected;
	int							logMainWindowError(int ErrorMes);
	void						addItem(class PicItem *item);
	void						removeItem(class PicItem *item);
	void						clearItems();
	void						downloadSelectedItems(entry_ref copyToDir, const char *fileName = NULL);
	void						removeSelectedItems();
	static status_t                                 RemoveItems(items_data *data);
	static status_t             DownloadItems(items_data *data);
	void                        addMenuBar();
    void                        adaptActionMenu(bool enabled);
	void                        CreateActionDock();
	void                        CreateStatusDock();
	void                        CreateConfigWindow();
	void                        CreateAboutWindow();
	void                        ShowStatusDock(Modus modus, const char *status = NULL, float total = -1.0f);
	void                        UpdateStatusDock(uint32 delta, const char *message = NULL);
	void						CalculatePos(float *xPos,float *yPos,int winType);
	// GUI components
	GridView					*fGridView;
	BMenuBar*					fMenubar;
	BMenu						*becam_fileMenu;
	BMenu						*becam_actionsMenu;
	BMenu						*becam_viewMenu;
	BMenuItem					*sortTitleMenu;
	BMenuItem					*sortDateMenu;
	BScrollView					*fGridScrollView;
	StatusDock					*fStatusDock;
	int							devicetype;
	PG_Settings					*pgsettings;
};

// Main Window error messages

#define	MAINW_OFFSET				455
#define MAINW_DEV_NO_FIND			MAINW_OFFSET + 1

// Size
#define MAIN_WINDOW					3
#define WINDOW_HEIGHT_MAIN			550
#define	WINDOW_WIDTH_MAIN			700


#endif

