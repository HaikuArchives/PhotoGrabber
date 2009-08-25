/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <Screen.h>
#include <File.h>
// local includes
#include "core_system.h"
//
// External variables
class BeDiGiCamApp *app;
FILE *lfcore;
//
//BeDiGiCam::Application Constructor
BeDiGiCamApp::BeDiGiCamApp()
		: BApplication(APP_SIG)
{
	//Initialization
	camera = NULL;
	globalPath = new char[512];
	title =	new char[512];
	bgcolor = new char[512];
	strcpy(globalPath,"/boot/home/Desktop");
}
// 
//	BeDiGiCam::Destructor (cleaning up)
BeDiGiCamApp::~BeDiGiCamApp()
{
	delete[] globalPath;
	delete[] title;
	delete[] bgcolor;
}
// 
// 	BeDiGiCam::The application is running, time to startup the rest
void BeDiGiCamApp::ReadyToRun()
{
	pgsettings = LoadSettingsFromFile();
	// Create a camera object if a plugin has been chosen
	if(pgsettings.pluginName != NULL)
	{
		#ifdef DEBUG
			lfcore = fopen(LOGFILE,"a");
			fprintf(lfcore,"CORE - Create camera looper.\n");
			fclose(lfcore);
		#endif
		camera = new Camera(pgsettings.pluginName);
		camera->Start();
		// Open the device
		if(GetDeviceType() == TYPE_USB)
		{
			camera->PostMessage(new BMessage(OPEN_DEVICE));
		}
	}
	CreateGUI();
}
// 
// 	BeDiGiCam::The application is running, time to startup the rest
bool BeDiGiCamApp::QuitRequested()
{
	return true;
}
// 	
//	BeDiGiCam::Messages to the application
void BeDiGiCamApp::MessageReceived(BMessage* message)
{	
	switch(message->what)
		{
			case CAM_CONNECTED:
			{
				#ifdef DEBUG
					lfcore = fopen(LOGFILE,"a");
					fprintf(lfcore,"CORE - Camera connected.\n");
					fclose(lfcore);
				#endif
				mainWindow->PostMessage(message);
				if(GetDeviceType() == TYPE_USB)
				{	
					BMessage *cammessage = new BMessage(GET_ITEMS);
					camera->PostMessage(cammessage);
				}
				break;
			}
			case GET_ITEMS_DONE:
				mainWindow->PostMessage(message);
				break;
			case CAM_DISCONNECTED:
			{	
				#ifdef DEBUG
					lfcore = fopen(LOGFILE,"a");
					fprintf(lfcore,"CORE - Camera disconnected.\n");
					fclose(lfcore);
				#endif
				mainWindow->PostMessage(message);
				break;
			}
			case CAM_CONNECT:
			{
				BMessage reply(CAM_CONNECT_OK);
				message->SendReply(&reply);
				if(GetDeviceType() == TYPE_PAR)
				{	
					BMessage *cammessage = new BMessage(GET_ITEMS);
					// Open the device
					bool open;
   					BMessage reply;
    				BMessenger messenger(NULL,camera);
					messenger.SendMessage(new BMessage(OPEN_DEVICE),&reply);
					if(reply.what == OPEN_DEVICE)
						reply.FindBool("open", &open);
					if(open)
						camera->PostMessage(cammessage);
					else
					{
						BMessage reply(CAM_CONNECT_FAIL);
						message->SendReply(&reply);
					}
				}
				break;
			}
			case CAM_DISCON:
			{
				camera->PostMessage(new BMessage(CLOSE_DEVICE));
				camera->Stop();
				break;
			}
			case ADD_ITEM:
			case B_COPY_TARGET:
				mainWindow->PostMessage(message);
				break;
			case DOWN_ITEM:
			case REM_ITEM:
					camera->PostMessage(message);
				break;
			case RELOAD_CONFIGURATION:
            {
                #ifdef DEBUG
					lfcore = fopen(LOGFILE,"a");
					fprintf(lfcore,"CORE - Reloading the new plugin.\n");
					fclose(lfcore);
				#endif
                // Close the old plugin
                camera->PostMessage(new BMessage(CLOSE_DEVICE));
                // Get new Camera Interface
                camera->PostMessage(message);
                // Open the device
				if(GetDeviceType() == TYPE_USB)
				{
					camera->PostMessage(new BMessage(OPEN_DEVICE));
				}	
                // Change the action menu items in the interface
                mainWindow->PostMessage(message);
                break;
            }
            case GET_CONFIGURATION:
            {	
            	BMessage reply;
            	reply.AddPointer("settings",&pgsettings);
            	message->SendReply(&reply);
            	break;
            }
            case SAVE_CONFIGURATION:
            {
            	#ifdef DEBUG
					lfcore = fopen(LOGFILE,"a");
					fprintf(lfcore,"CORE - Saving the new settings.\n");
					fclose(lfcore);
				#endif
            	SaveSettingsToFile(pgsettings);
            	break;
            }
            case GET_CAMSTRING:
            {	
            	BMessage reply;
            	if(GetCameraStrings())
            	{
            		reply.AddPointer("camstrings",&supportedCams);
            		reply.AddPointer("pluginstrings",&pluginSupportedCams);
            		message->SendReply(&reply);
            	}
            	break;
            }
            case GET_PLUG_DETAIL:
            {
            	const char *camerastring;
            	BMessage reply;
            	camerastring = message->FindString("camerastring");
            	if(GetPluginDetails((char *)camerastring))
            	{
            		reply.AddPointer("details",&plugininfo);
            		message->SendReply(&reply);
            	}
            	break;
            }
            case PLUG_CONFIG_PRESENT:
            {	
            	const char *camerastring;
            	BMessage reply;
            	camerastring = message->FindString("camerastring");
    			bool present = IsPluginConfigPresent((char *)camerastring);
            	reply.AddBool("present",present);
            	message->SendReply(&reply);
            	break;
            }
            case OPEN_PLUG_WINDOW:
            {
            	const char *camerastring;
            	BPoint interfacepoint;
            	camerastring = message->FindString("camerastring");
            	message->FindPoint("point",&interfacepoint);
            	OpenPluginConfig((char *)camerastring,&interfacepoint);
            	break;
            }
            case GET_DEVTYPE:
            {	
            	BMessage reply;
            	int devtype = GetDeviceType();
            	reply.AddInt32("devtype",devtype);
            	message->SendReply(&reply);
            	break;
            }
			default:
				BApplication::MessageReceived(message);
		}

}
// 
//	BeDiGiCam:: MainWindow of the Application
bool BeDiGiCamApp::CreateGUI()
{
	#ifdef DEBUG
		lfcore = fopen(LOGFILE,"a");
		fprintf(lfcore,"CORE - Create GUI.\n");
		fclose(lfcore);
	#endif
	BEntry appentry; 
	BPath path;
	BWindow* (*instantiate_mainWindow)(BLooper*,int);
	// get the path of the application
	int32 devType = GetDeviceType();
	#ifdef DEBUG
		lfcore = fopen(LOGFILE,"a");
		fprintf(lfcore,"CORE - Device type is: %d.\n",devType);
		fclose(lfcore);
	#endif
	app_info info; 
  	app->GetAppInfo(&info); 
  	appentry.SetTo(&info.ref); 
    appentry.GetPath(&path);
	path.GetParent(&path);
	path.Append("interface/");
	path.Append("libPGInterface");
	addonId = load_add_on(path.Path());
    if (addonId >= 0)
	{
		if (get_image_symbol(addonId, "instantiate_mainWindow", B_SYMBOL_TYPE_TEXT, (void **)&instantiate_mainWindow) == B_OK)
	    {
	 	   mainWindow = (*instantiate_mainWindow)(this,devType);
	 	   BMessage message(GET_CONFIGURATION);
	 	   mainWindow->PostMessage(&message);     
        }
	}
	else
	{
		#ifdef DEBUG
			lfcore = fopen(LOGFILE,"a");
			fprintf(lfcore,"CORE - Couldn't load the plugin '%s'.\n",path.Path());
			fclose(lfcore);
		#endif
	}
	return B_OK;
}
//
//	BeDiGiCam:: Check the device type
int BeDiGiCamApp::GetDeviceType()
{
	#ifdef DEBUG
		lfcore = fopen(LOGFILE,"a");
		fprintf(lfcore,"CORE - Get device type.\n");
		fclose(lfcore);
	#endif
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
bool BeDiGiCamApp::GetCameraStrings()
{
	char *addonName;
	BEntry appentry;
	BPath path;
	image_id lAddonId;
	// get the path of the application
	app_info info; 
  	app->GetAppInfo(&info); 
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
	while((err = searchDir.GetNextEntry(&entry,true)) == B_OK)
	{
		if( entry.InitCheck() != B_NO_ERROR )
      		break;
    	if( entry.GetPath(&path) != B_NO_ERROR )
    	{
    		printf( "entry.GetPath failed\n" );
    	}
    	else
    	{
      		addonName = new char[B_FILE_NAME_LENGTH];
      		entry.GetName(addonName);
			lAddonId = load_add_on(path.Path());
			if(addonId > 0)
			{
				unload_add_on(lAddonId);
				interface = new CamInterface(addonName);
				std::vector<std::string> plugincamstrings = interface->getCameraStrings();
				std::vector<std::string>::iterator i = plugincamstrings.begin();
				while(i != plugincamstrings.end())
				{
					char *dummy = new char[B_FILE_NAME_LENGTH];
					strcpy(dummy,(*i).c_str());
					supportedCams.push_back(dummy);
					pluginSupportedCams.insert(std::pair<const char* const,std::string>(addonName,(*i)));
					i++;
				}
				delete(interface);
			}
			else
				printf("loading %s failed!",addonName);
		}
	}
	return true;
}
//
//	BeDiGiCam:: Check the device type
bool BeDiGiCamApp::GetPluginDetails(char *camerastring)
{
	// save the plugin name
	std::multimap<const char*,std::string>::iterator i = pluginSupportedCams.begin();
	while(i != pluginSupportedCams.end())
	{
		//	TODO should be revised :: FindMarked->label must be put in a variable first
		if(!strcmp((*i).second.c_str(),camerastring))
		{
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
bool BeDiGiCamApp::OpenPluginConfig(char *camerastring,BPoint *interfacePoint)
{
	BPoint lPoint = *interfacePoint;
	std::multimap<const char*,std::string>::iterator i = pluginSupportedCams.begin();
	while(i != pluginSupportedCams.end())
	{
		if(!strcmp((*i).second.c_str(),camerastring))
		{
			interface = new CamInterface((char*)(*i).first);
			if(interface->check_configurePlugin)
			{
				pluginconfwindow = interface->pluginConfiguration(lPoint);
				pluginconfwindow->Show();
			}
			return true;
		}
		i++;
	}
	return false;
}
//
//	BeDiGiCam:: Check if there is a plugin configuration screen
bool BeDiGiCamApp::IsPluginConfigPresent(char *camerastring)
{
	std::multimap<const char*,std::string>::iterator i = pluginSupportedCams.begin();
	while(i != pluginSupportedCams.end())
	{
		if(!strcmp((*i).second.c_str(),camerastring))
			return interface->check_configurePlugin;
		i++;
	}
	return false;
}
//
//	BeDiGiCam::main
int main()
{
	app = new BeDiGiCamApp();
    app->Run();
    delete app;
	return 0;
}
