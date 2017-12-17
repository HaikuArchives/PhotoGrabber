#include <DataIO.h>
#include <NodeInfo.h>
#include <string.h>
#include <stdlib.h>
#include <FindDirectory.h>
#include <View.h>
#include "bdcpPTP.h"
#include "PTPFunctions.h"
#include "PTPDefinitions.h"
#include "PictureList.h"


PTPDeviceInfo::PTPDeviceInfo() 
  : StandardVersion(0),
  	VendorExtensionId(0),
  	VendorExtensionVersion(0),
  	FunctionalMode(0)
{
};

PTPDeviceInfo::PTPDeviceInfo(char* raw) 
  : StandardVersion(0),
  	VendorExtensionId(0),
  	VendorExtensionVersion(0),
  	FunctionalMode(0)
{
    SetTo(raw);
};

PTPDeviceInfo::~PTPDeviceInfo() {
    OperationsSupported.clear();
    EventsSupported.clear();
    DevicePropertiesSupported.clear();
    CaptureFormats.clear();
    ImageFormats.clear();
}

void 
PTPDeviceInfo::SetTo(char* deviceInfo) {
    char* raw                   = deviceInfo;
    StandardVersion             = PTPUInt16(raw);
    VendorExtensionId           = PTPUInt32(raw);
    VendorExtensionVersion      = PTPUInt16(raw);
    VendorExtensionDesc         = PTPString(&raw);
    FunctionalMode              = PTPUInt16(raw);
    PTPSetUInt16(&OperationsSupported, &raw);
    PTPSetUInt16(&EventsSupported, &raw);
    PTPSetUInt16(&DevicePropertiesSupported, &raw);
    PTPSetUInt16(&CaptureFormats, &raw);
    PTPSetUInt16(&ImageFormats, &raw);
    Manufacturer                = PTPString(&raw);
    Model                       = PTPString(&raw);
    DeviceVersion               = PTPString(&raw);
    SerialNumber                = PTPString(&raw);
	Dump();
};


	
PTPRoster::PTPRoster(ConnectStateChangedCallBackFunc connectStateChanged) 
 :  BUSBRoster() 
{
    connectStateChangedCallback = connectStateChanged;
    connectStateChangedHandler  = NULL;
    camera                      = new PTPCamera(NULL);
}
PTPRoster::PTPRoster(BHandler* connectStateChanged) : BUSBRoster() {
    connectStateChangedHandler  = connectStateChanged;
    connectStateChangedCallback = NULL;
    camera                      = new PTPCamera(NULL);
}
    
status_t PTPRoster::DeviceAdded(BUSBDevice *dev) {
    status_t status = B_ERROR;

    if (dev->IsHub()) return status;

    if ((dev->Class()       == USB_DEVICE_CLASS_STILL_IMAGE && 
         dev->Subclass()    == USB_DEVICE_SUBCLASS_IMAGE) || 
        (dev->Class()       == 0 && 
         dev->Subclass()    == 0 && 
         dev->ConfigurationAt(0)->InterfaceAt(0)->Class()   
                            == USB_DEVICE_CLASS_STILL_IMAGE && 
         dev->ConfigurationAt(0)->InterfaceAt(0)->Subclass() 
                            == USB_INTF_CLASS_IMAGE_CAPTURE)) {

        status = camera->SetTo(dev);
        if (status != B_OK) return status;

        const char* product = camera->GetProduct();
        MSG("PTP Roster: %s has connected.\n", product);

        if (connectStateChangedCallback != NULL) {
            status = connectStateChangedCallback(camera);
            if (status == B_OK) { // 
                status = camera->Connect();
                connectStateChangedCallback(camera);
            }
        }
        if (connectStateChangedHandler != NULL) {
            status = camera->Connect();
            if (status == B_OK) {
                BMessage notification(MSG_CAMERA_CONNECTED);
                notification.AddPointer("camera", camera);
                notification.AddString("product", camera->GetProduct());
                BMessenger(connectStateChangedHandler).SendMessage(&notification);
            }
        }
    }
    return status;
}

void PTPRoster::DeviceRemoved(BUSBDevice *dev) {
    if (camera) {
        if (camera->dev == dev) {
            MSG("PTP Roster: %s has disconnected.\n", camera->GetProduct());
            camera->SetTo(NULL);
            if (connectStateChangedCallback != NULL)
                connectStateChangedCallback(camera);
            if (connectStateChangedHandler != NULL) 
                BMessenger(connectStateChangedHandler).SendMessage(MSG_CAMERA_DISCONNECTED);
        }
    }
}
      

/* Static Variables */
static PTPRoster* Roster = NULL;
static int CaptureNum = 0;

void StartPTP(ConnectStateChangedCallBackFunc connectStateChanged) {
    if (Roster == NULL) {
        MSG("PTP Roster: Starting with Callback\n");
        Roster = new PTPRoster(connectStateChanged);
        Roster->Start();
    }
}

void StartPTP(BHandler* connectStateChanged) {
    if (Roster == NULL) {
        MSG("PTP Roster: Starting with Messenger\n");
        Roster = new PTPRoster(connectStateChanged);
        Roster->Start();
    }
}

void StopPTP() {
    if (Roster) {
        Roster->Stop();
        delete Roster;
        Roster = NULL;
    }
}

PTPCamera::PTPCamera(BUSBDevice* device) 
 :  BLocker(),
    isViewFinding(false),
    isShooting(false),
    deviceInfo(NULL),
	dev(NULL), inep(NULL), outep(NULL), intep(NULL),
    fStatus(None),
	SessionID(0),
    byteorder(PTP_DL_LE),
	storage(),
    JPEGTrans(NULL),
	usbintf(),
	pictures(),
    picturesRetrieved(false)
{
    SetTo(device);
}

PTPCamera::~PTPCamera() {
    if (dev) {
        if (isViewFinding) SetViewFinding(false);
        if (isShooting) SetShooting(false);
        if (fStatus == Ready) CloseSession();
    }
	pictures.Clear();
    storage.clear();
    if (JPEGTrans) delete JPEGTrans;
    if (deviceInfo) delete deviceInfo;
}

status_t PTPCamera::SetTo(BUSBDevice* device) {
    if (!Lock()) return B_ERROR;
    const BUSBInterface* intf;
    const BUSBEndpoint* ep;
    status_t status = B_ERROR;

    pictures.Clear();
	picturesRetrieved = false;
    storage.clear();
    
    if (deviceInfo) {
        delete deviceInfo;
        deviceInfo = NULL;
    }
    
	dev = NULL;
	fStatus = None;
	isShooting = false;
	isViewFinding = false;
	productName = "None";

	if (!device) {
        Unlock();
        return B_OK;
    } 

    device->InitCheck();

    status = device->SetConfiguration(device->ConfigurationAt(0));
    intf = device->ActiveConfiguration()->InterfaceAt(0);

    DEBUG("SetTo: Checking USB device %s 0x%0x (%s 0x%0x)...\n", device->ProductString(), device->ProductID(), device->ManufacturerString(), device->VendorID());
    DEBUG("SetTo: Checking %" B_PRId32 " endpoints...\n", intf->CountEndpoints());
    for (uint i = 0; i < intf->CountEndpoints(); i++) {
        ep = intf->EndpointAt(i);
        DEBUG("SetTo: Endpoint %i: %s%s%s %s.\n", i, ep->IsBulk() ? "Bulk " : "", ep->IsInput() ? "Input" : "", ep->IsOutput() ? "Output" : "", ep->IsInterrupt() ? "Interrput" : "");
        if (ep->IsBulk()) {
            if (ep->IsInput()) {
                inep = ep;
                DEBUG("Endpoint %i at %02x is bulk input\n", i, ep->Descriptor()->endpoint_address);
            } else {
                outep = ep;
                DEBUG("Endpoint %i at %02x is bulk output\n", i, ep->Descriptor()->endpoint_address);
            }
        } else if (ep->IsInterrupt()) {
            intep = ep;
            DEBUG("Endpoint %i at %02x is interrupt\n", i, ep->Descriptor()->endpoint_address);
        }
    }
    if ((inep) && (outep) && (intep)) {
        DEBUG("All endpoints found, camera %s/%s status set to identified.\n", device->ProductString(), device->ManufacturerString());
        dev = device;
        productName.SetToFormat("%s - %s", dev->ManufacturerString(), dev->ProductString());
        fStatus = Identified;
    }
    Unlock();
    return status;
}

CameraStatus
PTPCamera::Status() {
    return fStatus;
}

