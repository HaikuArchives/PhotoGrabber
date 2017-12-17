/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
// 
//  Includes
#include <FindDirectory.h>
#include <vector>
#include <string>
#include <Layout.h>
#include <LayoutBuilder.h>
#include <Alignment.h>
#include <PopUpMenu.h>
#include <app/Messenger.h>

//  Local Includes
#include "ConfigWindow.h"
#include "logger.h"
//
FILE *lfconfigw;

//
//  ConfigWindow :: Constructor
BeCam_ConfigWindow::BeCam_ConfigWindow(float xPos,float yPos,BeCam_MainWindow *mainWindow)
  : BWindow(BRect(xPos, yPos, xPos + 50, yPos + 50), _T("PhotoGrabber Configuration"), B_TITLED_WINDOW, B_WILL_DRAW | B_NOT_MINIMIZABLE | B_NOT_ZOOMABLE | B_NOT_RESIZABLE)
{
    parent = mainWindow;
    // Get the settings from the system core
    BMessage reply;
    BMessage message(GET_CONFIGURATION);
    BMessenger messenger(NULL, parent->systemcore);
    messenger.SendMessage(&message, &reply);
    reply.FindPointer("settings",(void **)&pgsettings);

    //	Create the config groupbox
    BBox* configBox = new BBox("configbox");
    configBox->SetLabel(_T("Camera Configuration"));
    configBox->SetExplicitAlignment(BAlignment(B_ALIGN_USE_FULL_WIDTH, B_ALIGN_VERTICAL_UNSET));

    cameraMenu  = new BPopUpMenu("cameraMenu");
    BMenuField* cameraMenuField = new BMenuField(_T("Plugin:"), cameraMenu);
    
    BLayoutBuilder::Grid<>(configBox, B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING)
    	.SetExplicitAlignment(BAlignment(B_ALIGN_USE_FULL_WIDTH, B_ALIGN_VERTICAL_UNSET))
        .SetInsets(B_USE_WINDOW_INSETS)

        .AddGroup(B_HORIZONTAL, 0, 1, 0)
            .SetInsets(0)
            .SetExplicitMinSize(BSize(350, 3))
            .SetExplicitMaxSize(BSize(B_SIZE_UNSET, 3))
        .End()

        .Add(new BStringView(NULL, _T("Please select your type of camera.")), 0, 1, 2)

		.AddMenuField(cameraMenuField, 0, 2, B_ALIGN_LEFT, 1, 1)


		.Add(new BStringView(NULL, _T("Plugin Name:")), 0, 3)
		.Add(pluginName = new BStringView(NULL, ""), 1, 3)

		.Add(new BStringView(NULL, _T("Plugin Source:")), 0, 4)
		.Add(pluginSource = new BStringView(NULL, ""), 1, 4)

		.Add(new BStringView(NULL, _T("Plugin Version:")), 0, 5)
		.Add(pluginVersion = new BStringView(NULL, ""), 1, 5)

		.Add(pluginConfig = new BButton(_T("Plugin Configuration"), new BMessage(CONF_BUT)), 1, 6)

        .SetColumnWeight(0, 0.3)
        .SetColumnWeight(1, 0.7)
    .End();
    pluginConfig->SetExplicitAlignment(BAlignment(B_ALIGN_USE_FULL_WIDTH, B_ALIGN_VERTICAL_UNSET));
    cameraMenuField->Layout(true);
    
    //	Create the debug groupbox
    BBox* debugBox = new BBox("debugbox");
    debugBox->SetLabel(_T("Debug configuration"));
    debugBox->SetExplicitAlignment(BAlignment(B_ALIGN_USE_FULL_WIDTH, B_ALIGN_VERTICAL_UNSET));

    checkFile = new BCheckBox("file check", _T("Debug to file"), new BMessage(CHECK_FILE));
    checkFile->SetValue(pgsettings->debugFile);

    BLayoutBuilder::Group<>(debugBox, B_VERTICAL, 0)
        .SetInsets(B_USE_WINDOW_INSETS, 20, B_USE_WINDOW_INSETS, B_USE_WINDOW_INSETS)
        .Add(checkFile)
    .End();
    
    BLayoutBuilder::Group<>(this, B_VERTICAL, B_USE_ITEM_SPACING)
        .SetExplicitAlignment(BAlignment(B_ALIGN_USE_FULL_WIDTH, B_ALIGN_VERTICAL_UNSET))
        .SetInsets(B_USE_WINDOW_INSETS)
        .Add(configBox)
        .Add(debugBox)
        .AddGroup(B_HORIZONTAL, B_USE_ITEM_SPACING)
            .SetInsets(0)
            .SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_BOTTOM))
            .Add(new BButton(_T("Cancel"), new BMessage(CANCEL)))
            .Add(new BButton(_T("Save"), new BMessage(SAVE_CONFIGURATION)))
        .End()
    .End();

    FillPluginMenu(cameraMenu);

    Layout(true);
    BSize minSize = GetLayout()->MinSize();
    ResizeTo(minSize.width, minSize.height);
    LogDebug("CONFIGWINDOW - Window created.");
}

