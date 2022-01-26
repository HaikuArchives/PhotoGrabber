/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// includes
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <Screen.h>
#include <File.h>
// local includes
#include "core_system.h"
#include "logger.h"
#include "MainWindow.h"
//
//BeDiGiCam::Application Constructor
BeDiGiCamApp::BeDiGiCamApp()
  : BApplication(APP_SIG),
    camera(NULL)
{
    //Initialization
    pgsettings = LoadSettingsFromFile();
}

BeDiGiCamApp::~BeDiGiCamApp() {
    camera->PostMessage(CLOSE_DEVICE);
    SaveSettingsToFile(pgsettings);
}


// 
// 	BeDiGiCam::The application is running, time to startup the rest
void 
BeDiGiCamApp::ReadyToRun() {
    // Create a camera object if a plugin has been chosen
    CreateGUI();
    if (pgsettings.pluginName != NULL) {
        LogDebug("CORE - Create camera looper.");
        camera = new Camera(pgsettings.pluginName);
        camera->Start();
        // Open the device
        if (GetDeviceType() == TYPE_USB) {
            camera->PostMessage(new BMessage(OPEN_DEVICE));
        }
    }
}
// 	
//	BeDiGiCam::Messages to the application
void 
BeDiGiCamApp::MessageReceived(BMessage* message) {
    switch (message->what) {
        case CAM_CONNECTED: 
            LogDebug("CORE - Camera connected.");
            mainWindow->PostMessage(message);
            if (GetDeviceType() == TYPE_USB) {
                BMessage *cammessage = new BMessage(GET_ITEMS);
                camera->PostMessage(cammessage);
            }
            break;
        case GET_ITEMS_START:
        case GET_ITEMS_DONE:
        case CAM_SHOT_TAKEN:
            mainWindow->PostMessage(message);
            break;
        case CAM_DISCONNECTED:
            LogDebug("CORE - Camera disconnected.");
            mainWindow->PostMessage(message);
            break;
        case CAM_CONNECT: {
            BMessage reply(CAM_CONNECT_OK);
            message->SendReply(&reply);
            if (GetDeviceType() == TYPE_PAR) {
                BMessage *cammessage = new BMessage(GET_ITEMS);
                // Open the device
                bool open;
                BMessage reply;
                BMessenger messenger(NULL, camera);
                messenger.SendMessage(new BMessage(OPEN_DEVICE), &reply);
                if (reply.what == OPEN_DEVICE) 
                    reply.FindBool("open", &open);
                if (open)
                    camera->PostMessage(cammessage);
                else {
                    BMessage reply(CAM_CONNECT_FAIL);
                    message->SendReply(&reply);
                }
            } else if (GetDeviceType() == TYPE_USB) {
            	if (this->interface)
                	this->interface->setCoreSystemLoop(this);
            }
            break;
        }
        case CAM_DISCON: 
            camera->PostMessage(new BMessage(CLOSE_DEVICE));
            camera->Stop();
            break;
        case ADD_ITEM:
        case B_COPY_TARGET:
            mainWindow->PostMessage(message);
            break;
        case DOWN_ITEM:
        case REM_ITEM:
        case CHECK_FUNCTION:
        case CAM_TAKE_SHOT:
            camera->PostMessage(message);
            break;
        case RELOAD_CONFIGURATION:
            LogDebug("CORE - Reloading the new plugin.");
            // Close the old plugin
            camera->PostMessage(new BMessage(CLOSE_DEVICE));
            // Get new Camera Interface
            camera->PostMessage(message);
            // Open the device
            if (GetDeviceType() == TYPE_USB) 
                camera->PostMessage(new BMessage(OPEN_DEVICE));
                // Change the action menu items in the interface
            mainWindow->PostMessage(message);
            break;
            
        case GET_CONFIGURATION: {
            BMessage reply;
            reply.AddPointer("settings", &pgsettings);
            message->SendReply(&reply);
            break;
        }
        case SAVE_CONFIGURATION:
            LogDebug("CORE - Saving the new settings.");
            SaveSettingsToFile(pgsettings);
            break;
        case GET_CAMSTRING: {
            BMessage reply;
            if (GetCameraStrings()) {
                reply.AddPointer("camstrings", &supportedCams);
                reply.AddPointer("pluginstrings", &pluginSupportedCams);
                message->SendReply(&reply);
            }
            break;
        }
        case GET_PLUG_DETAIL: {
            const char *camerastring;
            BMessage reply;
            camerastring = message->FindString("camerastring");
            if (GetPluginDetails((char *) camerastring)) {
                reply.AddPointer("details", &plugininfo);
               message->SendReply(&reply);
            }
            break;
        }
        case PLUG_CONFIG_PRESENT: {
            const char *camerastring;
            BMessage reply;
            camerastring = message->FindString("camerastring");
            bool present = IsPluginConfigPresent((char *) camerastring);
            reply.AddBool("present", present);
            message->SendReply(&reply);
            break;
        }
        case OPEN_PLUG_WINDOW: {
            const char *camerastring;
            BPoint interfacepoint;
            camerastring = message->FindString("camerastring");
            message->FindPoint("point", &interfacepoint);
            OpenPluginConfig((char *) camerastring, &interfacepoint);
            break;
        }
        case GET_DEVTYPE: {
            BMessage reply;
            int devtype = GetDeviceType();
            reply.AddInt32("devtype", devtype);
            message->SendReply(&reply);
            break;
        }
        default:
            BApplication::MessageReceived(message);
    }
}
// 
//	BeDiGiCam:: MainWindow of the Application
bool
BeDiGiCamApp::CreateGUI() {
    LogDebug("CORE - Create GUI.");
    BEntry appentry; 
    BPath path;
    int32 devType = GetDeviceType();
    // get the path of the application
    LogDebug("CORE - Device type is: %d.",devType);

    mainWindow = new BeCam_MainWindow(this, devType);
    mainWindow->Show();
    return B_OK;
}
//
//	BeDiGiCam:: Check the device type
int 
BeDiGiCamApp::GetDeviceType() {
    LogDebug("CORE - Get device type.");
    int32 type = 0;
    BMessage reply;
    BMessenger messenger(NULL,camera);
    messenger.SendMessage(new BMessage(GET_DEVICE_TYPE),&reply);
    if(reply.what == GET_DEVICE_TYPE)
	reply.FindInt32("type", &type);
    return type;	
}
//
//	BeDiGiCam:: Check the device type
bool 
BeDiGiCamApp::GetCameraStrings() {
    char *addonName;
    BEntry appentry;
    BPath path;
    image_id lAddonId;
    // get the path of the application
    app_info info; 
    GetAppInfo(&info); 
    appentry.SetTo(&info.ref); 
    appentry.GetPath(&path);
    path.GetParent(&path);
    path.Append("plugins/");
    BDirectory searchDir(path.Path());
    BEntry entry;
    status_t err = B_OK;
    //clear the lists
    supportedCams.clear();
    pluginSupportedCams.clear();
    while((err = searchDir.GetNextEntry(&entry,true)) == B_OK) {
	if (entry.InitCheck() != B_NO_ERROR)
	break;
    	if (entry.GetPath(&path) != B_NO_ERROR) {
	    LogDebug( "CORE - entry.GetPath failed." );
	} else {
	    addonName = new char[B_FILE_NAME_LENGTH];
	    entry.GetName(addonName);
	    lAddonId = load_add_on(path.Path());
	    if(lAddonId > 0) {
			unload_add_on(lAddonId);
			interface = new CamInterface(addonName);
			vector<string> plugincamstrings = interface->getCameraStrings();
			vector<string>::iterator i = plugincamstrings.begin();
			while(i != plugincamstrings.end())
			{
			    char *dummy = new char[B_FILE_NAME_LENGTH];
			    strncpy(dummy,(*i).c_str(), B_FILE_NAME_LENGTH);
			    supportedCams.push_back(dummy);
			    pluginSupportedCams.insert(pair<const char* const,string>(addonName,(*i)));
			    i++;
			}
			delete(interface);
	    } else
		LogDebug("CORE - loading %s failed!",addonName);
	}
    }
    return true;
}
//
//	BeDiGiCam:: Check the device type
bool 
BeDiGiCamApp::GetPluginDetails(char *camerastring) {
    // save the plugin name
    multimap<const char*,string>::iterator i = pluginSupportedCams.begin();
    while(i != pluginSupportedCams.end()) {
	//	TODO should be revised :: FindMarked->label must be put in a variable first
	if(!strcmp((*i).second.c_str(),camerastring)) {
	    interface = new CamInterface((char*)(*i).first);
	    plugininfo = interface->getVersion();
	    delete(interface);
	    return true;
	}
	i++;
    }
    return false;
}
//
//	BeDiGiCam:: Check the device type
bool BeDiGiCamApp::OpenPluginConfig(char *camerastring,BPoint *interfacePoint) {
    BPoint lPoint = *interfacePoint;
    for (multimap<const char*, string>::iterator i = pluginSupportedCams.begin(); i != pluginSupportedCams.end(); i++) {
	if(!strcmp((*i).second.c_str(),camerastring)) {
	    interface = new CamInterface((char*)(*i).first);
	    if (interface->hasFunction(fn_configurePlugin)) 
		if (pluginconfwindow = interface->pluginConfiguration(lPoint))
		    pluginconfwindow->Show();
	    return true;
	}
    }
    return false;
}
//
//	BeDiGiCam:: Check if there is a plugin configuration screen
bool 
BeDiGiCamApp::IsPluginConfigPresent(char *camerastring) {
    for (multimap<const char*, string>::iterator i = pluginSupportedCams.begin();
            i != pluginSupportedCams.end(); i++) {
	if (!strcmp((*i).second.c_str(), camerastring))
	    return interface->hasFunction(fn_configurePlugin);
    }
    return false;
}
//
//	BeDiGiCam::main
int main() {
    BeDiGiCamApp app;
    app.Run();
    return 0;
}