PictureList*
PTPCamera::Pictures() {
    if (!picturesRetrieved) 
        UpdatePictures();

    return &pictures;
}

status_t 
PTPCamera::Connect() {
	if (fStatus == Ready) return B_OK;
	if (fStatus != Identified) return B_DEV_NOT_READY;
    if (!Lock()) return B_DEV_NOT_READY;
    DEBUG("Connect...\n");
	UpdateDeviceInfo();
    status_t status = OpenSession(1);
    Unlock();
    return status;
}

status_t 
PTPCamera::Disconnect() {
    return CloseSession();
}

status_t 
PTPCamera::UpdatePictures() {
    if (fStatus != Ready || !Lock()) return B_DEV_NOT_READY; 
	
    pictures.Clear();
    picturesRetrieved = false;
    status_t status = GetStorage();
    uint32 i = 0;
    int32 count = 0;
    while (status >= 0 && i < storage.size()) {
        status_t status = GetObjects(storage[i]);
        count += status;
        i++;
    }
    Unlock();
    if (status >= B_OK) {
        picturesRetrieved = true;
        return count;
    } else 
        return status;
}

status_t 
PTPCamera::GetThumb(int num, BBitmap **bmp) {
	if (num < 0 || num >= pictures.Count()) return B_BAD_INDEX;
    if (fStatus != Ready || !Lock()) return B_DEV_NOT_READY;

    PTPContainer ptp;
    char* object = NULL;
    *bmp = NULL;
    PTP_CNT_INIT(ptp);
    ptp.Code = PTP_OC_GetThumb;
    ptp.Param1 = pictures.Item(num)->Handle;
    ptp.Nparam = 1;
    DEBUG("Get Thumb for Picture %s with handle %" B_PRId32 ".\n", pictures.Item(num)->Name(), ptp.Param1);
    status_t status = Transaction(&ptp, PTP_DP_GETDATA, 0, &object);
    if (status >= B_OK) {
        BMemoryIO thumbData(object, status);
        *bmp = BTranslationUtils::GetBitmap(&thumbData);
        free(object);
    }
    Unlock();
    return status;
}

void 
SetJPEGFileAttrs(BFile *fh, PictureDesc *item) {
    BNodeInfo* ni = new BNodeInfo(fh);
    int32 width = (item->Orientation == 1 || item->Orientation == 7) ? item->XRes : item->YRes;
    int32 height = (item->Orientation == 1 || item->Orientation == 7) ? item->YRes : item->XRes;
    bool  flash = item->Flash ? true : false;
    bigtime_t time = mktime(gmtime((const time_t *) (&(item->Date))));

    fh->WriteAttr("Media:Width", B_INT32_TYPE, 0, (const void *) &width, sizeof (int32));
    fh->WriteAttr("Media:Height", B_INT32_TYPE, 0, (const void *) &height, sizeof (int32));
    fh->WriteAttr("EXIF:Flash", B_BOOL_TYPE, 0, (const void *) &flash, sizeof (bool));
    fh->WriteAttr("EXIF:Date", B_TIME_TYPE, 0, (const void *) &time, sizeof (time));
    switch (item->Type) {
        case picJPEG:
            ni->SetType("image/jpeg");
            break;
        case picMovie:
            ni->SetType("video/x-msvideo");
            break;
        case picRaw:
            ni->SetType("image/raw");
        default:
            DEBUG("SetJPegFileAttrs: Picture type unknown\n");
    }
    if (item->thumb) {
        BRect thumbRect = item->thumb->Bounds();
        for (int i = 0; i < 2; i++) {
            int size = 16 << (i*i);
            BBitmap* icon	= new BBitmap(BRect(0, 0, size - 1, size - 1), B_RGB32, true, false);
            BView* v		= new BView(icon->Bounds(), "temp", B_FOLLOW_NONE, 0);
            icon->AddChild(v);
            icon->Lock();
            v->DrawBitmap(item->thumb, thumbRect, icon->Bounds());
            icon->Unlock();
            icon->RemoveChild(v);
            delete v;
            switch(i) {
                case 0:
                    ni->SetIcon(icon, B_MINI_ICON);
                    break;
                case 1:
                    ni->SetIcon(icon, B_LARGE_ICON);
                    break;
            }
            delete icon;
        }
    }
    delete ni;
}

status_t 
PTPCamera::GetPicture(int num, BPath* savedir, UpdateCallBackFunc func) {
    if (!Lock() || fStatus != Ready) return B_DEV_NOT_READY;
	if (num < 0 || num >= pictures.Count()) return B_BAD_INDEX;

    PTPContainer ptp;
    char* object = NULL;

    PTP_CNT_INIT(ptp);
    ptp.Code = PTP_OC_GetObject;
    ptp.Param1 = pictures.Item(num)->Handle;
    ptp.Nparam = 1;
    status_t status = Transaction(&ptp, PTP_DP_GETDATA, 0, &object, func);
    if (status >= B_OK) {
        BEntry saveEntry(savedir->Path());
        if (saveEntry.IsDirectory())
            savedir->Append(pictures.Item(num)->Name());
        BFile picFile(savedir->Path(), B_WRITE_ONLY | B_CREATE_FILE);
        if (picFile.InitCheck() == B_OK) {
            picFile.Write(object, status);
            SetJPEGFileAttrs(&picFile, pictures.Item(num));
            DEBUG("GetPicture: Written %" B_PRId32 " bytes to %s.\n", status, savedir->Path());
        }
        free(object);
    }
    Unlock();
    return status;
}

status_t 
PTPCamera::DeletePicture(int num) {
    if (!Lock() || fStatus != Ready) return B_DEV_NOT_READY;
	if (num < 0 || num >= pictures.Count()) return B_BAD_INDEX;

    PTPContainer ptp;
    PTP_CNT_INIT(ptp);
    ptp.Code = PTP_OC_DeleteObject;
    ptp.Param1 = pictures.Item(num)->Handle;
    ptp.Param2 = 0;
    ptp.Nparam = 2;
    status_t status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);

    Unlock();
    return status;
}

status_t 
PTPCamera::SetShooting(bool OnOff) {
    if (OnOff == isShooting) return B_OK;
    if (!Lock()) return B_DEV_NOT_READY;

    status_t status = B_ERROR;
    PTPContainer ptp;
    DEBUG("Set Shooting to %s\n", OnOff ? "On" : "Off");
    if (VendorId() == PTP_VENDOR_CANON &&
			deviceInfo->OperationsSupported.count(PTP_OC_CANON_InitiateReleaseControl)) {  // Canon
        PTP_CNT_INIT(ptp);
        ptp.Code = OnOff ? PTP_OC_CANON_InitiateReleaseControl : PTP_OC_CANON_TerminateReleaseControl;
        ptp.Nparam = 0;
        status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
        
        PTP_CNT_INIT(ptp);
        ptp.Code = PTP_OC_CANON_DoAeAfAwb;
        ptp.Nparam = 1;
        ptp.Param1 = 7;
        status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
        
        UpdateDeviceInfo();
	} else if (VendorId() == PTP_VENDOR_CANON &&
			deviceInfo->OperationsSupported.count(PTP_OC_CANON_EOS_RemoteReleaseOn)) {  // Canon
        PTP_CNT_INIT(ptp);
        ptp.Code = OnOff ? PTP_OC_CANON_EOS_RemoteReleaseOn : PTP_OC_CANON_EOS_RemoteReleaseOff;
        ptp.Nparam = 0;
        status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
        
        PTP_CNT_INIT(ptp);
        ptp.Code = PTP_OC_CANON_EOS_DoAf;
        ptp.Nparam = 1;
        ptp.Param1 = 7;
        status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
        
        UpdateDeviceInfo();
		
    } else if (VendorId() == PTP_VENDOR_NIKON &&
			deviceInfo->OperationsSupported.count(PTP_OC_NIKON_SetControlMode)) { // Nikon
        if (OnOff) {
            PTP_CNT_INIT(ptp);
            ptp.Code = PTP_OC_NIKON_SetControlMode;
            ptp.Nparam = 1;
            ptp.Param1 = 1;
            status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
        } else {
            PTP_CNT_INIT(ptp);
            ptp.Code = PTP_OC_NIKON_EndLiveView;
            ptp.Nparam = 0;
            status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);

            PTP_CNT_INIT(ptp);
            ptp.Code = PTP_OC_NIKON_SetControlMode;
            ptp.Nparam = 1;
            ptp.Param1 = 0;
            status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
        }
    } 
    if (status >= B_OK) 
        isShooting = OnOff;
    Unlock();
    return status;
}

