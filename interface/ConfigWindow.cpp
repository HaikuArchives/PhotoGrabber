/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
// 
//		Includes
#include <FindDirectory.h>
#include <vector>
#include <string>
#include <app/Messenger.h>

//		Local Includes
#include "ConfigWindow.h"
#include "logger.h"
//
FILE *lfconfigw;
//
//		Configview :: Constructor
BeCam_ConfigView::BeCam_ConfigView(BRect r,float xPos, float yPos):BView(r, "configview", B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{
	
	rgb_color bg_color=ui_color(B_PANEL_BACKGROUND_COLOR);
	SetViewColor(bg_color);
}

//
//		ConfigView::~BeCam_ConfigView()
BeCam_ConfigView::~BeCam_ConfigView(void)
{
}

//
//		ConfigWindow :: Constructor
BeCam_ConfigWindow::BeCam_ConfigWindow(float xPos,float yPos,BeCam_MainWindow *mainWindow) : BWindow(BRect(xPos,yPos,xPos + WINDOW_WIDTH_CONFIG,yPos + WINDOW_HEIGHT_CONFIG), _T("PhotoGrabber Configuration"), B_TITLED_WINDOW, B_WILL_DRAW | B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE | B_NOT_RESIZABLE)
{
	parent = mainWindow;
	// Get the settings from the system core
	BMessage reply;
	BMessage message(GET_CONFIGURATION);
	BMessenger messenger(NULL,parent->systemcore);
	messenger.SendMessage(&message,&reply);
	reply.FindPointer("settings",(void **)&pgsettings);
	//
	becam_configView = new BeCam_ConfigView(Bounds(),xPos,yPos);
	//		Create the config groupbox
	BRect r = Bounds();
	r.top= 10;
	r.left= 5;
	r.right= r.right - 5;
	r.bottom= r.top + 168;
	becam_configbox = new BBox(r, 
								"configbox",
								B_FOLLOW_ALL_SIDES,
								B_WILL_DRAW | B_NAVIGABLE,
								B_FANCY_BORDER
								);
	becam_configbox->SetLabel(_T("Camera Configuration"));
	// 		Create the popup menu
	// 		Add the camera type label to the config groupbox
	BTextView	*becam_typelabel = new BTextView(BRect(15, r.top + 10, r.right - 20, r.top + 40),"typelabel",BRect(0,0,r.right - 30,r.top + 60),B_FOLLOW_LEFT | B_FOLLOW_BOTTOM,B_WILL_DRAW);
	becam_typelabel->SetWordWrap(true);
	becam_typelabel->MakeSelectable(false);
	becam_typelabel->MakeEditable(false);
	becam_typelabel->SetText(_T("Please select your type of camera."));
	rgb_color bg_color=ui_color(B_PANEL_BACKGROUND_COLOR);
	becam_typelabel->SetViewColor(bg_color);
	becam_configbox->AddChild(becam_typelabel);
	//	Create the plugin details textboxes
	BStringView *becam_pluginNameLabel = new BStringView(BRect(r.left + 10, r.top + 50 , r.left + 100, r.top + 85),"pnamelabel",_T("Plugin Name:"),B_FOLLOW_LEFT|B_FOLLOW_BOTTOM,B_WILL_DRAW);
	becam_configbox->AddChild(becam_pluginNameLabel);
	becam_pluginName = new BStringView(BRect(r.left + 110, r.top + 50 , r.right - 20 , r.top + 85),"pluginnamefield","n/a",B_FOLLOW_LEFT|B_FOLLOW_BOTTOM,B_WILL_DRAW);
	becam_configbox->AddChild(becam_pluginName);
	//
	BStringView *becam_pluginSourceLabel = new BStringView(BRect(r.left + 10, r.top + 70 , r.left + 100, r.top + 105),"psourcelabel",_T("Plugin Source:"),B_FOLLOW_LEFT|B_FOLLOW_BOTTOM,B_WILL_DRAW);
	becam_configbox->AddChild(becam_pluginSourceLabel);
	becam_pluginSource = new BStringView(BRect(r.left + 110, r.top + 70, r.right - 20, r.top + 105),"pluginsourcefield","n/a",B_FOLLOW_LEFT|B_FOLLOW_BOTTOM,B_WILL_DRAW);
	becam_configbox->AddChild(becam_pluginSource);
	//
	BStringView *becam_pluginVersionLabel = new BStringView(BRect(r.left + 10, r.top + 90 , r.left + 100, r.top + 125),"pverslabel",_T("Plugin Version:"),B_FOLLOW_LEFT|B_FOLLOW_BOTTOM,B_WILL_DRAW);
	becam_configbox->AddChild(becam_pluginVersionLabel);
	becam_pluginVersion = new BStringView(BRect(r.left + 110, r.top + 90 , r.right - 20, r.top + 125),"pluginversionfield","n/a",B_FOLLOW_LEFT|B_FOLLOW_BOTTOM,B_WILL_DRAW);
	becam_configbox->AddChild(becam_pluginVersion);
	//	Add the config button the the config box
	becam_pluginConfig = new BButton(BRect(r.left + 10, r.top + 118, r.left + 155, r.top + 135), "pluginconfbutton", _T("Plugin Configuration"),new BMessage(CONF_BUT), B_FOLLOW_LEFT|B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW);
	becam_configbox->AddChild(becam_pluginConfig);
	//
	//	Create and add the camera type popupmenu to the BDCP box
	Lock();
	CreateCameraTypeMenu(r);
	Unlock();
	// Create the debugging group box
	r.top = r.bottom + 10;
	r.bottom = r.top + 80;
	becam_debugbox = new BBox(r,"debugbox", B_FOLLOW_ALL_SIDES |
											B_WILL_DRAW | B_NAVIGABLE,
											B_FANCY_BORDER);
	becam_debugbox->SetLabel(_T("Debug configuration"));
	becam_checkFile = new BCheckBox(BRect(15,30,200,50), "file check", _T("Debug to file"),new BMessage(CHECK_FILE), B_FOLLOW_LEFT|B_FOLLOW_BOTTOM,B_WILL_DRAW|B_NAVIGABLE);
	if(pgsettings->debugFile)
		becam_checkFile->SetValue(true);
	else
		becam_checkFile->SetValue(false);
	becam_debugbox->AddChild(becam_checkFile);
	// 		Add the save button the the view
	becam_savebutton = new BButton(BRect(5, r.bottom + 5 , 105, r.bottom + 30), "name", _T("Save"),new BMessage(SAVE_CONFIGURATION), B_FOLLOW_LEFT|B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW);
	becam_configView->AddChild(becam_savebutton);
	// 		Add the cancel button the the view
	becam_cancelbutton = new BButton(BRect(115, r.bottom + 5, 215, r.bottom + 30), "name", _T("Cancel"),new BMessage(CANCEL), B_FOLLOW_LEFT|B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW);
	becam_configView->AddChild(becam_cancelbutton);
	// 		Add the config groupbox to the view
	becam_configView->AddChild(becam_configbox);
	// 		Add the debug groupbox to the view
	becam_configView->AddChild(becam_debugbox);
	AddChild(becam_configView);
	LogDebug("CONFIGWINDOW - Window created.");
}

//
//		ConfigWin :: QuitRequested
bool BeCam_ConfigWindow::QuitRequested()
{
	parent->configWindow=NULL;
	LogDebug("CONFIGWINDOW - Window destroyed.");
	return BWindow::QuitRequested();
}

//
//		ConfigWindow :: MessageRecieved
void BeCam_ConfigWindow::MessageReceived(BMessage* message)
{
	switch(message->what)
	{
		case SAVE_CONFIGURATION:
		{
			if(strcmp(pgsettings->deviceName,becam_cameraMenu->FindMarked()->Label()))
			{
				// save the camera name
				if(becam_cameraMenu->FindMarked() != NULL)
				strncpy(pgsettings->deviceName,becam_cameraMenu->FindMarked()->Label(),B_FILE_NAME_LENGTH);
				// save the plugin name
				std::multimap<const char*,std::string>::iterator i = pluginSupportedCams->begin();
				while(i != pluginSupportedCams->end())
				{
					if(!strcmp((*i).second.c_str(),pgsettings->deviceName))
                	{
						strncpy(pgsettings->pluginName,(*i).first,B_FILE_NAME_LENGTH);
						break;
					}
					i++;
				}
			}
			BMessage *appmessage = new BMessage(SAVE_CONFIGURATION);
			parent->systemcore->PostMessage(appmessage);
			appmessage = new BMessage(RELOAD_CONFIGURATION);
			appmessage->AddString("libname",pgsettings->pluginName);
			parent->systemcore->PostMessage(appmessage);
			delete(appmessage);
			
			parent->configWindow=NULL;
			LogDebug("CONFIGWINDOW - Close Window.");
 			Quit();
			break;
		}
		case CANCEL:
			parent->configWindow=NULL;
			LogDebug("CONFIGWINDOW - Close Window.");
			Quit();
			break;
		case CONF_MITM:
			GetPluginDetails((char *)becam_cameraMenu->FindMarked()->Label());
			break;
		case CONF_BUT:
			OpenPluginConfig();
			break;
		case CHECK_FILE:
			pgsettings->debugFile = !pgsettings->debugFile;
			break;
		default:
		{
			BWindow::MessageReceived(message);
			break;
		}
	}
} 
//
//		ConfigWindow :: CreateCameraTypeMenu
void BeCam_ConfigWindow::CreateCameraTypeMenu(BRect r)
{
	BMenuItem *cameraName;
	std::vector<std::string> *supportedCams;
 	//		Create the menu
 	becam_cameraMenu = new BPopUpMenu("");
	BMessenger messenger(NULL,parent->systemcore);
	BMessage appmessage(GET_CAMSTRING);
	BMessage reply;
	messenger.SendMessage(&appmessage,&reply);
	if(reply.FindPointer("camstrings",(void **)&supportedCams) == B_OK && reply.FindPointer("pluginstrings",(void **)&pluginSupportedCams) == B_OK)
	{
		
		std::vector<std::string>::iterator i = supportedCams->begin();
		while(i != supportedCams->end())
		{
			char *dummy = new char[B_FILE_NAME_LENGTH];
			strcpy(dummy,(*i).c_str());
			cameraName = new BMenuItem((*i).c_str(),new BMessage(CONF_MITM));
			if(!strcmp(pgsettings->deviceName,(*i).c_str()))
			{
				cameraName->SetMarked(true);	
				GetPluginDetails();
			}
			becam_cameraMenu->AddItem(cameraName);
			i++;
		}
	}
	
	// 	Add the popup menu to the config groupbox
	becam_cameraPopup = new BMenuField(BRect(15, r.top + 35, r.right - 20, r.top + 10), "plugin", _T("Plugin:"), becam_cameraMenu);
	becam_cameraPopup->SetDivider(be_plain_font->StringWidth(_T("Plugin:")) + 5);
	becam_configbox->AddChild(becam_cameraPopup);
}

//	configWindow::GetPluginName
void BeCam_ConfigWindow::GetPluginDetails(char * cameraname)
{
	version_info *versioninfo;
	BMessenger messenger(NULL,parent->systemcore);
	BMessage appmessage(GET_PLUG_DETAIL);
	BMessage reply;
	if(cameraname == NULL)
		appmessage.AddString("camerastring",pgsettings->deviceName);
	else
		appmessage.AddString("camerastring",(const char*)cameraname);
	messenger.SendMessage(&appmessage,&reply);
	reply.FindPointer("details",(void **)&versioninfo);
	if(versioninfo->long_info != NULL)
		becam_pluginName->SetText(versioninfo->long_info);
	else
		becam_pluginName->SetText("n/a");
	if(versioninfo->short_info != NULL)
		becam_pluginSource->SetText(versioninfo->short_info);
	else
		becam_pluginSource->SetText("n/a");
	char *lVersion = new char[10];
	sprintf(lVersion,"%ld.%ld.%ld",versioninfo->major,versioninfo->middle,versioninfo->minor);
	becam_pluginVersion->SetText(lVersion);
	if(!IsPluginConfigPresent())
		becam_pluginConfig->SetEnabled(false);
	delete(lVersion);	
}
//
//	configWindow::OpenPluginConfiguration
void BeCam_ConfigWindow::OpenPluginConfig()
{
	LogDebug("CONFIGWINDOW - Begin open plugin.");
	BRect rect = Frame();
	float Xpos,Ypos;
	Xpos = rect.left + ((rect.right - rect.left)/2);
	Ypos = rect.top + ((rect.bottom - rect.top)/2);
	BPoint lPoint(Xpos,Ypos);
	// get the camera name
	if(becam_cameraMenu->FindMarked() != NULL)
	{
			BMessage appmessage(OPEN_PLUG_WINDOW);
			appmessage.AddPoint("point",lPoint);
			appmessage.AddString("camerastring",(const char*)becam_cameraMenu->FindMarked()->Label());
			parent->systemcore->PostMessage(&appmessage);
	
	}
	LogDebug("CONFIGWINDOW - End open plugin.");		
}
//
//	configWindow::OpenPluginConfiguration
bool BeCam_ConfigWindow::IsPluginConfigPresent()
{
	bool present = false;
	BMessenger messenger(NULL,parent->systemcore);
	BMessage appmessage(PLUG_CONFIG_PRESENT);
	BMessage reply;
	appmessage.AddString("camerastring",pgsettings->deviceName);
	messenger.SendMessage(&appmessage,&reply);
	reply.FindBool("present",present);
	return present;	
}

