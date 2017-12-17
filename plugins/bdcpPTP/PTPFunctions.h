/* PTPFunctions.h */

#ifndef PTPFUNCTIONS_H
#define PTPFUNCTIONS_H

using namespace std;

#include <TranslationUtils.h>
#include <USBKit.h>
#include <Path.h>
#include <List.h>
#include <TranslatorRoster.h>
#include <String.h>
#include <Locker.h>
#include <SupportDefs.h>
#include <vector>
#include <set>
#include <ByteOrder.h>
#include "PictureList.h"
#include "PTPDefinitions.h"

#define MSG_CAMERA_CONNECTED 'cCon'
#define MSG_CAMERA_DISCONNECTED 'cDsC' 

/* Macros and helpers */
#define CHECK_PTP_RC(result)    {if (result < B_OK) return result;}
#define PTP_CNT_INIT(cnt) 	{memset(&cnt,0,sizeof(cnt));}


enum CameraStatus {
    None,
    Busy,
    Identified,
    Ready
};

union PTPPropertyValue {
    char str[1]; /* malloced */
    uint8 u8;
    int8 i8;
    uint16 u16;
    int16 i16;
    uint32 u32;
    int32 i32;

    /* XXXX: 64bit and 128 bit signed and unsigned missing */
    struct array {
        uint32 count;
        union PTPPropertyValue *v; /* malloced, count elements */
    } a;
};

#define PTPUInt16(raw) B_LENDIAN_TO_HOST_INT16(*((uint16*)raw));raw+=2
#define PTPUInt32(raw) B_LENDIAN_TO_HOST_INT32(*((uint32*)raw));raw+=4

template <typename dataType>
class PTPController;

BString PTPString(char** raw);
void PTPArrayUInt16(vector<uint16>* list, char** raw);
void PTPSetUInt16(set<uint16>* list, char** raw);

class PTPDeviceInfo {
public:
				    			PTPDeviceInfo(); 
							    PTPDeviceInfo(char* raw);
    virtual 					~PTPDeviceInfo();
    uint16 						StandardVersion;
    uint32 						VendorExtensionId;
    uint16 						VendorExtensionVersion;
    BString 					VendorExtensionDesc;
    uint16 						FunctionalMode;
    
    set<uint16>		 			OperationsSupported;
    set<uint16>					EventsSupported;
    set<uint16>					DevicePropertiesSupported;
    set<uint16> 				CaptureFormats;
    set<uint16> 				ImageFormats;
    BString 					Manufacturer;
    BString 					Model;
    BString 					DeviceVersion;
    BString 					SerialNumber;    
    void 						SetTo(char* deviceInfo);
	void						Dump();
};

/* Property Describing Dataset, Range Form */

template <typename dataType>
struct PTPPropDescRangeForm {
    dataType 					MinimumValue;
    dataType 					MaximumValue;
    dataType 					StepSize;
}  __attribute__((packed));

/* Property Describing Dataset, Enum Form */

template <typename dataType>
struct PTPPropDescEnumForm {
    uint16 						numberOfValues;
    dataType 					supportedValue[1];
} __attribute__((packed));

/* Device Property Describing Dataset (DevicePropDesc) */

template <typename dataType>
struct PTPDevicePropDesc {
    uint16 						DevicePropertyCode;
    uint16 						DataType;
    uint8 						GetSet;
    dataType 					FactoryDefaultValue;
    dataType 					CurrentValue;
    uint8 						FormFlag;

    union {
        PTPPropDescEnumForm<dataType> 
        						Enum;
        PTPPropDescRangeForm<dataType> 
        						Range;
    } Form;
} __attribute__((packed));

class PTPCamera;
class PTPRoster;

typedef void (*UpdateCallBackFunc)(float Progress);
typedef status_t (*ConnectStateChangedCallBackFunc)(PTPCamera* camera);

class PTPCamera : BLocker {
    friend PTPRoster;
    template <typename dataType> 
    friend class PTPController;
public:
    							PTPCamera(BUSBDevice* device = NULL);
    virtual						~PTPCamera();
    status_t 					SetTo(BUSBDevice* device);
    PictureList* 				Pictures();