status_t 
PTPCamera::TakePicture(BPath* path) {
    status_t status = B_ERROR;
    if (!Lock()) return B_BUSY;
    int retries;
    int Handle = 0;
    PTPContainer ptp;
    PTPPropertyValue v;
    PTPObjectInfoRaw* oi = NULL;
	
    switch (VendorId()) {
    	case PTP_VENDOR_CANON: { 
			if (deviceInfo->OperationsSupported.count(PTP_OC_CANON_InitiateCaptureInMemory)) {
				MSG("Take Picture Canon: Model does not support remote capture\n");
				return B_NOT_ALLOWED;
			}
            DEBUG("Take Picture Canon: Starting..\n");

            bool wasShooting = isShooting;
            if (!isShooting) SetShooting(true);
            bool wasViewFinding = isViewFinding;
            if (wasViewFinding) SetViewFinding(false);
            
            v.u16 = CANON_TRANSFER_MEMORY; 
            status = SetProperty(PTP_DPC_CANON_CaptureTransferMode, &v, PTP_DTC_UINT16);
            DEBUG("Take Picture: CaptureTransfermode set with status 0x%08" B_PRIx32 ".\n", status);
            PTP_CNT_INIT(ptp);
            ptp.Code = PTP_OC_CANON_DoAeAfAwb;
            ptp.Nparam = 1;
            ptp.Param1 = 7;
            status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
            DEBUG("Take Picture: DoAeAfAwb with status 0x%08" B_PRIx32 ", return code 0x%04x and Param 0x%08" B_PRIx32 ".\n", status, ptp.Code, ptp.Param1);


            v.u16 = 4;
            status = SetProperty(PTP_DPC_CANON_EventEmulateMode, &v, PTP_DTC_UINT16);
            DEBUG("Capture: EventEmulateMode set to 4, returned status 0x%08" B_PRIx32 ".\n", status);
            PTP_CNT_INIT(ptp);
            ptp.Code = PTP_OC_CANON_DoAeAfAwb;
            ptp.Nparam = 1;
            ptp.Param1 = 7;
            status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
            DEBUG("Take Picture: DoAeAfAwb with status 0x%08" B_PRIx32 ", return code 0x%04" B_PRIx16 " and Param 0x%08" B_PRIx32 ".\n", status, ptp.Code, ptp.Param1);


            PTP_CNT_INIT(ptp);
            ptp.Code = PTP_OC_CANON_InitiateCaptureInMemory;
            ptp.Nparam = 0;
            DEBUG("Take Picture: Initiating Capture to Memory - Params: %" B_PRId32 ", %" B_PRId32 ", %" B_PRId32 ", %" B_PRId32 ", %" B_PRId32 " - %d.\n", 
            		ptp.Param1, ptp.Param2, ptp.Param3, ptp.Param4, ptp.Param5, ptp.Nparam);
            status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
            DEBUG("Take Picture: Returned with status 0x%08" B_PRIx32 " - Params: %" B_PRId32 ", %" B_PRId32 ", %" B_PRId32 ", %" B_PRId32 ", %" B_PRId32 " - %d.\n", 
            		status, ptp.Param1, ptp.Param2, ptp.Param3, ptp.Param4, ptp.Param5, ptp.Nparam);

            if (status >= 0) {

                DEBUG("Take Picture: Checking for events..\n");
                for (retries = 5; retries > 0; retries--) {
                    PTPUSBEventsContainer etp;
                    status = CheckIntEvent(&etp, true);
                    DEBUG("Take Picture: Check int events returned - status 0x%08" B_PRIx32 ", code 0x%04" B_PRIx16 ", type 0x%04" B_PRIx16 " Params %" B_PRId32 ", %" B_PRId32 ", %" B_PRId32 ".\n", 
							status, etp.event[0].Code, etp.event[0].Type, 
							etp.event[0].Param1, etp.event[0].Param2, etp.event[0].Param3);
                    if (status < 0) {
                        snooze(50000);
                        continue;
                    } 
                    switch (etp.event[0].Code) {
                        case PTP_EC_StorageInfoChanged:
                            DEBUG("Take Picture: Storage Info Changed event received.\n");
                            GetStorage();
                            break;
                        case PTP_EC_CaptureComplete:
                            DEBUG("Take Picture: Capture complete event received.\n");
                            Handle = etp.event[0].Param1;
                            retries = 0;
                            break;
                    }
                } 
            }
            if (status >= 0) {
                for (retries = 100; retries > 0; retries--) {
                    PTPUSBEventsContainer* etp;
                    status = CheckEvent(&etp);
                    if (status < 0) {
                        snooze(50000);
                        continue;
                    }
                    if (etp->event[0].Code == PTP_EC_CANON_RequestObjectTransfer) {
                        DEBUG("Take Picture: Received Object Transfer Request with Params : %" B_PRId32 ", %" B_PRId32 ", %" B_PRId32 ".\n", 
								etp->event[0].Param1, 
								etp->event[0].Param2, 
								etp->event[0].Param3);
                        oi = NULL;
                        Handle = etp->event[0].Param1;
                        free(etp);
                        PTP_CNT_INIT(ptp);
                        ptp.Code = PTP_OC_GetObjectInfo;
                        ptp.Nparam = 1;
                        ptp.Param1 = Handle;
                        status = Transaction(&ptp, PTP_DP_GETDATA, 0, (char**) &oi);
                        DEBUG("Take Picture: Retrieving Object Info returned status 0x%08" B_PRIx32 ".\n", status);
                        if (status >= 0) {
                            PictureDesc* desc = MakePictureDesc(oi);
                            free(oi);
                            if (path) {
                                BEntry pathEntry(path->Path());
                                if (pathEntry.IsDirectory()) {
                                    sprintf(desc->Name(), "Capture%03" B_PRId16 ".jpg", CaptureNum);
                                } else {
                                    sprintf(desc->Name(), "%s", path->Leaf());
                                    path->GetParent(path);
                                }
                            } else sprintf(desc->Name(), "Capture%03" B_PRId16 ".jpg", CaptureNum);
                            desc->Handle = Handle;
                            PTP_CNT_INIT(ptp);
                            ptp.Code = PTP_OC_CANON_GetPartialObjectInfo;
                            ptp.Nparam = 2;
                            ptp.Param1 = Handle;
                            status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
                            DEBUG("Take Picture: Retrieved Object Size as %" B_PRId32 " with status 0x%08" B_PRIx32 ".\n", ptp.Param1, status);
                            desc->Size = ptp.Param1;

                            pictures.Add(desc);
                            DEBUG("Take Picture: Trying to retrieve...\n");
                            status = GetThumb(pictures.Count() - 1, &desc->thumb);
                            if (path) {
                                status = GetPicture(pictures.Count() - 1, path);
                                DEBUG("Take Picture: Get Picture returned with status 0x%08" B_PRIx32 ".\n", status);
                                if (status >= B_OK) 
                                    CaptureNum++;
                                pictures.Remove(pictures.Count() - 1);
                            } else CaptureNum++;
                        } 
                        retries = 0;
                    } else snooze(10000);
                } 
            }
            SetShooting(wasShooting);
            SetViewFinding(wasViewFinding);
            break;
    	}
    	case PTP_VENDOR_NIKON:
    	{
            DEBUG("Take Picture Nikon: Starting..\n");
            bool wasShooting = isShooting;
            bool wasViewFinding = isViewFinding;
            
            if (!isShooting) SetShooting(true);

            PTP_CNT_INIT(ptp);
            if (isViewFinding) {
                DEBUG("Take Picture: Initiating Capture to Memory\n");
                ptp.Code    = PTP_OC_NIKON_Capture;
                ptp.Nparam  = 1;
                ptp.Param1  = 0xFFFFFFFF;
            } else {
                DEBUG("Take Picture: Initiating Capture to Memory with Auto-Focus\n");
                ptp.Code    = PTP_OC_NIKON_AfCaptureSDRAM;
                ptp.Nparam  = 0;
            }
            status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
            DEBUG("Take Picture: Returned with status 0x%08" B_PRIx32 "\n", status);

            if (status < B_OK) { Unlock(); return status; }
            
            for (retries = 100; retries > 0; retries--) {
                PTP_CNT_INIT(ptp);
                ptp.Code    = PTP_OC_NIKON_DeviceReady;
                ptp.Nparam  = 0;
                status      = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
                if (status >= 0) {
                    switch (ptp.Code) {
                        case PTP_RC_OK:
                            retries = 0;
                            continue;
                            break;
                        case PTP_RC_DeviceBusy:
                            snooze(10000);
                            continue;
                            break;
                    }
                }
                snooze(100000);
            }
					    
            if (status < B_OK) { Unlock(); return status; }
            
            int done        = 0;
            int32 newHandle = 0xffff0001; 
            for (retries = 100; retries > 0 && done != 3; retries--) {
                PTPUSBEventsContainer* etp = NULL;

                status = CheckEvent(&etp);
                if (etp == NULL) {
                    snooze(100000);
                    continue;
                }
                if (etp->Length == 0) {
                    snooze(100000);
                    free(etp);
                    continue;
                }
                DEBUG("Take Picture: Check int events returned with status 0x%08" B_PRIx32 " and %" B_PRId32 " events\n", status, etp->Length);

                for (uint32 i = 0; i < etp->Length; i++) {
                    DEBUG("Take Picture: Event %" B_PRId32 ": code 0x%04" B_PRIx16 
							", type 0x%04" B_PRIx16 " Params %" B_PRIx32 ", %" B_PRIx32 
							", %" B_PRIx32 ".\n", i, 
							etp->event[i].Code, 
							etp->event[i].Type, 
							etp->event[i].Param1, 
							etp->event[i].Param2, 
							etp->event[i].Param3);
                    switch(etp->event[i].Code) {
                        case PTP_EC_Nikon_ObjectAddedInSDRAM:
                            done = 1 | 2;
                            newHandle = (etp->event[i].Param1) ? etp->event[i].Param1 : 0xffff0001;
                            break;
                        case PTP_EC_ObjectAdded: {
                            PTPObjectInfoRaw* oi = NULL;
                            PTP_CNT_INIT(ptp);
                            ptp.Code = PTP_OC_GetObjectInfo;
                            ptp.Nparam = 1;
                            ptp.Param1 = etp->event[i].Param1;
                            status = Transaction(&ptp, PTP_DP_GETDATA, 0, (char**) &oi);
                            if (status >= B_OK) {
                                if (oi->ObjectFormat != PTP_OFC_Association) {
                                    newHandle = etp->event[i].Param1;
                                    done |= 2;
                                }
                                free(oi);
                            }
                            break;
                        }
                        case PTP_EC_CaptureComplete:
                            done |= 1;
                            break;
                    }
                }
                free(etp);
            }
            
            oi = NULL;
            PTP_CNT_INIT(ptp);
            ptp.Code = PTP_OC_GetObjectInfo;
            ptp.Nparam = 1;
            ptp.Param1 = newHandle;
            status = Transaction(&ptp, PTP_DP_GETDATA, 0, (char**) &oi);

            if (status < B_OK) { Unlock(); return status; }

            PictureDesc* desc = MakePictureDesc(oi);
            free(oi);
            desc->Handle      = newHandle;
            pictures.Add(desc);
            DEBUG("Take Picture: Trying to retrieve...\n");
            status = GetThumb(pictures.Count() - 1, &desc->thumb);
            if (path) {
                if (BEntry(path->Path()).IsDirectory()) {
                    snprintf(desc->Name(), PICTURE_NAME_MAX_SIZE, "Capture%03" B_PRIx16 ".jpg", CaptureNum);
                } else {
                    snprintf(desc->Name(), PICTURE_NAME_MAX_SIZE, "%s", path->Leaf());
                    path->GetParent(path);
                }
                status = GetPicture(pictures.Count() - 1, path);
                PTP_CNT_INIT(ptp);
                ptp.Code = PTP_OC_DeleteObject;
                ptp.Nparam = 1;
                ptp.Param1 = newHandle;
                Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
                DEBUG("Take Picture: Get Picture returned with status 0x%08" B_PRIx32 ".\n", status);
                if (status >= B_OK) 
                    CaptureNum++;
                pictures.Remove(pictures.Count() - 1);
            }
            retries = 100;
            while (0 < retries--) {
                PTPUSBEventsContainer* etp = NULL;
                CheckEvent(&etp);
                if (etp) {
                    if (etp->Length == 0) retries = 0; 
                    free(etp);
                }
                snooze(100000);
            }
            isViewFinding = false;
            SetShooting(wasShooting);
            SetViewFinding(wasViewFinding);
    	}
    }
    Unlock();
    return status;
}

