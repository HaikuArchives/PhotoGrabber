/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/

//
// File defenition
#ifndef CORESYS_H
#define CORESYS_H

// OS
#define _HAIKU_OS_

//
// Includes
#include <Application.h>
#include <Deskbar.h>
#include <Roster.h>
#include <map.h>
#include <multimap.h>
#include <string.h>
#include <vector.h>
#include <FindDirectory.h>
#include <AppFileInfo.h>
#include <Window.h>

//
// Local Includes
#include "PluginInterface.h"
//#include "MainWindow.h"
#include "Camera.h"
#include "preferences.h"

//
// classes
class BeDiGiCamApp : public BApplication {
	public:
									BeDiGiCamApp(void);
									~BeDiGiCamApp(void);
		virtual void				MessageReceived(BMessage* message);
		virtual void				ReadyToRun(void);
		virtual bool				QuitRequested(void);
		bool						CreateGUI(void);
		int							GetDeviceType();
		// Configuration part
		bool						GetCameraStrings();
		bool						GetPluginDetails(char *camerastring);
		bool						OpenPluginConfig(char *camerastring, BPoint *interfacePoint);
		CamInterface 				*interface;
		vector<string> 				supportedCams;
		multimap<const char*,string>	pluginSupportedCams;
		version_info				plugininfo;
		BWindow						*pluginconfwindow;
		//
		char*						globalPath;
		char						*title,*bgcolor,*HTMLpath;
		int							column,picborder;
		class Camera				*camera;
		BWindow						*mainWindow;
		image_id 					addonId;
		SETTINGS                	pgsettings;
		
	private:
		int						LogError(int ErrorMes);
		void					Usage();
		void					VersionInfo();
		
		
};

// defenitions

#define CLI_OFFSET				55
#define CLI_WRONG_ARGUMENT		CLI_OFFSET+1
#define CLI_NO_ARGUMENT			CLI_OFFSET+2


#endif /* __CORESYS_H__ */

