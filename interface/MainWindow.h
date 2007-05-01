/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
//
// File defenition
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//
// local Includes
#include "intf_global.h"
#include "Item.h"
#include "MainView.h"
#include "ListView.h"
#include "StatusWindow.h"
#include "AboutWindow.h"
#include "ConfigWindow.h"
#include "settings.h"
#include "debug.h"
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
#ifdef _ZETA_OS_
	#include <interface/IconMenu.h>
	#include <experimental/ToolBar.h>	
	#include <experimental/BitmapTool.h>
	#include <sys_apps/Tracker/Icons.h>
#endif
#include <interface/Bitmap.h>
#include <app/Looper.h>
#include <Path.h>


//		PTP wrapper class
class BeCam_MainWindow : public BWindow
{
	public:
							BeCam_MainWindow(BRect r,BLooper *syscore,int devtype);
		virtual bool		QuitRequested();
		virtual void		MessageReceived(BMessage* message);
		// Other interface windows
		class BeCam_StatusWindow	*statusWindow;
		class BeCam_AboutWindow		*aboutWindow;
		class BeCam_ConfigWindow	*configWindow;
		// system core looper
		BLooper			*systemcore;
		
	private:
	
		bool				becam_connected;
		int					logMainWindowError(int ErrorMes);
		void				addItem(class BeCam_Item *item);
		void				removeItem(class BeCam_Item *item);
		void				clearItems();
		void				downloadSelectedItems(entry_ref *copyToDir = NULL);
		void				removeSelectedItems();
		void				loadButtonPictures();
		void				addMenuBar();
		#ifdef _ZETA_OS_
			void				addToolBar();
		#else
			void				addActionBar();
		#endif
		void				addStatusBar();
		void                CreateConfigWindow();
		void                CreateAboutWindow();
		void                CreateStatusWindow(int32 totalbytes, char *message);
		void                UpdateStatusWindow(uint32 delta, char *message);
		void                CloseStatusWindow(void);
		void                CalculatePos(float *xPos,float *yPos,int winType);
		// GUI components
		BeCam_ListView		*becam_listview;
		BeCam_MainView		*becam_view;
		BMenuBar			*becam_menubar;
		BMenu				*becam_iconMenu;
		BMenu				*becam_fileMenu;
		BMenu				*actionsMenu;
		BMenu				*becam_extraMenu;
		BScrollView			*becam_scrollview;
        BTextControl		*becam_winstatusbar;
        #ifdef _ZETA_OS_
        	BToolBar			*becam_toolBar;
			BTool				*becam_tbDownload;
			BTool				*becam_tbRemove;
			BTool				*becam_tbDetails;
		#else
			BButton				*becam_download;
			BButton				*becam_delete;
		#endif
		// the camera interface
		BMenuField			*becam_downloadPopup;
		BPopUpMenu			*becam_downloadMenu;
		BMenuItem 			*defaultPath;
		BFilePanel 			*becam_selectdirpanel;
		int					devicetype;
		SETTINGS			*pgsettings;
};

// Main Window error messages

#define	MAINW_OFFSET				455
#define MAINW_DEV_NO_FIND			MAINW_OFFSET + 1

// Size
#define MAIN_WINDOW					3
#define WINDOW_HEIGHT_MAIN			450
#define	WINDOW_WIDTH_MAIN			350
#endif