status_t 
PTPCamera::SetViewFinding(bool onoff) {
    if (isViewFinding == onoff) return B_OK;
    status_t status = B_ERROR;
    if (!Lock()) return status;
    
    PTPContainer ptp;
    DEBUG("SetviewFinder to %s ...\n", onoff ? "On" : "Off");
    if (VendorId() == PTP_VENDOR_CANON) {
        PTP_CNT_INIT(ptp);
		if (deviceInfo->OperationsSupported.count(PTP_OC_CANON_ViewfinderOn))
			ptp.Code = onoff ? PTP_OC_CANON_ViewfinderOn : PTP_OC_CANON_ViewfinderOff;
		else if (deviceInfo->OperationsSupported.count(PTP_OC_CANON_EOS_InitiateViewfinder))
			ptp.Code = onoff ? PTP_OC_CANON_EOS_InitiateViewfinder : PTP_OC_CANON_EOS_TerminateViewfinder;
		else
			return B_NOT_ALLOWED;
		
        PTP_CNT_INIT(ptp);
        ptp.Nparam = 0;
        ptp.Code = onoff ? PTP_OC_CANON_ViewfinderOn : PTP_OC_CANON_ViewfinderOff;
        status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
    } else if (VendorId() == PTP_VENDOR_NIKON) { 
        if (onoff) {
			if (deviceInfo->OperationsSupported.count(PTP_OC_NIKON_StartLiveView)) {
				PTPPropertyValue value;
				value.u8 = 1;
				status = SetProperty(PTP_DPC_NIKON_RecordingMedia, &value, PTP_DTC_UINT8);

				PTP_CNT_INIT(ptp);
				ptp.Nparam = 0;
				ptp.Code = PTP_OC_NIKON_StartLiveView;
				status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
				snooze(5000);
				while (status == B_OK) {
					PTP_CNT_INIT(ptp);
					ptp.Nparam = 0;
					ptp.Code = PTP_OC_NIKON_DeviceReady;
					status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
					if (ptp.Code == PTP_RC_OK) break;
				}
			} else status = B_NOT_ALLOWED;
        } else {
			if (deviceInfo->OperationsSupported.count(PTP_OC_NIKON_EndLiveView)) {
				PTP_CNT_INIT(ptp);
				ptp.Nparam = 0;
				ptp.Code = PTP_OC_NIKON_EndLiveView;
				status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
				while (status == B_OK) {
					PTP_CNT_INIT(ptp);
					ptp.Nparam = 0;
					ptp.Code = PTP_OC_NIKON_DeviceReady;
					status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
					if (ptp.Code == PTP_RC_OK) break;
				}
			} else status = B_NOT_ALLOWED;
			
        }
    }
    if (status >= B_OK) {
        isViewFinding = onoff;
        UpdateDeviceInfo();
		status = B_OK;
    }
    DEBUG("SetViewFinder %s\n", status >= B_OK ? "done" : "failed");
    Unlock();
    return status;
}

status_t 
PTPCamera::GetViewFinderBitmap(BBitmap** bm) {
    if (!isViewFinding) return B_ERROR;
    if (LockWithTimeout(50000) != B_OK) return B_BUSY;

    status_t status = B_ERROR;
    char* image = NULL;

    PTPContainer ptp;

    PTP_CNT_INIT(ptp);
    if (VendorId() == PTP_VENDOR_CANON) {
        ptp.Code = PTP_OC_CANON_GetViewfinderImage;
        ptp.Nparam = 0;
        status = Transaction(&ptp, PTP_DP_GETDATA, 0, &image);
        if (status > 0) {
            DEBUG("ViewFinder %" B_PRId32 " bytes\n", status);
            BMemoryIO memfh(image, status);
            *bm = BTranslationUtils::GetBitmap(&memfh /*, JPEGTrans */);
            free(image);
        } else *bm = NULL;
    } else if (VendorId() == PTP_VENDOR_NIKON) { // Nikon
        ptp.Code = PTP_OC_NIKON_GetLiveViewImg;
        ptp.Nparam = 0;
        status = Transaction(&ptp, PTP_DP_GETDATA, 0, &image);
        if (status >= B_OK) {
            DEBUG("ViewFinder %" B_PRId32 " bytes\n", status);
            char* soi = image;
            while (soi - image < status - 2) {
                if (soi[0] == '\xff' && soi[1] == '\xd8') break;
                soi++;
            }
            if (soi - image < status - 2) {
                BMemoryIO memfh(soi, status + image - soi);
                *bm = BTranslationUtils::GetBitmap(&memfh /*, JPEGTrans */);
            } else *bm = NULL;
            free(image);
        } else *bm = NULL;

    }
    Unlock();
    return status;
}

