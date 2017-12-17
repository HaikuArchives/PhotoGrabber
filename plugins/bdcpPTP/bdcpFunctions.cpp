/* CanonCamera.cpp */

#include <String.h>
#include <Locker.h>
#include "bdcpPTP.h"
#include "bdcpFunctions.h"
#include "bdcpCapture.h"
#include "PTPFunctions.h"
#include "PictureList.h"
#include "bdcpConfig.h"

static BLooper* msgtarget = NULL;
static PTPCamera* Camera  = NULL;

int get_BDCP_API_Revision(void) {
    return 2;
}

void getPluginVersion(version_info &ver) {
    DEBUG("getPluginVersion\n");
    ver.major = 2;
    ver.middle = 0;
    ver.minor = 0;
    ver.variety = 0;
    ver.internal = 0;
    sprintf(ver.short_info, "Fishpond");
    sprintf(ver.long_info, "BDCP Plugin for PTP Cameras");
}

void getSupportedCameras(vector<string> & listofcams) {
    DEBUG("getSupportedCameras\n");
    listofcams.push_back("PTP Cameras (enhanced)");
}

status_t openCamera(void) {
    int i = 20;
    StartPTP(&connectStateChanged);
    while ((i > 0) && (!Camera || Camera->Status() != Ready)) {
        DEBUG("openCamera: waiting Camera to register - %i\n", i);
        snooze(100000);
        i--;
    }
    return B_OK; // Return ok even if nothing connected. Use messaging for later connect.
}

status_t closeCamera(void) {
    DEBUG("closeCamera\n");
    if ((Camera) && (Camera->Status() == Ready)) Camera->Disconnect();
    return B_OK;
    StopPTP();
}

status_t getNumberofPics(int &number) {

    status_t status = B_ERROR;
    DEBUG("getNumerofPics\n");
    number = 0;
    if ((Camera) && Camera->Status() == Identified) 
        Camera->Connect();
    
    if ((Camera) && Camera->Status() == Ready) {
        number = Camera->Pictures()->Count();
        status = B_OK;
    }
    DEBUG("getNumberofPics: %i Pictures on Camera\n", number);
    return status;
}

status_t setCurrentPicture(int picturenum) {
    if ((Camera) && Camera->Status() == Ready) {
        DEBUG("Set current picture to %i of %i\n", picturenum, Camera->Pictures()->Count());
        return Camera->Pictures()->SetCurrent(picturenum);
    } else return B_INTERRUPTED;
}

void updateStatus(float bytesread) {
    if (msgtarget) {
        BMessage *msg = new BMessage(MSG_UPDATEBAR);
        msg->AddFloat("updateby", bytesread);
        msgtarget->PostMessage(msg);
        delete msg;
    }
}

status_t downloadPicture(BPath savedir) {
    if (!Camera) return B_DEV_NOT_READY;
	
	PictureDesc* picture = Camera->Pictures()->CurrentItem();
	
	if (!picture) return B_BAD_INDEX;

    DEBUG("downloadPicture %s to %s\n", picture->Name(), savedir.Path());

    return Camera->GetPicture(Camera->Pictures()->Current(), &savedir, (msgtarget) ? &updateStatus : NULL);
}

status_t deletePicture() {
    status_t status;

	int picture = Camera->Pictures()->Current();
    DEBUG("Delete picture %i\n", picture);

    status = Camera->DeletePicture(picture);
    return status;
}

status_t takePicture() {
    DEBUG("takePicture: Starting\n");
    PTPCaptWindow* captWindow = new PTPCaptWindow(Camera);
	if (captWindow->InitCheck() < B_OK) {
		delete captWindow;
		return B_NOT_ALLOWED;
	}
		
	captWindow->Go();
    return B_OK;
}

status_t getImageName(char* &name) {
    if (!Camera) return B_DEV_NOT_READY;
	PictureDesc* desc = Camera->Pictures()->CurrentItem();
	if (desc) {
		name = strdup(desc->Name());
		DEBUG("getImageName: for picture %i yields %s\n", Camera->Pictures()->Current(), name);
		return B_OK;
    }
    return B_BAD_INDEX;
}

status_t getImageSize(int &size) {
    PictureDesc* desc;
    if ((Camera) && (desc = Camera->Pictures()->CurrentItem())) {
        size = desc->Size;
        DEBUG("getImageSize: for picture %d yields %" B_PRId32 "\n", Camera->Pictures()->Current(), desc->Size);
        return B_OK;
    }
    return B_ERROR;
}

status_t getImageDate(char* &date) {
    PictureDesc* desc;
    if ((Camera) && (desc = Camera->Pictures()->CurrentItem())) {
        tm t = desc->Date;
        BString dateString;
        dateString.SetToFormat("%04i-%02i-%02i %02i:%02i:%02i", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday + 1, t.tm_hour, t.tm_min, t.tm_sec);
        date = strdup(dateString);
        DEBUG("Get date of picture %i (%s)\n", Camera->Pictures()->Current(), date);
        return B_OK;
    } else return B_ERROR;
}

BWindow * configurePlugin(BPoint middle) {
    return (new PTPCnfWindow(middle));
}

status_t getThumbnail(BBitmap* &picture) {
    PictureDesc* item;
    if ((Camera) && (item = Camera->Pictures()->CurrentItem())) {

        DEBUG("getThumbnail for picture %i\n", Camera->Pictures()->Current());

        if (!item->thumb || !item->thumb->IsValid()) {
            Camera->GetThumb(Camera->Pictures()->Current(), &(item->thumb));            
        }
        picture = item->thumb;
        return B_OK;
    } else return B_ERROR;
}

status_t getLevel3FunctionNames(map<char*, char*> &menunames) {
    DEBUG("Get Level 3 Function names\n");
    menunames.clear();
    return B_NO_ERROR;
}

status_t setMessageTarget(BLooper* mtarget) {
    DEBUG("Set message target\n");
    msgtarget = mtarget;
    return B_NO_ERROR;
}

status_t getfiledata() {
    DEBUG("Get file data\n");
    return B_NO_ERROR;
}

status_t getDeviceType(int* deviceType) {
    *deviceType = TYPE_USB;
    return B_OK;
}

status_t connectStateChanged(PTPCamera* camera) {
    Camera = camera;
    if (msgtarget == NULL) return B_OK;

    if (camera == NULL || camera->Status() == None) {
        BMessage *notification = new BMessage(CAM_DISCONNECTED);
        msgtarget->PostMessage(notification);
    } else {
        switch(camera->Status()) {
            case Identified : 
                return B_OK;
                break;
            case Ready : {
                BMessage *notification = new BMessage(CAM_CONNECTED);
                notification->AddString("product", camera->GetProduct());
                msgtarget->PostMessage(notification);
                return B_OK;
            }
            default:
                return B_ERROR;
        }
    }
    return B_ERROR;
}

status_t getImageHeight(int* height) {
    *height = Camera->Pictures()->CurrentItem()->YRes;
    return B_OK;
}

status_t getImageWidth(int* width) {
    *width = Camera->Pictures()->CurrentItem()->XRes;
    return B_OK;
}