    CameraStatus 				Status();
    status_t 					Connect();
    status_t 					Disconnect();
    status_t 					UpdatePictures();
    status_t 					GetThumb(int num, BBitmap** bmp);
    status_t 					GetPicture(int num, BPath* savedir, 
    							  	UpdateCallBackFunc func = NULL);
    status_t 					DeletePicture(int num);
    status_t 					SetShooting(bool OnOff);
    status_t 					SetViewFinding(bool onoff);
    status_t 					GetViewFinderBitmap(BBitmap** bm);
    status_t 					TakePicture(BPath* path = NULL);
    bool     					HasProperty(uint16 DevicePropertyCode);
    PTPController<uint16>* 		GetController(uint16 DevicePropertyCode, type_code* type = NULL);
    status_t 					GetProperty(uint16 propcode, PTPPropertyValue* propvalue, uint16 datatype);
    status_t 					SetProperty(uint16 propcode, PTPPropertyValue* propvalue, uint16 datatype);
    const char* 				GetProduct() {return productName;}
    uint32   					VendorId() { return deviceInfo ? deviceInfo->VendorExtensionId : 0; }
    status_t 					UpdateDeviceInfo();
    bool 						isViewFinding;
    bool 						isShooting;
    PTPDeviceInfo* 				deviceInfo;

protected:
    void 						updateStatus(float bytesread);
    status_t 					OpenSession(uint32 NewSessionID);
    status_t 					CloseSession();
    status_t 					Transaction(PTPContainer* ptp, uint16 flags, uint32 length, char** data, 
    							  UpdateCallBackFunc func = NULL);
    status_t 					SendRequest(PTPContainer* ptp);
    status_t 					SendData(PTPContainer *ptp, char* data, uint32 length);
    status_t 					GetData(PTPContainer *ptp, char** data, UpdateCallBackFunc func = NULL);
    status_t 					GetResponse(PTPContainer *ptp);
    status_t 					CheckIntEvent(PTPUSBEventsContainer* event, bool wait);
    status_t 					CheckEvent(PTPUSBEventsContainer** event);
    status_t 					GetStorage();
    status_t 					GetObjects(uint32 storage);
    BUSBDevice* 				dev;
    PictureDesc* 				MakePictureDesc(PTPObjectInfoRaw* oi);
    const BUSBEndpoint* 		inep;
    const BUSBEndpoint* 		outep;
    const BUSBEndpoint* 		intep;
    CameraStatus 				fStatus;
    uint32 						SessionID;
    uint32 						TransactionID;
    PTPbyteorder 				byteorder;
    vector<uint32>				storage;
    BTranslatorRoster* 			JPEGTrans;
    BLocker 					usbintf;
    BString 					productName;
	PictureList					pictures;
	bool						picturesRetrieved;
};

class PTPRoster : public BUSBRoster {
public:
    							PTPRoster(ConnectStateChangedCallBackFunc connectStateChanged);
    							PTPRoster(BHandler* connectStateChanged);
    virtual status_t 			DeviceAdded(BUSBDevice *dev);
    virtual void 				DeviceRemoved(BUSBDevice *dev);
private:    
    ConnectStateChangedCallBackFunc 
    							connectStateChangedCallback;
    BHandler* 					connectStateChangedHandler;
    PTPCamera* 					camera;
};