bool 
PTPCamera::HasProperty(uint16 DevicePropertyCode) {
    if (!deviceInfo) return false;
	return (deviceInfo->DevicePropertiesSupported.count(DevicePropertyCode) != 0);
}


PTPController<uint16>* 
PTPCamera::GetController(uint16 DevicePropertyCode, type_code* type) {
    PTPContainer ptp;
    PTPController<uint16>* result;
    type_code resultType;
    status_t status;

    char* propDescHeader = NULL;
    PTP_CNT_INIT(ptp);
    ptp.Code = PTP_OC_GetDevicePropDesc;
    ptp.Nparam = 1;
    ptp.Param1 = DevicePropertyCode;
    status = Transaction(&ptp, PTP_DP_GETDATA, 0, &propDescHeader);
    DEBUG("GetController: Retrieving description for property %d returned status 0x%08" B_PRIx32 ".\n", DevicePropertyCode, status);
    if (status >= B_OK) {
        char* raw = propDescHeader;
        DevicePropertyCode = PTPUInt16(raw);
        uint16 DataType    = PTPUInt16(raw);
        switch (DataType) {
            case PTP_DTC_UINT8: {
                PTPDevicePropDesc<uint8>* propDesc = (PTPDevicePropDesc<uint8>*)propDescHeader;
                
                if (propDesc->FormFlag == PTP_DPFF_Enumeration &&
                        propDesc->Form.Enum.numberOfValues == 2 &&
                        propDesc->Form.Enum.supportedValue[0] == 0 &&
                        propDesc->Form.Enum.supportedValue[1] == 1) {
                    resultType = B_BOOL_TYPE;
                    PTPDevicePropDesc<bool>* propDescBool = (PTPDevicePropDesc<bool>*)propDescHeader;
                    result = (PTPController<uint16>*)new PTPController<bool>(this, propDescBool, resultType);
                } else {
                    resultType = B_UINT8_TYPE;
                    result = (PTPController<uint16>*)new PTPController<uint8>(this, propDesc, resultType);
                }
                break;
            }
            case PTP_DTC_UINT16: {
                resultType = B_UINT16_TYPE;
                PTPDevicePropDesc<uint16>* propDesc = (PTPDevicePropDesc<uint16>*)propDescHeader;
                propDesc->CurrentValue = B_LENDIAN_TO_HOST_INT16(propDesc->CurrentValue);
                propDesc->FactoryDefaultValue = B_LENDIAN_TO_HOST_INT16(propDesc->FactoryDefaultValue);
                result = new PTPController<uint16>(this, propDesc, resultType);
                break;
            }
            case PTP_DTC_UINT32: {
                resultType = B_ANY_TYPE;
                PTPDevicePropDesc<uint32>* propDesc = (PTPDevicePropDesc<uint32>*)propDescHeader;
                propDesc->CurrentValue = B_LENDIAN_TO_HOST_INT32(propDesc->CurrentValue);
                propDesc->FactoryDefaultValue = B_LENDIAN_TO_HOST_INT32(propDesc->FactoryDefaultValue);
                result = (PTPController<uint16>*)new PTPController<uint32>(this, propDesc, resultType);
                break;
            }
            default:
                resultType = B_ANY_TYPE;
                free(propDescHeader);
                result = NULL;
                break;
        }
    } else {
        result = NULL;
        resultType = B_ANY_TYPE;
    }
    if (type) *type = resultType;
    return result;
};

status_t 
PTPCamera::GetProperty(uint16 propcode, PTPPropertyValue* propvalue, uint16 datatype) {
    if (!Lock()) return B_BUSY;
    
    status_t status;
    PTPContainer ptp;
    char* dpv = NULL;

    PTP_CNT_INIT(ptp);
    ptp.Code = PTP_OC_GetDevicePropValue;
    ptp.Param1 = propcode;
    ptp.Nparam = 1;
    status = Transaction(&ptp, PTP_DP_GETDATA, 0, &dpv);
    if (status > 0) {
        switch (datatype) {
            case PTP_DTC_INT8:
                propvalue = (PTPPropertyValue*) malloc(sizeof (int8));
                propvalue->i8 = dpv[0];
                break;
            case PTP_DTC_UINT8:
                propvalue = (PTPPropertyValue*) malloc(sizeof (uint8));
                propvalue->u8 = dpv[0];
                break;
            case PTP_DTC_INT16:
                propvalue = (PTPPropertyValue*) malloc(sizeof (int16));
                propvalue->i16 = B_LENDIAN_TO_HOST_INT16(*(int16*) dpv);
                break;
            case PTP_DTC_UINT16:
                propvalue = (PTPPropertyValue*) malloc(sizeof (uint16));
                propvalue->u16 = B_LENDIAN_TO_HOST_INT16(*(uint16*) dpv);
                break;
            case PTP_DTC_INT32:
                propvalue = (PTPPropertyValue*) malloc(sizeof (int32));
                propvalue->i16 = B_LENDIAN_TO_HOST_INT32(*(int32*) dpv);
                break;
            case PTP_DTC_UINT32:
                propvalue = (PTPPropertyValue*) malloc(sizeof (uint32));
                propvalue->u16 = B_LENDIAN_TO_HOST_INT32(*(uint32*) dpv);
                break;
            case PTP_DTC_STR:
                uint8 len = dpv[0];
                propvalue = (PTPPropertyValue*) malloc(len);
                for (uint i = 0; i < len; i++) propvalue->str[i] = dpv[2 * i + 1];
                propvalue->str[len] = 0;
        }
        free(dpv);
    }
    Unlock();
    return status;
}

status_t 
PTPCamera::SetProperty(uint16 propcode, PTPPropertyValue* propvalue, uint16 datatype) {
    if (!Lock()) return B_BUSY;
    status_t status;
    PTPContainer ptp;
    char* dpv = NULL;
    int len = 0;
    PTP_CNT_INIT(ptp);
    ptp.Code = PTP_OC_SetDevicePropValue;
    ptp.Param1 = propcode;
    ptp.Nparam = 1;

    switch (datatype) {
        case PTP_DTC_INT8:
            len = sizeof (int8);
            dpv = (char*) malloc(len);
            *(int8*) dpv = propvalue->i8;
            break;
        case PTP_DTC_UINT8:
            len = sizeof (uint8);
            dpv = (char*) malloc(len);
            *(uint8*) dpv = propvalue->u8;
            break;
        case PTP_DTC_INT16:
            len = sizeof (int16);
            dpv = (char*) malloc(len);
            *(int16*) dpv = B_HOST_TO_LENDIAN_INT16(propvalue->i16);
            break;
        case PTP_DTC_UINT16:
            len = sizeof (uint16);
            dpv = (char*) malloc(len);
            *(uint16*) dpv = B_HOST_TO_LENDIAN_INT16(propvalue->u16);
            break;
        case PTP_DTC_INT32:
            len = sizeof (int32);
            dpv = (char*) malloc(len);
            *(int32*) dpv = B_HOST_TO_LENDIAN_INT32(propvalue->i32);
            break;
        case PTP_DTC_UINT32:
            len = sizeof (uint32);
            dpv = (char*) malloc(len);
            *(uint32*) dpv = B_HOST_TO_LENDIAN_INT32(propvalue->u32);
            break;
        case PTP_DTC_STR:
            uint8 len = strlen(&propvalue->str[0]);
            dpv = (char*) malloc(len * 2 + 1);
            dpv[0] = len;
            for (int i = 0; i < len; i++) ((uint16*) (&dpv[1]))[i] = B_HOST_TO_LENDIAN_INT16(propvalue->str[i]);
            len = 2 * len + 1;

    }
    if (dpv) {
        status = Transaction(&ptp, PTP_DP_SENDDATA, len, &dpv);
        free(dpv);
    } else 
        status = B_ERROR;
    Unlock();
    return status;
}

