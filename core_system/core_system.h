/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// File definition
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
                                    BeDiGiCamApp();
                                    ~BeDiGiCamApp();
    virtual void                    MessageReceived(BMessage* message);
    virtual void                    ReadyToRun();
    bool                            CreateGUI();
    int                             GetDeviceType();
    // Configuration part
    bool                            GetCameraStrings();
    bool                            GetPluginDetails(char *camerastring);
    bool                            OpenPluginConfig(char *camerastring, BPoint *interfacePoint);
    bool                            IsPluginConfigPresent(char *camerastring);
    CamInterface*                   interface;
    vector<string>                  supportedCams;
    multimap<const char*, string>   pluginSupportedCams;
    version_info                    plugininfo;
    BWindow*                        pluginconfwindow;
    //
    class Camera*                   camera;
    BWindow*                        mainWindow;

private:
    PG_Settings                     pgsettings;
};

// definitions

#define CLI_OFFSET                  55

#endif /* __CORESYS_H__ */