template <typename dataType>
class PTPController {
    friend class PTPCamera;
public:
    							~PTPController() { delete fDesc; };
    status_t 					UpdateValue() {
								    PTPContainer ptp;
    								status_t status;
								    PTP_CNT_INIT(ptp);
								    ptp.Code = PTP_OC_GetDevicePropValue;
								    ptp.Nparam = 1;
								    ptp.Param1 = fDesc->DevicePropertyCode;
								    char* data = (char*)&fDesc->CurrentValue;
								    status = fCamera->Transaction(&ptp, PTP_DP_GETDATA, sizeof(dataType), &data); 
								    if (status >= B_OK) {
								        swap_data(fType, &fDesc->CurrentValue, sizeof(dataType), B_SWAP_LENDIAN_TO_HOST);
								    }
								    return status;
								};
    dataType 					Value(bool fetch=true) {
								    if (fetch) 
								        UpdateValue();
								    return fDesc->CurrentValue;
								};
    int32 						Index(bool fetch=true) {
								    if (fetch) 
								        UpdateValue();
								    if (fDesc->FormFlag == PTP_DPFF_Enumeration) {
								        for (int32 i = 0; i < fDesc->Form.Enum.numberOfValues; i++) {
								            if (fDesc->Form.Enum.supportedValue[i] == fDesc->CurrentValue) 
								                return i;
								        }
								    }
								    return 0;
								};
    status_t 					SetValue(dataType value) {
								    status_t status;
								    char* data = (char*)&value;
								    swap_data(fType, &value, sizeof(value), B_SWAP_HOST_TO_LENDIAN);
								    
								    PTPContainer ptp;
								    PTP_CNT_INIT(ptp);
								    ptp.Code = PTP_OC_SetDevicePropValue;
								    ptp.Nparam = 1;
								    ptp.Param1 = fDesc->DevicePropertyCode;
								    status = fCamera->Transaction(&ptp, PTP_DP_SENDDATA, sizeof(value), &data);
								    return status;
								};
    status_t 					SetIndex(int32 index) {
								    if (fDesc->FormFlag == PTP_DPFF_Enumeration &&
								        0 <= index && index < fDesc->Form.Enum.numberOfValues)
								        return SetValue(fDesc->Form.Enum.supportedValue[index]);
								    else
								        return B_ERROR;
								};
    uint16 						PTPDataType() { return fDesc->DataType; };
    bool 						IsEnum() { return fDesc->FormFlag == PTP_DPFF_Enumeration; };
    bool 						IsRange() {return fDesc->FormFlag == PTP_DPFF_Range; };
    bool 						IsBoolean() { 
									return fDesc->FormFlag == PTP_DPFF_Enumeration && 
								      fDesc->Form.Enum.numberOfValues == 2; 
								};
    dataType 					Min() { 
								    switch(fDesc->FormFlag) {
								        case PTP_DPFF_Range: 
								            return fDesc->Form.Range.MinimumValue; 
								        case PTP_DPFF_Enumeration:
								            return 0;
								        default:
								            return fDesc->FactoryDefaultValue;
								    }
								};
    dataType 					Max() { 
								    switch(fDesc->FormFlag) {
								        case PTP_DPFF_Range: 
								            return fDesc->Form.Range.MaximumValue; 
								        case PTP_DPFF_Enumeration:
								            return fDesc->Form.Enum.numberOfValues - 1;
								        default:
								            return fDesc->FactoryDefaultValue;
								    }
								};
    dataType 					Step() { return fDesc->Form.Range.StepSize; };
    int32 						CountValues() { return fDesc->Form.Enum.numberOfValues; };
    dataType 					SupportedValue(int32 i) { return fDesc->Form.Enum.supportedValue[i]; };
    bool 						IsSettable() { return (fDesc->GetSet == PTP_DPGS_GetSet); };
    status_t 					ReScan() {
								    PTPContainer ptp;
								    PTPDevicePropDesc<dataType>* tmpDesc = NULL;
								    PTP_CNT_INIT(ptp);
								    ptp.Code = PTP_OC_GetDevicePropDesc;
								    ptp.Nparam = 1;
								    ptp.Param1 = fDesc->DevicePropertyCode;
								    status_t status = fCamera->Transaction(&ptp, PTP_DP_GETDATA, 0, (char**)&tmpDesc);
								    if (status >= B_OK) {
								        delete fDesc;
								        fDesc = tmpDesc;
								    }
								    return B_OK;
								};
    uint16 						DevicePropertyCode() { return fDesc->DevicePropertyCode; };
    
protected:
    							PTPController(PTPCamera* camera, 
   								  PTPDevicePropDesc<dataType>* propDesc, type_code type) {
								    fCamera     = camera;
								    fDesc       = propDesc;
								    fType       = type;
								};
																	;
    PTPDevicePropDesc<dataType>* fDesc;
    type_code 					fType;
    PTPCamera* 					fCamera;
};


void StartPTP(ConnectStateChangedCallBackFunc connectStateChanged);
void StartPTP(BHandler* connectStateChanged);
void StopPTP();


#endif