status_t 
PTPCamera::OpenSession(uint32 NewSessionID) {
    if (!Lock()) return B_BUSY;
    status_t status;
    PTPContainer ptp;
    //DEBUG("OpenSession: Starting...\n");
    fStatus = Busy;

    SessionID = 0;
    TransactionID = 0;

    PTP_CNT_INIT(ptp);
    ptp.Code = PTP_OC_OpenSession;
    ptp.Param1 = NewSessionID;
    ptp.Nparam = 1;
    status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
    /* now set the global session id to current session number */
    if (status >= B_OK) {
        SessionID = NewSessionID;
        fStatus = Ready;
        status = B_OK;
    } else fStatus = Identified;
    DEBUG("OpenSession: Completed with status 0x%08" B_PRIx32 ", Session ID is %" B_PRId32 ".\n", status, SessionID);
    Unlock();
    return status;
}

status_t 
PTPCamera::CloseSession() {
    status_t status;
    PTPContainer ptp;

    DEBUG("CloseSession: Starting...\n");

    PTP_CNT_INIT(ptp);
    ptp.Code = PTP_OC_CloseSession;
    ptp.Nparam = 0;
    status = Transaction(&ptp, PTP_DP_NODATA, 0, NULL);
    DEBUG("CloseSession: Completed with status 0x%08" B_PRIx32 ".\n", status);
    SessionID = 0;
    fStatus = Identified;
    return status;
}

status_t 
PTPCamera::Transaction(PTPContainer *ptp, uint16 flags, uint32 length, char** data, UpdateCallBackFunc func) {
    status_t status = B_ERROR;
    ptp->TransactionID = TransactionID++;
    ptp->SessionID = SessionID;

    DEBUG("Transaction: Session %" B_PRId32 ", Transaction %" B_PRIx32 ", Code: %" B_PRIx16 ".\n", ptp->SessionID, ptp->TransactionID, ptp->Code);
    if (usbintf.LockWithTimeout(100000) != B_OK) return B_BUSY;
    /* send request */
    status = SendRequest(ptp);
    /* is there a dataphase? */
    switch (flags & PTP_DP_DATA_MASK) {
        case PTP_DP_SENDDATA:
            status = SendData(ptp, *data, length);
            break;
        case PTP_DP_GETDATA:
            status = GetData(ptp, data, func);
            break;
        case PTP_DP_NODATA:
            break;
        default:
            usbintf.Unlock();
            return B_ERROR;
    }
    /* get response */
    status_t respStatus = GetResponse(ptp);
    if (respStatus <= B_OK) status = respStatus;
    usbintf.Unlock();
    return status;
}

status_t 
PTPCamera::SendRequest(PTPContainer *req) {
    status_t status;
    PTPUSBBulkContainer usbreq;

    /* build appropriate USB container */
    PTP_CNT_INIT(usbreq);
    usbreq.Length = B_HOST_TO_LENDIAN_INT32(PTP_USB_BULK_REQ_LEN - (sizeof (uint32)*(5 - req->Nparam)));
    usbreq.Type = B_HOST_TO_LENDIAN_INT16(PTP_USB_CONTAINER_COMMAND);
    usbreq.Code = B_HOST_TO_LENDIAN_INT16(req->Code);
    usbreq.TransactionID = B_HOST_TO_LENDIAN_INT32(req->TransactionID);
    usbreq.payload.params.Param1 = B_HOST_TO_LENDIAN_INT32(req->Param1);
    usbreq.payload.params.Param2 = B_HOST_TO_LENDIAN_INT32(req->Param2);
    usbreq.payload.params.Param3 = B_HOST_TO_LENDIAN_INT32(req->Param3);
    usbreq.payload.params.Param4 = B_HOST_TO_LENDIAN_INT32(req->Param4);
    usbreq.payload.params.Param5 = B_HOST_TO_LENDIAN_INT32(req->Param5);
    /* send it to responder */
    status = outep->BulkTransfer((unsigned char *) &usbreq, PTP_USB_BULK_REQ_LEN - (sizeof (uint32)*(5 - req->Nparam)));

    return status;

}

status_t 
PTPCamera::SendData(PTPContainer *ptp, char *data, uint32 length) {
    status_t status;
    PTPUSBBulkContainer usbdata;
    /* build appropriate USB container */
    usbdata.Length = B_HOST_TO_LENDIAN_INT32(PTP_USB_BULK_HDR_LEN + length);
    usbdata.Type = B_HOST_TO_LENDIAN_INT16(PTP_USB_CONTAINER_DATA);
    usbdata.Code = B_HOST_TO_LENDIAN_INT16(ptp->Code);
    usbdata.TransactionID = B_HOST_TO_LENDIAN_INT32(ptp->TransactionID);
    memcpy(usbdata.payload.data, data, MIN(length, PTP_USB_BULK_PAYLOAD_LEN));
    /* send first part of data */
    status = outep->BulkTransfer((unsigned char *) &usbdata, PTP_USB_BULK_HDR_LEN + MIN(length, PTP_USB_BULK_PAYLOAD_LEN));
    if (status < B_OK) {
        DEBUG("SendData: request code 0x%04x sending data error 0x%08" B_PRIx32 ".\n", ptp->Code, status);
        return status;
    }

    if (length <= PTP_USB_BULK_PAYLOAD_LEN) return status;

    /* if everything OK send the rest */
    status = outep->BulkTransfer(data + PTP_USB_BULK_PAYLOAD_LEN, length - PTP_USB_BULK_PAYLOAD_LEN);
    if (status < B_OK) {
        DEBUG("SendData: request code 0x%04" B_PRIx16" sending data error 0x%08" B_PRIx32 ".\n", ptp->Code, status);
        return status;
    }
    return status;
}

status_t 
PTPCamera::GetData(PTPContainer *ptp, char **data, UpdateCallBackFunc func) {
    status_t status;
    uint32 len, read;
    uint32 progress = 0;
    PTPUSBBulkContainer usbdata;

    PTP_CNT_INIT(usbdata);

    /* read first(?) part of data */
    status = inep->BulkTransfer((unsigned char *) &usbdata, sizeof (usbdata));
    if (status < B_OK) {
        DEBUG("GetData: Reading of %ld bytes failed with status 0x%08" B_PRIx32 "\n.", 
        		sizeof(usbdata), status);
        return status;
    }
    if (B_LENDIAN_TO_HOST_INT16(usbdata.Type) != PTP_USB_CONTAINER_DATA) {
        DEBUG("GetData: Expected data container, received %04x.\n", B_LENDIAN_TO_HOST_INT16(usbdata.Type));
        status = B_ERROR;
        return status;
    }
    if (B_LENDIAN_TO_HOST_INT16(usbdata.Code) != ptp->Code) {
        DEBUG("GetData: Expected code %02" B_PRIx16 ", received %02" B_PRIx16 ".\n", ptp->Code, B_LENDIAN_TO_HOST_INT16(usbdata.Code));
        status = B_ERROR;
        return status;
    }

    /* evaluate data length */
    len = B_LENDIAN_TO_HOST_INT32(usbdata.Length) - PTP_USB_BULK_HDR_LEN;


    if (!len) return len;

    /* allocate memory for data if not allocated already */
    if (*data == NULL) *data = (char*) malloc(len);
    /* copy first part of data to 'data' */
    memcpy(*data, usbdata.payload.data,
            PTP_USB_BULK_PAYLOAD_LEN < len ?
            PTP_USB_BULK_PAYLOAD_LEN : len);

    /* is that all of data? */
    progress = read = status - PTP_USB_BULK_HDR_LEN;

    if (func) func(read);

    /* if not, read the rest of it */
    while (read < len) {
        status = inep->BulkTransfer(((unsigned char *) (*data)) + read, MIN(len - read, 0x1000));
        if (status >= 0) {
            read += status;
            if (func) {
                progress += status;
                if (progress >= 65536) {
                    func(progress);
                    progress = 0;
                }
            }
        } else break;

    }
    if (status < B_OK) {
        DEBUG("GetData: Reading remaining %" B_PRId32 " bytes failed with status 0x%08" B_PRIx32 "\n.", len - read, status);
        return status;
    }

    if (func) func(progress);
    return len;
}