//
//		ConfigWin :: QuitRequested
bool 
BeCam_ConfigWindow::QuitRequested() {
    parent->configWindow=NULL;
    LogDebug("CONFIGWINDOW - Window destroyed.");
    return true;
}

//
//		ConfigWindow :: MessageRecieved
void 
BeCam_ConfigWindow::MessageReceived(BMessage* message) {
    switch(message->what) {
		case SAVE_CONFIGURATION: {
			if (strcmp(cameraMenu->FindMarked()->Label(), pgsettings->deviceName)) {
				// save the camera name
				if (cameraMenu->FindMarked() != NULL)
					strncpy(pgsettings->deviceName, cameraMenu->FindMarked()->Label(), B_FILE_NAME_LENGTH);
				// save the plugin name
				for (multimap<const char*, std::string>::iterator i = pluginSupportedCams->begin();
						i != pluginSupportedCams->end(); i++) {
					if (!strcmp((*i).second.c_str(),pgsettings->deviceName)) {
						strncpy(pgsettings->pluginName, (*i).first, B_FILE_NAME_LENGTH);
						break;
					}
				}
			}
			BMessage *appmessage = new BMessage(SAVE_CONFIGURATION);
			parent->systemcore->PostMessage(appmessage);
			appmessage = new BMessage(RELOAD_CONFIGURATION);
			appmessage->AddString("libname", pgsettings->pluginName);
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
		    GetPluginDetails(cameraMenu->FindMarked()->Label());
			break;
		case CONF_BUT:
		    OpenPluginConfig();
			break;
		case CHECK_FILE:
			pgsettings->debugFile = !pgsettings->debugFile;
			break;
		default: 
			BWindow::MessageReceived(message);
			break;
    }
} 
//
//  ConfigWindow :: CreateCameraTypeMenu
void 
BeCam_ConfigWindow::FillPluginMenu(BMenu* menu) {
    //	Create the menu
    BMessenger messenger(NULL, parent->systemcore);
    std::vector<std::string> *supportedCams;
    BMessage appmessage(GET_CAMSTRING);
    BMessage reply;
    messenger.SendMessage(&appmessage, &reply);
    if (reply.FindPointer("camstrings",(void **)&supportedCams) == B_OK && 
			reply.FindPointer("pluginstrings",(void **)&pluginSupportedCams) == B_OK) {

		BMenuItem *cameraName;
		for (vector<string>::iterator i = supportedCams->begin(); i != supportedCams->end(); i++) {
			cameraName = new BMenuItem((*i).c_str(), new BMessage(CONF_MITM));
			if (!strcmp(pgsettings->deviceName,(*i).c_str())) {
				cameraName->SetMarked(true);	
				GetPluginDetails();
			}
			menu->AddItem(cameraName);
		}
    }
}

//	configWindow::GetPluginName
void 
BeCam_ConfigWindow::GetPluginDetails(const char* cameraname) {
    version_info *versioninfo;
    BMessenger messenger(NULL,parent->systemcore);
    BMessage appmessage(GET_PLUG_DETAIL);
    BMessage reply;
    if (cameraname == NULL)
		cameraname = pgsettings->deviceName;
    
    appmessage.AddString("camerastring",(const char*)cameraname);
    messenger.SendMessage(&appmessage, &reply);
    reply.FindPointer("details",(void **)&versioninfo);

    pluginName->SetText(versioninfo->long_info ? versioninfo->long_info : "N/A");
    pluginSource->SetText(versioninfo->short_info ? versioninfo->short_info : "N/A");
    
    char lVersion[10];

    sprintf(lVersion,"%ld.%ld.%ld",versioninfo->major,versioninfo->middle,versioninfo->minor);
    pluginVersion->SetText(lVersion);
    pluginConfig->SetEnabled(IsPluginConfigPresent());
}
//
//	configWindow::OpenPluginConfiguration
void 
BeCam_ConfigWindow::OpenPluginConfig() {
    LogDebug("CONFIGWINDOW - Begin open plugin.");
    BRect rect = Frame();
    float Xpos,Ypos;
    Xpos = rect.left + ((rect.right - rect.left)/2);
    Ypos = rect.top + ((rect.bottom - rect.top)/2);
    BPoint lPoint(Xpos,Ypos);
    // get the camera name
    if (cameraMenu->FindMarked() != NULL) {
		BMessage appmessage(OPEN_PLUG_WINDOW);
		appmessage.AddPoint("point",lPoint);
		appmessage.AddString("camerastring", cameraMenu->FindMarked()->Label());
		parent->systemcore->PostMessage(&appmessage);
    }
    LogDebug("CONFIGWINDOW - End open plugin.");		
}
//
//	configWindow::OpenPluginConfiguration
bool 
BeCam_ConfigWindow::IsPluginConfigPresent() {
    bool present = false;
    BMessenger messenger(NULL, parent->systemcore);
    BMessage appmessage(PLUG_CONFIG_PRESENT);
    BMessage reply;
    appmessage.AddString("camerastring", pgsettings->deviceName);
    messenger.SendMessage(&appmessage, &reply);
    reply.FindBool("present", &present);
    return present;	
}

