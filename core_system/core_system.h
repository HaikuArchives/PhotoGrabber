/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// File defenition
#ifndef CORESYS_H
#define CORESYS_H

//
// Includes
#include <Application.h>
#include <Deskbar.h>
#include <Roster.h>
#include <map>
#include <string>
#include <vector>
#include <FindDirectory.h>
#include <AppFileInfo.h>
#include <Window.h>

//
// Local Includes
#include "PluginInterface.h"
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
		bool						IsPluginConfigPresent(char *camerastring);
		// Debugging functions
		static void					Debug(const char *message,...)
		{
			char debugString[1024];
	
			va_list arglist;
			va_start(arglist,message);
	
			struct tm *current;
			time_t now;
			time(&now);
			current = localtime(&now);
	
			//if(pgsettings.debugFile)
			//{
				FILE *lfcore = fopen(LOGFILE,"a");
				vsprintf(debugString,message,arglist);
				fprintf(lfcore,"%i:%i:%i:: %s",current->tm_hour,current->tm_min, current->tm_sec,debugString);
				fclose(lfcore);
			//}
			va_end(arglist);
		}
		CamInterface 				*interface;
		std::vector<std::string>	supportedCams;
		std::multimap<const char*,std::string>	pluginSupportedCams;
		version_info				plugininfo;
		BWindow						*pluginconfwindow;
		//
		char*						globalPath;
		char						*title,*bgcolor,*HTMLpath;
		int							column,picborder;
		class Camera				*camera;
		BWindow						*mainWindow;
		image_id 					addonId;
		
	private:
		SETTINGS            	pgsettings;
		
		
};

// defenitions

#define CLI_OFFSET				55


#endif /* __CORESYS_H__ */