status_t 
PTPCamera::GetResponse(PTPContainer *resp) {
    status_t status;
    PTPUSBBulkContainer usbresp;

    PTP_CNT_INIT(usbresp);
    /* read response, it should never be longer than sizeof(usbresp) */
    status = inep->BulkTransfer((unsigned char *) &usbresp, sizeof (usbresp));
    if (status < B_OK) {
        DEBUG("GetResponse: Reading of %ld bytes failed with status 0x%08" B_PRIx32 ".\n", sizeof(usbresp), status);
        return status;
    }
    if (B_LENDIAN_TO_HOST_INT16(usbresp.Type) != PTP_USB_CONTAINER_RESPONSE) {
        DEBUG("GetResponse: Container not a response but 0x%02x.\n", B_LENDIAN_TO_HOST_INT16(usbresp.Type));
        status = inep->BulkTransfer((unsigned char *) &usbresp, sizeof (usbresp));
        if (B_LENDIAN_TO_HOST_INT16(usbresp.Type) != PTP_USB_CONTAINER_RESPONSE) {
            DEBUG("GetResponse: Container again not a response but 0x%02" B_PRIx16 ".\n", B_LENDIAN_TO_HOST_INT16(usbresp.Type));
            return B_ERROR;
        }
    }
    if (B_LENDIAN_TO_HOST_INT16(usbresp.Code) != PTP_RC_OK) {
        DEBUG("GetResponse: Return code 0x%02" B_PRIx16 ".\n", B_LENDIAN_TO_HOST_INT16(usbresp.Code));
        status = B_ERROR;
    }
    else {
        /* build an appropriate PTPContainer */
        resp->Code = B_LENDIAN_TO_HOST_INT16(usbresp.Code);
        resp->SessionID = SessionID;
        resp->TransactionID = B_LENDIAN_TO_HOST_INT32(usbresp.TransactionID);
        resp->Param1 = B_LENDIAN_TO_HOST_INT32(usbresp.payload.params.Param1);
        resp->Param2 = B_LENDIAN_TO_HOST_INT32(usbresp.payload.params.Param2);
        resp->Param3 = B_LENDIAN_TO_HOST_INT32(usbresp.payload.params.Param3);
        resp->Param4 = B_LENDIAN_TO_HOST_INT32(usbresp.payload.params.Param4);
        resp->Param5 = B_LENDIAN_TO_HOST_INT32(usbresp.payload.params.Param5);
    }
    return status;
}

status_t 
PTPCamera::CheckEvent(PTPUSBEventsContainer **event) {
    status_t status = B_ERROR;
    PTPContainer ptp;

    PTP_CNT_INIT(ptp);
    ptp.Nparam = 0;
    if (VendorId() == PTP_VENDOR_CANON) // Canon
    {
        PTPUSBEventContainerRaw* evdata = NULL;
        ptp.Code = PTP_OC_CANON_CheckEvent;
        status = Transaction(&ptp, PTP_DP_GETDATA, 0, (char**) &evdata);
        if (evdata != NULL) {
            if (status >= B_OK) {
                PTPUSBEventsContainer* result = (PTPUSBEventsContainer*)malloc(sizeof(PTPUSBEventsContainer));
                result->Length = B_LENDIAN_TO_HOST_INT32(evdata->Length);
                result->event[0].Type = B_LENDIAN_TO_HOST_INT16(evdata->Type);
                result->event[0].Code = B_LENDIAN_TO_HOST_INT16(evdata->Code);
                result->event[0].TransactionID = B_LENDIAN_TO_HOST_INT32(evdata->TransactionID);
                result->event[0].Param1 = B_LENDIAN_TO_HOST_INT32(evdata->Param1);
                result->event[0].Param2 = B_LENDIAN_TO_HOST_INT32(evdata->Param2);
                result->event[0].Param3 = B_LENDIAN_TO_HOST_INT32(evdata->Param3);
                DEBUG("Check Event: Received: status 0x%08" B_PRIx32 ", code 0x%04" B_PRIx16 ", type 0x%04" B_PRIx16 " Params %" B_PRId32 ", %" B_PRId32 ", %" B_PRId32 ".\n", 
						status, result->event[0].Code, result->event[0].Type, result->event[0].Param1, result->event[0].Param2, result->event[0].Param3);
                *event = result;
            }
            free(evdata);
        }
    } else if (VendorId() == PTP_VENDOR_NIKON) { // Nikon
        PTPUSBEventContainerNikonRaw* evdata = NULL;
        ptp.Code = PTP_OC_NIKON_CheckEvent;
        status = Transaction(&ptp, PTP_DP_GETDATA, 0, (char**) &evdata);
        if (status >= 2) {
            evdata->Length = B_LENDIAN_TO_HOST_INT16(evdata->Length);
            PTPUSBEventsContainer* result = (PTPUSBEventsContainer*)malloc(sizeof(PTPUSBEventsContainer) + evdata->Length * sizeof(PTPUSBEventItem));
            result->Length = evdata->Length;
            for (int i = 0; i < evdata->Length; i++) {
                result->event[i].Code   = B_LENDIAN_TO_HOST_INT16(evdata->event[i].Code);
                result->event[i].Param1 = B_LENDIAN_TO_HOST_INT32(evdata->event[i].Param1);
                result->event[i].Type   = 0;
                result->event[i].TransactionID = 0;
                result->event[i].Param2 = 0;
                result->event[i].Param3 = 0;
            }
            *event = result;
            status = result->Length;
        } else status = B_BUSY;
    }
    return status;
}

status_t 
PTPCamera::CheckIntEvent(PTPUSBEventsContainer* event, bool wait) {
    status_t status = B_ERROR;
    PTPUSBEventContainerRaw usbevent;
    PTP_CNT_INIT(usbevent);
    status = intep->InterruptTransfer((void*) &usbevent, sizeof (usbevent));

    if (status <= 0) return status;

    event->Length = 1;
    event->event[0].Code = B_LENDIAN_TO_HOST_INT16(usbevent.Code);
    event->event[0].TransactionID = B_LENDIAN_TO_HOST_INT32(usbevent.TransactionID);
    event->event[0].Param1 = B_LENDIAN_TO_HOST_INT32(usbevent.Param1);
    event->event[0].Param2 = B_LENDIAN_TO_HOST_INT32(usbevent.Param2);
    event->event[0].Param3 = B_LENDIAN_TO_HOST_INT32(usbevent.Param3);
    return status;
}

status_t 
PTPCamera::GetStorage() {
    status_t status = B_ERROR;
    PTPContainer ptp;
    char* sids = NULL;

    DEBUG("GetStorage:\n");
    storage.clear();

    PTP_CNT_INIT(ptp);
    ptp.Code = PTP_OC_GetStorageIDs;
    ptp.Nparam = 0;
    status = Transaction(&ptp, PTP_DP_GETDATA, 0, &sids);

    if (status > 0) {
        uint32* data = (uint32*) sids;
        uint32 count = B_LENDIAN_TO_HOST_INT32(*data);
        DEBUG("GetStorage: Received list of %" B_PRId32 " storage IDs\n", count);
        for (uint32 i = 0; i < count; i++) storage.push_back(B_LENDIAN_TO_HOST_INT32(data[i + 1]));
        free(sids);
    }
    return status;
}

uint32 strCpy(char* from, char* to) {
    uint8* len = (uint8*) from;
    uint16* s = (uint16*) (len + 1);
    for (uint8 i = 0; i<*len; i++) {
        to[i] = B_LENDIAN_TO_HOST_INT16(s[i]);
    }
    to[*len] = 0;
    return *len;
}

