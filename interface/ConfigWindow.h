/*****************************************************************
 * Copyright (c) 2004-2008,	Jan-Rixt Van Hoye		 			 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.    			 *
 *****************************************************************/
//
// File defenition
#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H
//
//		Includes
#include <stdio.h>
#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <Box.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <MenuField.h>
#include <interface/TextView.h>
#include <interface/StringView.h>
#include <CheckBox.h>
#include <RadioButton.h>
#include <Button.h>
#include <map>
#include <string>
#include <AppFileInfo.h>

//
//		Local includes
#include "intf_global.h"
#include "core_global.h"
#include "settings.h"
#include "MainWindow.h"

//
//		Config View class
class BeCam_ConfigView : public BView
{
	public:
					BeCam_ConfigView(BRect r,float xPos,float yPos);
					~BeCam_ConfigView(void);
	private:
			
};

//
//		Config window Class
class BeCam_ConfigWindow : public BWindow
{
	public:
						BeCam_ConfigWindow(float xPos,float yPos,class BeCam_MainWindow *parent);
		virtual bool	QuitRequested();
		virtual void	MessageReceived(BMessage* message);

	private:
		BeCam_ConfigView	*becam_configView;
		//	Configuration section
		BBox				*becam_configbox;
		BPopUpMenu			*becam_cameraMenu;
		BMenuField			*becam_cameraPopup;
		BStringView			*becam_pluginName;
		BStringView			*becam_pluginSource;
		BStringView			*becam_pluginVersion;
		BButton				*becam_pluginConfig;
		//	Debug section
		BBox				*becam_debugbox;
		BCheckBox			*becam_checkTerminal;
		BCheckBox			*becam_checkFile;
		//	Screen section
		BButton				*becam_savebutton;
		BButton				*becam_cancelbutton;
		void				CreateCameraTypeMenu(BRect r);
		void				GetPluginDetails(char *cameraname = NULL);
		void				OpenPluginConfig();
		multimap<const char*,string>	*pluginSupportedCams;
		class BeCam_MainWindow    *parent;
		version_info		versioninfo;
		SETTINGS            *pgsettings;
};

#define	CONF_OFFSET				1055
#define CONF_BUT				CONF_OFFSET + 1
#define CONF_MITM				CONF_OFFSET + 2

// Size
#define CONFIG_WINDOW				4
#define WINDOW_HEIGHT_CONFIG		330
#define	WINDOW_WIDTH_CONFIG			350

#endif


