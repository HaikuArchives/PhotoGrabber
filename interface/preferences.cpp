/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
// 
// Includes
#include <storage/StorageDefs.h>
//
// Local Includes
#include "preferences.h"
#include "prefsaver.h"

//
//		External variables
extern class BeDiGiCamApp *app;

//
//		Load settings from file
SETTINGS LoadSettingsFromFile(void)
{
	BeCam_PrefSaver prefs("photograbber.prefs",10);	
	SETTINGS pgsettings;
    char tempbuf[10];
	//	Default download path
	char tempbufName[B_FILE_NAME_LENGTH];
	if (prefs.GetString("defaultdownpath", tempbufName, B_FILE_NAME_LENGTH))
	{
		strncpy(pgsettings.defaultDownloadPath,tempbufName,B_FILE_NAME_LENGTH);
	}
	else
	{
		printf("PREF - problem load default download path setting: %s\n",tempbuf);
		strcpy(pgsettings.defaultDownloadPath,"/boot/home/Pictures/PhotoGrabber");
	}
	//	Get the plugin name
	if (prefs.GetString("pluginname", tempbufName,B_FILE_NAME_LENGTH))
	{
		strncpy(pgsettings.pluginName,tempbufName,B_FILE_NAME_LENGTH);
	}
	else
	{
		printf("PREF - problem load plugin name: %s\n",tempbufName);
		strcpy(pgsettings.pluginName,"error");
	}
	//	Get the camera name
	if (prefs.GetString("cameraname", tempbufName,B_FILE_NAME_LENGTH))
	{
		strncpy(pgsettings.deviceName,tempbufName,B_FILE_NAME_LENGTH);
	}
	else
	{
		printf("PREF - problem load camera name: %s\n",tempbufName);
		strcpy(pgsettings.deviceName,"error");
	}
	return pgsettings;
}
//
//		Save the settings to a file
void SaveSettingsToFile(SETTINGS pgsettings)
{
	BeCam_PrefSaver prefs("photograbber.prefs",10); 
	// save the default download path
	prefs.AddField("defaultdownpath",pgsettings.defaultDownloadPath);
	//	save the plugin name for bdcp cameras
    prefs.AddField("pluginname",pgsettings.pluginName);
	//	save the camera name for bdcp cameras
	prefs.AddField("cameraname",pgsettings.deviceName);
	//	
	prefs.SaveFile(); 
}