PictureDesc* 
PTPCamera::MakePictureDesc(PTPObjectInfoRaw* oi) {
    PictureDesc* obj = NULL;
    picType Type = picUnknown;
    char* s = NULL;
    char str[255];
    char tmp[10];
    oi->StorageID = B_LENDIAN_TO_HOST_INT32(oi->StorageID);
    oi->ObjectFormat = B_LENDIAN_TO_HOST_INT16(oi->ObjectFormat);
    oi->ObjectCompressedSize = B_LENDIAN_TO_HOST_INT32(oi->ObjectCompressedSize);
    oi->ImagePixWidth = B_LENDIAN_TO_HOST_INT32(oi->ImagePixWidth);
    oi->ImagePixHeight = B_LENDIAN_TO_HOST_INT32(oi->ImagePixHeight);
    switch (oi->ObjectFormat) {
        case PTP_OFC_Undefined:
            if (VendorId() == PTP_VENDOR_NIKON && oi->ThumbFormat == PTP_OFC_JFIF) // Nikon Raw
                Type = picRaw;
            break;
        case PTP_OFC_JP2:
        case PTP_OFC_JPX:
        case PTP_OFC_EXIF_JPEG:
            Type = picJPEG;
            break;
        case PTP_OFC_CANON_RAW:
            Type = picRaw;
            break;
        case PTP_OFC_CIFF:
        case PTP_OFC_TIFF_EP:
        case PTP_OFC_FlashPix:
        case PTP_OFC_BMP:
        case PTP_OFC_GIF:
        case PTP_OFC_PCD:
        case PTP_OFC_PICT:
        case PTP_OFC_PNG:
        case PTP_OFC_JFIF:
        case PTP_OFC_TIFF:
        case PTP_OFC_TIFF_IT:
            Type = picOther;
            break;
        case PTP_OFC_AVI:
        case PTP_OFC_MPEG:
        case PTP_OFC_ASF:
            Type = picMovie;
            break;
        default:
            DEBUG("Unknown object type 0x%x encountered\r", oi->ObjectFormat);
            Type = picUnknown;
    }
    if (Type != picUnknown) {
        obj = new PictureDesc();
        obj->Type = Type;
        obj->Handle = 0;
        obj->Size = oi->ObjectCompressedSize;
        obj->Orientation = oi->ImagePixWidth > oi->ImagePixHeight ? 1 : 2;
        obj->XRes = oi->ImagePixWidth;
        obj->YRes = oi->ImagePixHeight;
        s = &(oi->StringList[0]);
        s = s + 2 * strCpy(s, obj->Name()) + 1;
        s = s + 2 * strCpy(s, str) + 1;
        strncpy(tmp, str, 4);
        tmp[4] = 0;
        obj->Date.tm_year = atoi(tmp) - 1900;
        strncpy(tmp, str + 4, 2);
        tmp[2] = 0;
        obj->Date.tm_mon = atoi(tmp) - 1;
        strncpy(tmp, str + 6, 2);
        tmp[2] = 0;
        obj->Date.tm_mday = atoi(tmp);
        strncpy(tmp, str + 9, 2);
        tmp[2] = 0;
        obj->Date.tm_hour = atoi(tmp);
        strncpy(tmp, str + 11, 2);
        tmp[2] = 0;
        obj->Date.tm_min = atoi(tmp);
        strncpy(tmp, str + 13, 2);
        tmp[2] = 0;
        obj->Date.tm_sec = atoi(tmp);
    }
    return obj;
}

status_t 
PTPCamera::GetObjects(uint32 storage) {
    status_t status = B_ERROR;
    PTPContainer ptp;
    uint32* data = NULL;
    PictureDesc* obj;

    PTP_CNT_INIT(ptp);
    ptp.Code = PTP_OC_GetObjectHandles;
    ptp.Param1 = storage;
    ptp.Param2 = 0;
    ptp.Param3 = 0;
    ptp.Nparam = 3;
    status = Transaction(&ptp, PTP_DP_GETDATA, 0, (char**)&data);
    if (status > 0) {
        uint32 count = B_LENDIAN_TO_HOST_INT32(*data);
        DEBUG("GetObjects: Received list of %" B_PRId32 " object IDs\n", count);
        for (uint32 i = 0; i < count; i++) {
            char* oi = NULL;

            uint32 Handle = B_LENDIAN_TO_HOST_INT32(data[i+1]);

            PTP_CNT_INIT(ptp);
            ptp.Code = PTP_OC_GetObjectInfo;
            ptp.Param1 = Handle;
            ptp.Nparam = 1;
            status = Transaction(&ptp, PTP_DP_GETDATA, 0, &oi);
            if (status >= B_OK) {
                obj = MakePictureDesc((PTPObjectInfoRaw*) oi);
                free(oi);
                if (obj) {
                    obj->Handle = Handle;
                    pictures.Add(obj);
                    DEBUG("GetObjects: Object %s of type 0x%04x added\n", obj->Name(), ((PTPObjectInfoRaw*) oi)->ObjectFormat);
                }
            }
        }
        free(data);
    }
    return status;
}

status_t 
PTPCamera::UpdateDeviceInfo() {
    PTPContainer ptp;
    status_t status = B_ERROR;
    char* DeviceInfoRaw = NULL;
    PTP_CNT_INIT(ptp);
    ptp.Code = PTP_OC_GetDeviceInfo;
    ptp.Nparam = 0;
    status = Transaction(&ptp, PTP_DP_GETDATA, 0, (char**) &DeviceInfoRaw);
    if (status >= B_OK) {
        if (deviceInfo) 
            delete deviceInfo;
        deviceInfo = new PTPDeviceInfo(DeviceInfoRaw);
        if (deviceInfo->VendorExtensionId == PTP_VENDOR_MICROSOFT) {
            switch (dev->VendorID()) {
                case USB_VENDOR_ID_CANON :
                    deviceInfo->VendorExtensionId = PTP_VENDOR_CANON;
                    break;
                case USB_VENDOR_ID_NIKON :
                    deviceInfo->VendorExtensionId = PTP_VENDOR_NIKON;
                    break;
            }
        }
        productName.SetToFormat("%s - %s", deviceInfo->Manufacturer.String(), deviceInfo->Model.String());
        free(DeviceInfoRaw);
    }
    return status;
}

BString 
PTPString(char** raw) {
    BString result;
    uint8 length = *((uint8*)*raw); *raw += 1;
    for (int i = 0; i< length; i++) {
        result << (char)B_LENDIAN_TO_HOST_INT16(*((uint16*)*raw)); *raw += 2;
    }
    return result;
}

void 
PTPArrayUInt16(vector<uint16>* list, char** raw) {
    list->clear();
    uint32 length = B_LENDIAN_TO_HOST_INT32(*((uint32*)*raw));*raw += 4;
    for (uint i = 0; i < length; i++) {
        list->push_back(B_LENDIAN_TO_HOST_INT16(*((uint16*)*raw)));
        *raw += 2;
    }
}

void 
PTPSetUInt16(set<uint16>* list, char** raw) {
    list->clear();
    uint32 length = B_LENDIAN_TO_HOST_INT32(*((uint32*)*raw));*raw += 4;
    for (uint i = 0; i < length; i++) {
		uint16 value = B_LENDIAN_TO_HOST_INT16(*((uint16*)*raw));
        list->insert(value);
        *raw += 2;
    }
}

void PTPDeviceInfo::Dump() {
	printf("Manufacturer/Model:   %s / %s\n",		
			Manufacturer.String(), Model.String());
	printf("Version / Serial#:    %s / %s\n",	
			DeviceVersion.String(), SerialNumber.String());
	printf("PTP Version / Mode:   %" B_PRId16 " / %" B_PRId16 "\n",		
			StandardVersion, FunctionalMode);
	printf("Extension ID/Version: %s - %" B_PRId32 " / %" B_PRId16 "\n", 
			VendorExtensionDesc.String(), VendorExtensionId, VendorExtensionVersion);

	int count = 0;
	printf("Image Formats:      ");
	for (set<uint16>::const_iterator i = ImageFormats.begin(); i !=ImageFormats.end(); ++i) {
		if (count >= 8) {
			printf("\n                       %4hx", *i);
			count = 1;
		} else {
			printf(" %4hx", *i);
			count++;
		}
	}
	count = 0;
	printf("\nCapture Formats:      ");
	for (set<uint16>::const_iterator i = CaptureFormats.begin(); i !=CaptureFormats.end(); ++i) {
		if (count >= 8) {
			printf("\n                       %4hx", *i);
			count = 0;
		} else {
			printf(" %4hx", *i);
			count++;
		}
	}
	count = 0;
	printf("\nOperations Supported: ");
	for (set<uint16>::const_iterator i = OperationsSupported.begin(); i !=OperationsSupported.end(); ++i) {
		if (count >= 8) {
			printf("\n                       %4hx", *i);
			count = 1;
		} else {
			printf(" %4hx", *i);
			count++;
		}
	}
	count = 0;
	printf("\nProperties Supported: ");
	for (set<uint16>::const_iterator i = DevicePropertiesSupported.begin(); i !=DevicePropertiesSupported.end(); ++i) {
		if (count >= 8) {
			printf("\n                       %4hx", *i);
			count = 1;
		} else {
			printf(" %4hx", *i);
			count++;
		}
	}
	count = 0;
	printf("\nEvents Supported:     ");
	for (set<uint16>::const_iterator i = EventsSupported.begin(); i !=EventsSupported.end(); ++i) {
		if (count >= 8) {
			printf("\n                       %4hx", *i);
			count = 1;
		} else {
			printf(" %4hx", *i);
			count++;
		}
	}
	printf("\n");
}


