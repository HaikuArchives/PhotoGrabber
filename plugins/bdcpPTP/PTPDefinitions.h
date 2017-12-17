/*
* File:   PTPDefinitions.h
 * Author: Kai Niessen
 *
 * Created on December 22, 2014, 2:11 PM
 */

#ifndef PTPDEFINITIONS_H
#define	PTPDEFINITIONS_H

#define USB_DEVICE_CLASS_STILL_IMAGE 0x06
#define USB_DEVICE_SUBCLASS_IMAGE 0x00
#define USB_INTF_CLASS_IMAGE_CAPTURE 0x01

#define USB_VENDOR_ID_CANON		0x04a9
#define USB_VENDOR_ID_NIKON		0x04b0


/* Vendor IDs */
#define PTP_VENDOR_EASTMAN_KODAK	0x00000001
#define PTP_VENDOR_SEIKO_EPSON		0x00000002
#define PTP_VENDOR_AGILENT		0x00000003
#define PTP_VENDOR_POLAROID		0x00000004
#define PTP_VENDOR_AGFA_GEVAERT		0x00000005
#define PTP_VENDOR_MICROSOFT		0x00000006
#define PTP_VENDOR_EQUINOX		0x00000007
#define PTP_VENDOR_VIEWQUEST		0x00000008
#define PTP_VENDOR_STMICROELECTRONICS	0x00000009
#define PTP_VENDOR_NIKON		0x0000000A
#define PTP_VENDOR_CANON		0x0000000B

/* Operation Codes */

/* PTP v1.0 operation codes */
#define PTP_OC_Undefined                0x1000
#define PTP_OC_GetDeviceInfo            0x1001
#define PTP_OC_OpenSession              0x1002
#define PTP_OC_CloseSession             0x1003
#define PTP_OC_GetStorageIDs            0x1004
#define PTP_OC_GetStorageInfo           0x1005
#define PTP_OC_GetNumObjects            0x1006
#define PTP_OC_GetObjectHandles         0x1007
#define PTP_OC_GetObjectInfo            0x1008
#define PTP_OC_GetObject                0x1009
#define PTP_OC_GetThumb                 0x100A
#define PTP_OC_DeleteObject             0x100B
#define PTP_OC_SendObjectInfo           0x100C
#define PTP_OC_SendObject               0x100D
#define PTP_OC_InitiateCapture          0x100E
#define PTP_OC_FormatStore              0x100F
#define PTP_OC_ResetDevice              0x1010
#define PTP_OC_SelfTest                 0x1011
#define PTP_OC_SetObjectProtection      0x1012
#define PTP_OC_PowerDown                0x1013
#define PTP_OC_GetDevicePropDesc        0x1014
#define PTP_OC_GetDevicePropValue       0x1015
#define PTP_OC_SetDevicePropValue       0x1016
#define PTP_OC_ResetDevicePropValue     0x1017
#define PTP_OC_TerminateOpenCapture     0x1018
#define PTP_OC_MoveObject               0x1019
#define PTP_OC_CopyObject               0x101A
#define PTP_OC_GetPartialObject         0x101B
#define PTP_OC_InitiateOpenCapture      0x101C
/* PTP v1.1 operation codes */
#define PTP_OC_StartEnumHandles		0x101D
#define PTP_OC_EnumHandles		0x101E
#define PTP_OC_StopEnumHandles		0x101F
#define PTP_OC_GetVendorExtensionMaps	0x1020
#define PTP_OC_GetVendorDeviceInfo	0x1021
#define PTP_OC_GetResizedImageObject	0x1022
#define PTP_OC_GetFilesystemManifest	0x1023
#define PTP_OC_GetStreamInfo		0x1024
#define PTP_OC_GetStream		0x1025

/* Eastman Kodak extension Operation Codes */
#define PTP_OC_EK_GetSerial		0x9003
#define PTP_OC_EK_SetSerial		0x9004
#define PTP_OC_EK_SendFileObjectInfo	0x9005
#define PTP_OC_EK_SendFileObject	0x9006
#define PTP_OC_EK_SetText		0x9008

/* Canon extension Operation Codes */
#define PTP_OC_CANON_GetPartialObjectInfo	0x9001
/* 9002 - sends 2 uint32, nothing back  */
#define PTP_OC_CANON_SetObjectArchive		0x9002
#define PTP_OC_CANON_KeepDeviceOn		0x9003
#define PTP_OC_CANON_LockDeviceUI		0x9004
#define PTP_OC_CANON_UnlockDeviceUI		0x9005
#define PTP_OC_CANON_GetObjectHandleByName	0x9006
/* no 9007 observed yet */
#define PTP_OC_CANON_InitiateReleaseControl	0x9008
#define PTP_OC_CANON_TerminateReleaseControl	0x9009
#define PTP_OC_CANON_TerminatePlaybackMode	0x900A
#define PTP_OC_CANON_ViewfinderOn		0x900B
#define PTP_OC_CANON_ViewfinderOff		0x900C
#define PTP_OC_CANON_DoAeAfAwb			0x900D

/* 900e - send nothing, gets 5 uint16t in 32bit entities back in 20byte datablob */
#define PTP_OC_CANON_GetCustomizeSpec		0x900E
#define PTP_OC_CANON_GetCustomizeItemInfo	0x900F
#define PTP_OC_CANON_GetCustomizeData		0x9010
#define PTP_OC_CANON_SetCustomizeData		0x9011
#define PTP_OC_CANON_GetCaptureStatus		0x9012
#define PTP_OC_CANON_CheckEvent			0x9013
#define PTP_OC_CANON_FocusLock			0x9014
#define PTP_OC_CANON_FocusUnlock		0x9015
#define PTP_OC_CANON_GetLocalReleaseParam	0x9016
#define PTP_OC_CANON_SetLocalReleaseParam	0x9017
#define PTP_OC_CANON_AskAboutPcEvf		0x9018
#define PTP_OC_CANON_SendPartialObject		0x9019
#define PTP_OC_CANON_InitiateCaptureInMemory	0x901A
#define PTP_OC_CANON_GetPartialObjectEx		0x901B
#define PTP_OC_CANON_SetObjectTime		0x901C
#define PTP_OC_CANON_GetViewfinderImage		0x901D
#define PTP_OC_CANON_GetObjectAttributes	0x901E
#define PTP_OC_CANON_ChangeUSBProtocol		0x901F
#define PTP_OC_CANON_GetChanges			0x9020
#define PTP_OC_CANON_GetObjectInfoEx		0x9021
#define PTP_OC_CANON_InitiateDirectTransfer	0x9022
#define PTP_OC_CANON_TerminateDirectTransfer 	0x9023
#define PTP_OC_CANON_SendObjectInfoByPath 	0x9024
#define PTP_OC_CANON_SendObjectByPath 		0x9025
#define PTP_OC_CANON_InitiateDirectTansferEx	0x9026
#define PTP_OC_CANON_GetAncillaryObjectHandles	0x9027
#define PTP_OC_CANON_GetTreeInfo 		0x9028
#define PTP_OC_CANON_GetTreeSize 		0x9029
#define PTP_OC_CANON_NotifyProgress 		0x902A
#define PTP_OC_CANON_NotifyCancelAccepted	0x902B
/* 902c: no parms, read 3 uint32 in data, no response parms */
#define PTP_OC_CANON_902C			0x902C
#define PTP_OC_CANON_GetDirectory		0x902D

#define PTP_OC_CANON_SetPairingInfo		0x9030
#define PTP_OC_CANON_GetPairingInfo		0x9031
#define PTP_OC_CANON_DeletePairingInfo		0x9032
#define PTP_OC_CANON_GetMACAddress		0x9033
/* 9034: 1 param, no parms returned */
#define PTP_OC_CANON_SetDisplayMonitor		0x9034
#define PTP_OC_CANON_PairingComplete		0x9035
#define PTP_OC_CANON_GetWirelessMAXChannel	0x9036

#define PTP_OC_CANON_GetWebServiceSpec		0x9068
#define PTP_OC_CANON_GetWebServiceData		0x9069
#define PTP_OC_CANON_SetWebServiceData		0x906B
#define PTP_OC_CANON_GetRootCertificateSpec	0x906C
#define PTP_OC_CANON_GetRootCertificateData	0x906D
#define PTP_OC_CANON_SetRootCertificateData	0x906F

/* 9101: no args, 8 byte data (01 00 00 00 00 00 00 00), no resp data. */
#define PTP_OC_CANON_EOS_GetStorageIDs		0x9101
/* 9102: 1 arg (0)
 * 0x28 bytes of data:
    00000000: 34 00 00 00 02 00 02 91 0a 00 00 00 04 00 03 00
    00000010: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    00000020: 00 00 ff ff ff ff 03 43 00 46 00 00 00 03 41 00
    00000030: 3a 00 00 00
 * no resp args
 */
#define PTP_OC_CANON_EOS_GetStorageInfo		0x9102
#define PTP_OC_CANON_EOS_GetObjectInfo		0x9103
#define PTP_OC_CANON_EOS_GetObject		0x9104
#define PTP_OC_CANON_EOS_DeleteObject		0x9105
#define PTP_OC_CANON_EOS_FormatStore		0x9106
#define PTP_OC_CANON_EOS_GetPartialObject	0x9107
#define PTP_OC_CANON_EOS_GetDeviceInfoEx	0x9108

/* sample1:
 * 3 cmdargs: 1,0xffffffff,00 00 10 00;
 * data:
    00000000: 48 00 00 00 02 00 09 91 12 00 00 00 01 00 00 00
    00000010: 38 00 00 00 00 00 00 30 01 00 00 00 01 30 00 00
    00000020: 01 00 00 00 10 00 00 00 00 00 00 00 00 00 00 20
    00000030: 00 00 00 30 44 43 49 4d 00 00 00 00 00 00 00 00	DCIM
    00000040: 00 00 00 00 cc c3 01 46
 * 2 respargs: 0x0, 0x3c
 * 
 * sample2:
 *
    00000000: 18 00 00 00 01 00 09 91 15 00 00 00 01 00 00 00
    00000010: 00 00 00 30 00 00 10 00

    00000000: 48 00 00 00 02 00 09 91 15 00 00 00 01 00 00 00
    00000010: 38 00 00 00 00 00 9c 33 01 00 00 00 01 30 00 00
    00000020: 01 00 00 00 10 00 00 00 00 00 00 00 00 00 00 30
    00000030: 00 00 9c 33 32 33 31 43 41 4e 4f 4e 00 00 00 00	 231CANON
    00000040: 00 00 00 00 cc c3 01 46

 */
#define PTP_OC_CANON_EOS_GetObjectInfoEx	0x9109
#define PTP_OC_CANON_EOS_GetThumbEx		0x910A
#define PTP_OC_CANON_EOS_SendPartialObject	0x910B
#define PTP_OC_CANON_EOS_SetObjectAttributes	0x910C
#define PTP_OC_CANON_EOS_GetObjectTime		0x910D
#define PTP_OC_CANON_EOS_SetObjectTime		0x910E

/* 910f: no args, no data, 1 response arg (0). */
#define PTP_OC_CANON_EOS_RemoteRelease		0x910F
/* Marcus: looks more like "Set DeviceProperty" in the trace. 
 *
 * no cmd args
 * data phase (0xc, 0xd11c, 0x1)
 * no resp args 
 */
#define PTP_OC_CANON_EOS_SetDevicePropValueEx	0x9110
#define PTP_OC_CANON_EOS_GetRemoteMode		0x9113
/* 9114: 1 arg (0x1), no data, no resp data. */
#define PTP_OC_CANON_EOS_SetRemoteMode		0x9114
/* 9115: 1 arg (0x1), no data, no resp data. */
#define PTP_OC_CANON_EOS_SetEventMode		0x9115
/* 9116: no args, data phase, no resp data. */
#define PTP_OC_CANON_EOS_GetEvent		0x9116
#define PTP_OC_CANON_EOS_TransferComplete	0x9117
#define PTP_OC_CANON_EOS_CancelTransfer		0x9118
#define PTP_OC_CANON_EOS_ResetTransfer		0x9119

/* 911a: 3 args (0xfffffff7, 0x00001000, 0x00000001), no data, no resp data. */
/* 911a: 3 args (0x001dfc60, 0x00001000, 0x00000001), no data, no resp data. */
#define PTP_OC_CANON_EOS_PCHDDCapacity		0x911A

/* 911b: no cmd args, no data, no resp args */
#define PTP_OC_CANON_EOS_SetUILock		0x911B
/* 911c: no cmd args, no data, no resp args */
#define PTP_OC_CANON_EOS_ResetUILock		0x911C
#define PTP_OC_CANON_EOS_KeepDeviceOn		0x911D
#define PTP_OC_CANON_EOS_SetNullPacketMode	0x911E
#define PTP_OC_CANON_EOS_UpdateFirmware		0x911F
#define PTP_OC_CANON_EOS_TransferCompleteDT	0x9120
#define PTP_OC_CANON_EOS_CancelTransferDT	0x9121
#define PTP_OC_CANON_EOS_SetWftProfile		0x9122
#define PTP_OC_CANON_EOS_GetWftProfile		0x9123
#define PTP_OC_CANON_EOS_SetProfileToWft	0x9124
#define PTP_OC_CANON_EOS_BulbStart		0x9125
#define PTP_OC_CANON_EOS_BulbEnd		0x9126
#define PTP_OC_CANON_EOS_RequestDevicePropValue	0x9127

/* 0x9128 args (0x1/0x2, 0x0), no data, no resp args */
#define PTP_OC_CANON_EOS_RemoteReleaseOn	0x9128
/* 0x9129 args (0x1/0x2), no data, no resp args */
#define PTP_OC_CANON_EOS_RemoteReleaseOff	0x9129

#define PTP_OC_CANON_EOS_RegistBackgroundImage	0x912A
#define PTP_OC_CANON_EOS_ChangePhotoStudioMode	0x912B
#define PTP_OC_CANON_EOS_GetPartialObjectEx	0x912C
#define PTP_OC_CANON_EOS_ResetMirrorLockupState	0x9130
#define PTP_OC_CANON_EOS_PopupBuiltinFlash	0x9131
#define PTP_OC_CANON_EOS_EndGetPartialObjectEx	0x9132
#define PTP_OC_CANON_EOS_MovieSelectSWOn	0x9133
#define PTP_OC_CANON_EOS_MovieSelectSWOff	0x9134
#define PTP_OC_CANON_EOS_GetCTGInfo		0x9135
#define PTP_OC_CANON_EOS_GetLensAdjust		0x9136
#define PTP_OC_CANON_EOS_SetLensAdjust		0x9137
#define PTP_OC_CANON_EOS_GetMusicInfo		0x9138
#define PTP_OC_CANON_EOS_CreateHandle		0x9139
#define PTP_OC_CANON_EOS_SendPartialObjectEx	0x913A
#define PTP_OC_CANON_EOS_EndSendPartialObjectEx	0x913B
#define PTP_OC_CANON_EOS_SetCTGInfo		0x913C
#define PTP_OC_CANON_EOS_SetRequestOLCInfoGroup	0x913D
#define PTP_OC_CANON_EOS_SetRequestRollingPitchingLevel	0x913E
#define PTP_OC_CANON_EOS_GetCameraSupport	0x913F
#define PTP_OC_CANON_EOS_SetRating		0x9140 /* 2 args */
#define PTP_OC_CANON_EOS_RequestInnerDevelopStart	0x9141 /* 2 args: 1 type, 1 object? */
#define PTP_OC_CANON_EOS_RequestInnerDevelopParamChange	0x9142
#define PTP_OC_CANON_EOS_RequestInnerDevelopEnd		0x9143
#define PTP_OC_CANON_EOS_GpsLoggingDataMode		0x9144 /* 1 arg */
#define PTP_OC_CANON_EOS_GetGpsLogCurrentHandle		0x9145

#define PTP_OC_CANON_EOS_InitiateViewfinder	0x9151
#define PTP_OC_CANON_EOS_TerminateViewfinder	0x9152
#define PTP_OC_CANON_EOS_GetViewFinderData	0x9153
#define PTP_OC_CANON_EOS_DoAf			0x9154
#define PTP_OC_CANON_EOS_DriveLens		0x9155
#define PTP_OC_CANON_EOS_DepthOfFieldPreview	0x9156 /* 1 arg */
#define PTP_OC_CANON_EOS_ClickWB		0x9157 /* 2 args: x,y */
#define PTP_OC_CANON_EOS_Zoom			0x9158 /* 1 arg */
#define PTP_OC_CANON_EOS_ZoomPosition		0x9159 /* 2 args: x,y */
#define PTP_OC_CANON_EOS_SetLiveAfFrame		0x915A
#define PTP_OC_CANON_EOS_TouchAfPosition	0x915B /* 3 args: type,x,y */
#define PTP_OC_CANON_EOS_SetLvPcFlavoreditMode	0x915C /* 1 arg */
#define PTP_OC_CANON_EOS_SetLvPcFlavoreditParam	0x915D /* 1 arg */
#define PTP_OC_CANON_EOS_AfCancel		0x9160
#define PTP_OC_CANON_EOS_SetDefaultCameraSetting		0x91BE
#define PTP_OC_CANON_EOS_GetAEData		0x91BF
#define PTP_OC_CANON_EOS_NotifyNetworkError	0x91E8
#define PTP_OC_CANON_EOS_AdapterTransferProgress	0x91E9
#define PTP_OC_CANON_EOS_TransferComplete2	0x91F0
#define PTP_OC_CANON_EOS_CancelTransfer2	0x91F1
#define PTP_OC_CANON_EOS_FAPIMessageTX		0x91FE
#define PTP_OC_CANON_EOS_FAPIMessageRX		0x91FF

/* Nikon extension Operation Codes */
#define PTP_OC_NIKON_GetProfileAllData	0x9006
#define PTP_OC_NIKON_SendProfileData	0x9007
#define PTP_OC_NIKON_DeleteProfile	0x9008
#define PTP_OC_NIKON_SetProfileData	0x9009
#define PTP_OC_NIKON_AdvancedTransfer	0x9010
#define PTP_OC_NIKON_GetFileInfoInBlock	0x9011
#define PTP_OC_NIKON_Capture		0x90C0	/* 1 param,   no data */
#define PTP_OC_NIKON_AfDrive		0x90C1	/* no params, no data */
#define PTP_OC_NIKON_SetControlMode	0x90C2	/* 1 param,  no data */
#define PTP_OC_NIKON_DelImageSDRAM	0x90C3	/* 1 param,  no data */
#define PTP_OC_NIKON_GetLargeThumb	0x90C4
#define PTP_OC_NIKON_CurveDownload	0x90C5	/* 1 param,   data in */
#define PTP_OC_NIKON_CurveUpload	0x90C6	/* 1 param,   data out */
#define PTP_OC_NIKON_CheckEvent		0x90C7	/* no params, data in */
#define PTP_OC_NIKON_DeviceReady	0x90C8	/* no params, no data */
#define PTP_OC_NIKON_SetPreWBData	0x90C9	/* 3 params,  data out */
#define PTP_OC_NIKON_GetVendorPropCodes	0x90CA	/* 0 params, data in */
#define PTP_OC_NIKON_AfCaptureSDRAM	0x90CB	/* no params, no data */
#define PTP_OC_NIKON_GetPictCtrlData	0x90CC	/* 2 params, data in */
#define PTP_OC_NIKON_SetPictCtrlData	0x90CD	/* 2 params, data out */
#define PTP_OC_NIKON_DelCstPicCtrl	0x90CE	/* 1 param, no data */
#define PTP_OC_NIKON_GetPicCtrlCapability	0x90CF	/* 1 param, data in */

/* Nikon Liveview stuff */
#define PTP_OC_NIKON_GetPreviewImg	0x9200
#define PTP_OC_NIKON_StartLiveView	0x9201	/* no params */
#define PTP_OC_NIKON_EndLiveView	0x9202	/* no params */
#define PTP_OC_NIKON_GetLiveViewImg	0x9203	/* no params, data in */
#define PTP_OC_NIKON_MfDrive		0x9204	/* 2 params */
#define PTP_OC_NIKON_ChangeAfArea	0x9205	/* 2 params */
#define PTP_OC_NIKON_AfDriveCancel	0x9206	/* no params */
/* 2 params:
 * 0xffffffff == No AF before,  0xfffffffe == AF before capture
 * sdram=1, card=0
 */
#define PTP_OC_NIKON_InitiateCaptureRecInMedia	0x9207	/* 1 params */

#define PTP_OC_NIKON_GetVendorStorageIDs	0x9209	/* no params, data in */

#define PTP_OC_NIKON_StartMovieRecInCard	0x920a	/* no params, no data */
#define PTP_OC_NIKON_EndMovieRec		0x920b	/* no params, no data */

#define PTP_OC_NIKON_TerminateCapture		0x920c	/* 2 params */

#define PTP_OC_NIKON_GetDevicePTPIPInfo	0x90E0

#define PTP_OC_NIKON_GetPartialObjectHiSpeed	0x9400	/* 3 params, data in */

/* From Nikon V1 Trace */
#define PTP_OC_NIKON_GetDevicePropEx		0x9504	/* gets device prop dataa */

/* Casio EX-F1 (from http://code.google.com/p/exf1ctrl/ ) */
#define PTP_OC_CASIO_STILL_START	0x9001
#define PTP_OC_CASIO_STILL_STOP		0x9002

#define PTP_OC_CASIO_FOCUS		0x9007
#define PTP_OC_CASIO_CF_PRESS		0x9009
#define PTP_OC_CASIO_CF_RELEASE		0x900A
#define PTP_OC_CASIO_GET_OBJECT_INFO	0x900C

#define PTP_OC_CASIO_SHUTTER		0x9024
#define PTP_OC_CASIO_GET_STILL_HANDLES	0x9027
#define PTP_OC_CASIO_STILL_RESET	0x9028
#define PTP_OC_CASIO_HALF_PRESS		0x9029
#define PTP_OC_CASIO_HALF_RELEASE	0x902A
#define PTP_OC_CASIO_CS_PRESS		0x902B
#define PTP_OC_CASIO_CS_RELEASE		0x902C

#define PTP_OC_CASIO_ZOOM		0x902D
#define PTP_OC_CASIO_CZ_PRESS		0x902E
#define PTP_OC_CASIO_CZ_RELEASE		0x902F

#define PTP_OC_CASIO_MOVIE_START	0x9041
#define PTP_OC_CASIO_MOVIE_STOP		0x9042
#define PTP_OC_CASIO_MOVIE_PRESS	0x9043
#define PTP_OC_CASIO_MOVIE_RELEASE	0x9044
#define PTP_OC_CASIO_GET_MOVIE_HANDLES	0x9045
#define PTP_OC_CASIO_MOVIE_RESET	0x9046

#define PTP_OC_CASIO_GET_OBJECT		0x9025
#define PTP_OC_CASIO_GET_THUMBNAIL	0x9026

/* Sony stuff */
/* 9201:
 *  3 params: 1,0,0 ; IN: data 8 bytes all 0
 * or:
 *  3 params: 2,0,0 ; IN: data 8 bytes all 0 
 * or
 *  3 params: 3,0,0,: IN: data 8 butes all 0
 */
#define PTP_OC_SONY_SDIOConnect			0x9201
/* 9202: 1 param, 0xc8; IN data: 
 * 16 bit: 0xc8
 * ptp array 32 bit: index, 16 bit values of propcodes  */
#define PTP_OC_SONY_GetSDIOGetExtDeviceInfo	0x9202

#define PTP_OC_SONY_GetDevicePropdesc		0x9203
#define PTP_OC_SONY_GetDevicePropertyValue	0x9204
/* 1 param, 16bit propcode, SEND DATA: propvalue */
#define PTP_OC_SONY_SetControlDeviceA		0x9205
#define PTP_OC_SONY_GetControlDeviceDesc	0x9206
/* 1 param, 16bit propcode, SEND DATA: propvalue */
#define PTP_OC_SONY_SetControlDeviceB		0x9207
/* get all device property data at once */
#define PTP_OC_SONY_GetAllDevicePropData	0x9209	/* gets a 4126 byte blob of device props ?*/

/* Microsoft / MTP extension codes */

#define PTP_OC_MTP_GetObjectPropsSupported	0x9801
#define PTP_OC_MTP_GetObjectPropDesc		0x9802
#define PTP_OC_MTP_GetObjectPropValue		0x9803
#define PTP_OC_MTP_SetObjectPropValue		0x9804
#define PTP_OC_MTP_GetObjPropList		0x9805
#define PTP_OC_MTP_SetObjPropList		0x9806
#define PTP_OC_MTP_GetInterdependendPropdesc	0x9807
#define PTP_OC_MTP_SendObjectPropList		0x9808
#define PTP_OC_MTP_GetObjectReferences		0x9810
#define PTP_OC_MTP_SetObjectReferences		0x9811
#define PTP_OC_MTP_UpdateDeviceFirmware		0x9812
#define PTP_OC_MTP_Skip				0x9820

/*
 * Windows Media Digital Rights Management for Portable Devices 
 * Extension Codes (microsoft.com/WMDRMPD: 10.1)
 */
#define PTP_OC_MTP_WMDRMPD_GetSecureTimeChallenge	0x9101
#define PTP_OC_MTP_WMDRMPD_GetSecureTimeResponse	0x9102
#define PTP_OC_MTP_WMDRMPD_SetLicenseResponse	0x9103
#define PTP_OC_MTP_WMDRMPD_GetSyncList		0x9104
#define PTP_OC_MTP_WMDRMPD_SendMeterChallengeQuery	0x9105
#define PTP_OC_MTP_WMDRMPD_GetMeterChallenge	0x9106
#define PTP_OC_MTP_WMDRMPD_SetMeterResponse		0x9107
#define PTP_OC_MTP_WMDRMPD_CleanDataStore		0x9108
#define PTP_OC_MTP_WMDRMPD_GetLicenseState		0x9109
#define PTP_OC_MTP_WMDRMPD_SendWMDRMPDCommand	0x910A
#define PTP_OC_MTP_WMDRMPD_SendWMDRMPDRequest	0x910B

/* 
 * Windows Media Digital Rights Management for Portable Devices 
 * Extension Codes (microsoft.com/WMDRMPD: 10.1)
 * Below are operations that have no public documented identifier 
 * associated with them "Vendor-defined Command Code"
 */
#define PTP_OC_MTP_WMDRMPD_SendWMDRMPDAppRequest	0x9212
#define PTP_OC_MTP_WMDRMPD_GetWMDRMPDAppResponse	0x9213
#define PTP_OC_MTP_WMDRMPD_EnableTrustedFilesOperations	0x9214
#define PTP_OC_MTP_WMDRMPD_DisableTrustedFilesOperations 0x9215
#define PTP_OC_MTP_WMDRMPD_EndTrustedAppSession		0x9216
/* ^^^ guess ^^^ */

/*
 * Microsoft Advanced Audio/Video Transfer 
 * Extensions (microsoft.com/AAVT: 1.0)
 */
#define PTP_OC_MTP_AAVT_OpenMediaSession		0x9170
#define PTP_OC_MTP_AAVT_CloseMediaSession		0x9171
#define PTP_OC_MTP_AAVT_GetNextDataBlock		0x9172
#define PTP_OC_MTP_AAVT_SetCurrentTimePosition		0x9173

/*
 * Windows Media Digital Rights Management for Network Devices 
 * Extensions (microsoft.com/WMDRMND: 1.0) MTP/IP?
 */
#define PTP_OC_MTP_WMDRMND_SendRegistrationRequest	0x9180
#define PTP_OC_MTP_WMDRMND_GetRegistrationResponse	0x9181
#define PTP_OC_MTP_WMDRMND_GetProximityChallenge	0x9182
#define PTP_OC_MTP_WMDRMND_SendProximityResponse	0x9183
#define PTP_OC_MTP_WMDRMND_SendWMDRMNDLicenseRequest	0x9184
#define PTP_OC_MTP_WMDRMND_GetWMDRMNDLicenseResponse	0x9185

/* 
 * Windows Media Player Portiable Devices 
 * Extension Codes (microsoft.com/WMPPD: 11.1)
 */
#define PTP_OC_MTP_WMPPD_ReportAddedDeletedItems	0x9201
#define PTP_OC_MTP_WMPPD_ReportAcquiredItems 	        0x9202
#define PTP_OC_MTP_WMPPD_PlaylistObjectPref		0x9203

/*
 * Undocumented Zune Operation Codes 
 * maybe related to WMPPD extension set?
 */
#define PTP_OC_MTP_ZUNE_GETUNDEFINED001		        0x9204

/* WiFi Provisioning MTP Extension Codes (microsoft.com/WPDWCN: 1.0) */
#define PTP_OC_MTP_WPDWCN_ProcessWFCObject		0x9122


/* Olympus E series commands */
#define PTP_OC_OLYMPUS_Capture				0x9101
#define PTP_OC_OLYMPUS_SelfCleaning			0x9103
#define PTP_OC_OLYMPUS_SetRGBGain			0x9106
#define PTP_OC_OLYMPUS_SetPresetMode			0x9107
#define PTP_OC_OLYMPUS_SetWBBiasAll			0x9108
#define PTP_OC_OLYMPUS_GetCameraControlMode		0x910a
#define PTP_OC_OLYMPUS_SetCameraControlMode		0x910b
#define PTP_OC_OLYMPUS_SetWBRGBGain			0x910c
#define PTP_OC_OLYMPUS_GetDeviceInfo			0x9301
#define PTP_OC_OLYMPUS_OpenSession			0x9302
#define PTP_OC_OLYMPUS_SetDateTime			0x9402
#define PTP_OC_OLYMPUS_GetDateTime			0x9482
#define PTP_OC_OLYMPUS_SetCameraID			0x9501
#define PTP_OC_OLYMPUS_GetCameraID			0x9581

/* Android Random I/O Extensions Codes */
#define PTP_OC_ANDROID_GetPartialObject64		0x95C1
#define PTP_OC_ANDROID_SendPartialObject		0x95C2
#define PTP_OC_ANDROID_TruncateObject			0x95C3
#define PTP_OC_ANDROID_BeginEditObject			0x95C4
#define PTP_OC_ANDROID_EndEditObject			0x95C5

/* Proprietary vendor extension operations mask */
#define PTP_OC_EXTENSION_MASK           0xF000
#define PTP_OC_EXTENSION                0x9000

/* Response Codes */

#define PTP_RC_Undefined                0x2000
#define PTP_RC_OK                       0x2001
#define PTP_RC_GeneralError             0x2002
#define PTP_RC_SessionNotOpen           0x2003
#define PTP_RC_InvalidTransactionID	0x2004
#define PTP_RC_OperationNotSupported    0x2005
#define PTP_RC_ParameterNotSupported    0x2006
#define PTP_RC_IncompleteTransfer       0x2007
#define PTP_RC_InvalidStorageId         0x2008
#define PTP_RC_InvalidObjectHandle      0x2009
#define PTP_RC_DevicePropNotSupported   0x200A
#define PTP_RC_InvalidObjectFormatCode  0x200B
#define PTP_RC_StoreFull                0x200C
#define PTP_RC_ObjectWriteProtected     0x200D
#define PTP_RC_StoreReadOnly            0x200E
#define PTP_RC_AccessDenied             0x200F
#define PTP_RC_NoThumbnailPresent       0x2010
#define PTP_RC_SelfTestFailed           0x2011
#define PTP_RC_PartialDeletion          0x2012
#define PTP_RC_StoreNotAvailable        0x2013
#define PTP_RC_SpecificationByFormatUnsupported         0x2014
#define PTP_RC_NoValidObjectInfo        0x2015
#define PTP_RC_InvalidCodeFormat        0x2016
#define PTP_RC_UnknownVendorCode        0x2017
#define PTP_RC_CaptureAlreadyTerminated 0x2018
#define PTP_RC_DeviceBusy               0x2019
#define PTP_RC_InvalidParentObject      0x201A
#define PTP_RC_InvalidDevicePropFormat  0x201B
#define PTP_RC_InvalidDevicePropValue   0x201C
#define PTP_RC_InvalidParameter         0x201D
#define PTP_RC_SessionAlreadyOpened     0x201E
#define PTP_RC_TransactionCanceled      0x201F
#define PTP_RC_SpecificationOfDestinationUnsupported            0x2020
/* Eastman Kodak extension Response Codes */
#define PTP_RC_EK_FilenameRequired	0xA001
#define PTP_RC_EK_FilenameConflicts	0xA002
#define PTP_RC_EK_FilenameInvalid	0xA003

/* Proprietary vendor extension response code mask */
#define PTP_RC_EXTENSION_MASK		0xF000
#define PTP_RC_EXTENSION		0xA000

/* libptp2 extended ERROR codes */
#define PTP_ERROR_IO			0x02FF
#define PTP_ERROR_DATA_EXPECTED		0x02FE
#define PTP_ERROR_RESP_EXPECTED		0x02FD
#define PTP_ERROR_BADPARAM		0x02FC

/* PTP Event Codes */

#define PTP_EC_Undefined		0x4000
#define PTP_EC_CancelTransaction	0x4001
#define PTP_EC_ObjectAdded		0x4002
#define PTP_EC_ObjectRemoved		0x4003
#define PTP_EC_StoreAdded		0x4004
#define PTP_EC_StoreRemoved		0x4005
#define PTP_EC_DevicePropChanged	0x4006
#define PTP_EC_ObjectInfoChanged	0x4007
#define PTP_EC_DeviceInfoChanged	0x4008
#define PTP_EC_RequestObjectTransfer	0x4009
#define PTP_EC_StoreFull		0x400A
#define PTP_EC_DeviceReset		0x400B
#define PTP_EC_StorageInfoChanged	0x400C
#define PTP_EC_CaptureComplete		0x400D
#define PTP_EC_UnreportedStatus		0x400E
/* Canon extension Event Codes */
#define PTP_EC_CANON_DeviceInfoChanged	0xC008
#define PTP_EC_CANON_RequestObjectTransfer	0xC009
#define PTP_EC_CANON_CameraModeChanged	0xC00C
/* Nikon extension Event Codes */
#define PTP_EC_Nikon_ObjectAddedInSDRAM		0xC101
#define PTP_EC_Nikon_CaptureCompleteRecInSdram	0xC102 // Gets 1 parameter, objectid pointing to DPOF object
#define PTP_EC_Nikon_AdvancedTransfer		0xC103
#define PTP_EC_Nikon_PreviewImageAdded		0xC104


/* max ptp string length INCLUDING terminating null character */

#define PTP_MAXSTRLEN				255

/* PTP Object Format Codes */

/* ancillary formats */
#define PTP_OFC_Undefined			0x3000
#define PTP_OFC_Association			0x3001
#define PTP_OFC_Script				0x3002
#define PTP_OFC_Executable			0x3003
#define PTP_OFC_Text				0x3004
#define PTP_OFC_HTML				0x3005
#define PTP_OFC_DPOF				0x3006
#define PTP_OFC_AIFF	 			0x3007
#define PTP_OFC_WAV				0x3008
#define PTP_OFC_MP3				0x3009
#define PTP_OFC_AVI				0x300A
#define PTP_OFC_MPEG				0x300B
#define PTP_OFC_ASF				0x300C
#define PTP_OFC_QT				0x300D /* guessing */
/* image formats */
#define PTP_OFC_EXIF_JPEG			0x3801
#define PTP_OFC_TIFF_EP				0x3802
#define PTP_OFC_FlashPix			0x3803
#define PTP_OFC_BMP				0x3804
#define PTP_OFC_CIFF				0x3805
#define PTP_OFC_Undefined_0x3806		0x3806
#define PTP_OFC_GIF				0x3807
#define PTP_OFC_JFIF				0x3808
#define PTP_OFC_PCD				0x3809
#define PTP_OFC_PICT				0x380A
#define PTP_OFC_PNG				0x380B
#define PTP_OFC_Undefined_0x380C		0x380C
#define PTP_OFC_TIFF				0x380D
#define PTP_OFC_TIFF_IT				0x380E
#define PTP_OFC_JP2				0x380F
#define PTP_OFC_JPX				0x3810
/* Eastman Kodak extension ancillary format */
#define PTP_OFC_EK_M3U				0xB002
#define PTP_OFC_CANON_RAW			0xB101


/* PTP Association Types */

#define PTP_AT_Undefined			0x0000
#define PTP_AT_GenericFolder			0x0001
#define PTP_AT_Album				0x0002
#define PTP_AT_TimeSequence			0x0003
#define PTP_AT_HorizontalPanoramic		0x0004
#define PTP_AT_VerticalPanoramic		0x0005
#define PTP_AT_2DPanoramic			0x0006
#define PTP_AT_AncillaryData			0x0007

/* PTP Protection Status */

#define PTP_PS_NoProtection			0x0000
#define PTP_PS_ReadOnly				0x0001

/* PTP Storage Types */

#define PTP_ST_Undefined			0x0000
#define PTP_ST_FixedROM				0x0001
#define PTP_ST_RemovableROM			0x0002
#define PTP_ST_FixedRAM				0x0003
#define PTP_ST_RemovableRAM			0x0004

/* PTP FilesystemType Values */

#define PTP_FST_Undefined			0x0000
#define PTP_FST_GenericFlat			0x0001
#define PTP_FST_GenericHierarchical		0x0002
#define PTP_FST_DCF				0x0003

/* PTP StorageInfo AccessCapability Values */

#define PTP_AC_ReadWrite			0x0000
#define PTP_AC_ReadOnly				0x0001
#define PTP_AC_ReadOnly_with_Object_Deletion	0x0002

#define USB_TIMEOUT					10000
#define USB_CAPTURE_TIMEOUT			20000000

/* PTP datalayer byteorder */
enum PTPbyteorder {
    PTP_DL_BE = 0xF0,
    PTP_DL_LE = 0x0F
};
/* Canon filesystem's folder entry Dataset */

#define PTP_CANON_FilenameBufferLen	13
#define PTP_CANON_FolderEntryLen	sizeof(PTPCANONFolderEntry)

struct PTPCANONFolderEntry {
    uint32 ObjectHandle;
    uint16 ObjectFormatCode;
    uint8 Flags;
    uint32 ObjectSize;
    time_t Time;
    char Filename[PTP_CANON_FilenameBufferLen];
};

/* DataType Codes */

#define PTP_DTC_UNDEF		0x0000
#define PTP_DTC_INT8		0x0001
#define PTP_DTC_UINT8		0x0002
#define PTP_DTC_INT16		0x0003
#define PTP_DTC_UINT16		0x0004
#define PTP_DTC_INT32		0x0005
#define PTP_DTC_UINT32		0x0006
#define PTP_DTC_INT64		0x0007
#define PTP_DTC_UINT64		0x0008
#define PTP_DTC_INT128		0x0009
#define PTP_DTC_UINT128		0x000A
#define PTP_DTC_AINT8		0x4001
#define PTP_DTC_AUINT8		0x4002
#define PTP_DTC_AINT16		0x4003
#define PTP_DTC_AUINT16		0x4004
#define PTP_DTC_AINT32		0x4005
#define PTP_DTC_AUINT32		0x4006
#define PTP_DTC_AINT64		0x4007
#define PTP_DTC_AUINT64		0x4008
#define PTP_DTC_AINT128		0x4009
#define PTP_DTC_AUINT128	0x400A
#define PTP_DTC_STR		0xFFFF

/* Device Properties Codes */

/* PTP v1.0 property codes */
#define PTP_DPC_Undefined			0x5000
#define PTP_DPC_BatteryLevel		0x5001
#define PTP_DPC_FunctionalMode		0x5002
#define PTP_DPC_ImageSize			0x5003
#define PTP_DPC_CompressionSetting	0x5004
#define PTP_DPC_WhiteBalance		0x5005
#define PTP_DPC_RGBGain				0x5006
#define PTP_DPC_FNumber				0x5007
#define PTP_DPC_FocalLength			0x5008
#define PTP_DPC_FocusDistance		0x5009
#define PTP_DPC_FocusMode			0x500A
#define PTP_DPC_ExposureMeteringMode 0x500B
#define PTP_DPC_FlashMode			0x500C
#define PTP_DPC_ExposureTime		0x500D
#define PTP_DPC_ExposureProgramMode	0x500E
#define PTP_DPC_ExposureIndex		0x500F
#define PTP_DPC_ExposureBiasCompensation	0x5010
#define PTP_DPC_DateTime			0x5011
#define PTP_DPC_CaptureDelay		0x5012
#define PTP_DPC_StillCaptureMode	0x5013
#define PTP_DPC_Contrast			0x5014
#define PTP_DPC_Sharpness			0x5015
#define PTP_DPC_DigitalZoom			0x5016
#define PTP_DPC_EffectMode			0x5017
#define PTP_DPC_BurstNumber			0x5018
#define PTP_DPC_BurstInterval		0x5019
#define PTP_DPC_TimelapseNumber		0x501A
#define PTP_DPC_TimelapseInterval	0x501B
#define PTP_DPC_FocusMeteringMode	0x501C
#define PTP_DPC_UploadURL			0x501D
#define PTP_DPC_Artist				0x501E
#define PTP_DPC_CopyrightInfo		0x501F
/* PTP v1.1 property codes */
#define PTP_DPC_SupportedStreams	0x5020
#define PTP_DPC_EnabledStreams		0x5021
#define PTP_DPC_VideoFormat			0x5022
#define PTP_DPC_VideoResolution		0x5023
#define PTP_DPC_VideoQuality		0x5024
#define PTP_DPC_VideoFrameRate		0x5025
#define PTP_DPC_VideoContrast		0x5026
#define PTP_DPC_VideoBrightness		0x5027
#define PTP_DPC_AudioFormat			0x5028
#define PTP_DPC_AudioBitrate		0x5029
#define PTP_DPC_AudioSamplingRate	0x502A
#define PTP_DPC_AudioBitPerSample	0x502B
#define PTP_DPC_AudioVolume			0x502C

/* Proprietary vendor extension device property mask */
#define PTP_DPC_EXTENSION_MASK		0xF000
#define PTP_DPC_EXTENSION			0xD000

/* Zune extension device property codes */
#define PTP_DPC_MTP_ZUNE_UNKNOWN1	0xD181
#define PTP_DPC_MTP_ZUNE_UNKNOWN2	0xD132
#define PTP_DPC_MTP_ZUNE_UNKNOWN3	0xD215
#define PTP_DPC_MTP_ZUNE_UNKNOWN4	0xD216

/* Eastman Kodak extension device property codes */
#define PTP_DPC_EK_ColorTemperature	0xD001
#define PTP_DPC_EK_DateTimeStampFormat	0xD002
#define PTP_DPC_EK_BeepMode			0xD003
#define PTP_DPC_EK_VideoOut			0xD004
#define PTP_DPC_EK_PowerSaving		0xD005
#define PTP_DPC_EK_UI_Language		0xD006

/* Canon extension device property codes */
#define PTP_DPC_CANON_BeepMode		0xD001
#define PTP_DPC_CANON_BatteryKind	0xD002
#define PTP_DPC_CANON_BatteryStatus	0xD003
#define PTP_DPC_CANON_UILockType	0xD004
#define PTP_DPC_CANON_CameraMode	0xD005
#define PTP_DPC_CANON_ImageQuality	0xD006
#define PTP_DPC_CANON_FullViewFileFormat 0xD007
#define PTP_DPC_CANON_ImageSize		0xD008
#define PTP_DPC_CANON_SelfTime		0xD009
#define PTP_DPC_CANON_FlashMode		0xD00A
#define PTP_DPC_CANON_Beep			0xD00B
#define PTP_DPC_CANON_ShootingMode	0xD00C
#define PTP_DPC_CANON_ImageMode		0xD00D
#define PTP_DPC_CANON_DriveMode		0xD00E
#define PTP_DPC_CANON_EZoom			0xD00F
#define PTP_DPC_CANON_MeteringMode	0xD010
#define PTP_DPC_CANON_AFDistance	0xD011
#define PTP_DPC_CANON_MacroMode     0xD011
#define PTP_DPC_CANON_FocusingPoint	0xD012
#define PTP_DPC_CANON_WhiteBalance	0xD013
#define PTP_DPC_CANON_SlowShutterSetting 0xD014
#define PTP_DPC_CANON_AFMode		0xD015
#define PTP_DPC_CANON_ImageStabilization 0xD016
#define PTP_DPC_CANON_Contrast		0xD017
#define PTP_DPC_CANON_ColorGain		0xD018
#define PTP_DPC_CANON_Sharpness		0xD019
#define PTP_DPC_CANON_Sensitivity	0xD01A
#define PTP_DPC_CANON_ParameterSet	0xD01B
#define PTP_DPC_CANON_ISOSpeed		0xD01C
#define PTP_DPC_CANON_Aperture		0xD01D
#define PTP_DPC_CANON_ShutterSpeed	0xD01E
#define PTP_DPC_CANON_ExpCompensation	0xD01F
#define PTP_DPC_CANON_FlashCompensation	0xD020
#define PTP_DPC_CANON_AEBExposureCompensation 0xD021
#define PTP_DPC_CANON_AvOpen		0xD023
#define PTP_DPC_CANON_AvMax			0xD024
#define PTP_DPC_CANON_FocalLength	0xD025
#define PTP_DPC_CANON_FocalLengthTele	0xD026
#define PTP_DPC_CANON_FocalLengthWide	0xD027
#define PTP_DPC_CANON_FocalLengthDenominator	0xD028
#define PTP_DPC_CANON_CaptureTransferMode	0xD029
#define CANON_TRANSFER_ENTIRE_IMAGE_TO_PC	0x0002
#define CANON_TRANSFER_SAVE_THUMBNAIL_TO_DEVICE	0x0004
#define CANON_TRANSFER_SAVE_IMAGE_TO_DEVICE	0x0008
/* we use those values: */
#define CANON_TRANSFER_MEMORY		(2|1)
#define CANON_TRANSFER_CARD			(8|4|1)

#define PTP_DPC_CANON_Zoom			0xD02A
#define PTP_DPC_CANON_NamePrefix	0xD02B
#define PTP_DPC_CANON_SizeQualityMode	0xD02C
#define PTP_DPC_CANON_SupportedThumbSize	0xD02D
#define PTP_DPC_CANON_SizeOfOutputDataFromCamera	0xD02E
#define PTP_DPC_CANON_SizeOfInputDataToCamera		0xD02F
#define PTP_DPC_CANON_RemoteAPIVersion	0xD030
#define PTP_DPC_CANON_FirmwareVersion	0xD031
#define PTP_DPC_CANON_CameraModel	0xD032
#define PTP_DPC_CANON_CameraOwner	0xD033
#define PTP_DPC_CANON_UnixTime		0xD034
#define PTP_DPC_CANON_CameraBodyID	0xD035
#define PTP_DPC_CANON_CameraOutput	0xD036
#define PTP_DPC_CANON_DispAv		0xD037
#define PTP_DPC_CANON_AvOpenApex	0xD038
#define PTP_DPC_CANON_DZoomMagnification	0xD039
#define PTP_DPC_CANON_MlSpotPos		0xD03A
#define PTP_DPC_CANON_DispAvMax		0xD03B
#define PTP_DPC_CANON_AvMaxApex		0xD03C
#define PTP_DPC_CANON_EZoomStartPosition		0xD03D
#define PTP_DPC_CANON_FocalLengthOfTele	0xD03E
#define PTP_DPC_CANON_EZoomSizeOfTele	0xD03F
#define PTP_DPC_CANON_PhotoEffect	0xD040
#define PTP_DPC_CANON_AssistLight	0xD041
#define PTP_DPC_CANON_FlashQuantityCount	0xD042
#define PTP_DPC_CANON_RotationAngle	0xD043
#define PTP_DPC_CANON_RotationScene	0xD044
#define PTP_DPC_CANON_EventEmulateMode	0xD045
#define PTP_DPC_CANON_DPOFVersion	0xD046
#define PTP_DPC_CANON_TypeOfSupportedSlideShow	0xD047
#define PTP_DPC_CANON_AverageFilesizes	0xD048
#define PTP_DPC_CANON_ModelID		0xD049

/* From EOS 400D trace. */
#define PTP_DPC_CANON_EOS_Aperture		0xD101
#define PTP_DPC_CANON_EOS_ShutterSpeed		0xD102
#define PTP_DPC_CANON_EOS_ISOSpeed		0xD103
#define PTP_DPC_CANON_EOS_ExpCompensation	0xD104
#define PTP_DPC_CANON_EOS_AutoExposureMode	0xD105
#define PTP_DPC_CANON_EOS_DriveMode		0xD106
#define PTP_DPC_CANON_EOS_MeteringMode		0xD107
#define PTP_DPC_CANON_EOS_FocusMode		0xD108
#define PTP_DPC_CANON_EOS_WhiteBalance		0xD109
#define PTP_DPC_CANON_EOS_ColorTemperature	0xD10A
#define PTP_DPC_CANON_EOS_WhiteBalanceAdjustA	0xD10B
#define PTP_DPC_CANON_EOS_WhiteBalanceAdjustB	0xD10C
#define PTP_DPC_CANON_EOS_WhiteBalanceXA	0xD10D
#define PTP_DPC_CANON_EOS_WhiteBalanceXB	0xD10E
#define PTP_DPC_CANON_EOS_ColorSpace		0xD10F
#define PTP_DPC_CANON_EOS_PictureStyle		0xD110
#define PTP_DPC_CANON_EOS_BatteryPower		0xD111
#define PTP_DPC_CANON_EOS_BatterySelect		0xD112
#define PTP_DPC_CANON_EOS_CameraTime		0xD113
#define PTP_DPC_CANON_EOS_AutoPowerOff		0xD114
#define PTP_DPC_CANON_EOS_Owner			0xD115
#define PTP_DPC_CANON_EOS_ModelID		0xD116
#define PTP_DPC_CANON_EOS_PTPExtensionVersion	0xD119
#define PTP_DPC_CANON_EOS_DPOFVersion		0xD11A
#define PTP_DPC_CANON_EOS_AvailableShots	0xD11B
#define PTP_CANON_EOS_CAPTUREDEST_HD		4
#define PTP_DPC_CANON_EOS_CaptureDestination	0xD11C
#define PTP_DPC_CANON_EOS_BracketMode		0xD11D
#define PTP_DPC_CANON_EOS_CurrentStorage	0xD11E
#define PTP_DPC_CANON_EOS_CurrentFolder		0xD11F
#define PTP_DPC_CANON_EOS_ImageFormat		0xD120	/* file setting */
#define PTP_DPC_CANON_EOS_ImageFormatCF		0xD121	/* file setting CF */
#define PTP_DPC_CANON_EOS_ImageFormatSD		0xD122	/* file setting SD */
#define PTP_DPC_CANON_EOS_ImageFormatExtHD	0xD123	/* file setting exthd */
#define PTP_DPC_CANON_EOS_CompressionS		0xD130
#define PTP_DPC_CANON_EOS_CompressionM1		0xD131
#define PTP_DPC_CANON_EOS_CompressionM2		0xD132
#define PTP_DPC_CANON_EOS_CompressionL		0xD133
#define PTP_DPC_CANON_EOS_AEModeDial		0xD138
#define PTP_DPC_CANON_EOS_AEModeCustom		0xD139
#define PTP_DPC_CANON_EOS_MirrorUpSetting	0xD13A
#define PTP_DPC_CANON_EOS_HighlightTonePriority	0xD13B
#define PTP_DPC_CANON_EOS_AFSelectFocusArea	0xD13C
#define PTP_DPC_CANON_EOS_HDRSetting		0xD13D
#define PTP_DPC_CANON_EOS_PCWhiteBalance1	0xD140
#define PTP_DPC_CANON_EOS_PCWhiteBalance2	0xD141
#define PTP_DPC_CANON_EOS_PCWhiteBalance3	0xD142
#define PTP_DPC_CANON_EOS_PCWhiteBalance4	0xD143
#define PTP_DPC_CANON_EOS_PCWhiteBalance5	0xD144
#define PTP_DPC_CANON_EOS_MWhiteBalance		0xD145
#define PTP_DPC_CANON_EOS_MWhiteBalanceEx	0xD146
#define PTP_DPC_CANON_EOS_UnknownPropD14D	0xD14D  /*found in Canon EOS 5D M3*/
#define PTP_DPC_CANON_EOS_PictureStyleStandard	0xD150
#define PTP_DPC_CANON_EOS_PictureStylePortrait	0xD151
#define PTP_DPC_CANON_EOS_PictureStyleLandscape	0xD152
#define PTP_DPC_CANON_EOS_PictureStyleNeutral	0xD153
#define PTP_DPC_CANON_EOS_PictureStyleFaithful	0xD154
#define PTP_DPC_CANON_EOS_PictureStyleBlackWhite	0xD155
#define PTP_DPC_CANON_EOS_PictureStyleAuto	0xD156
#define PTP_DPC_CANON_EOS_PictureStyleUserSet1	0xD160
#define PTP_DPC_CANON_EOS_PictureStyleUserSet2	0xD161
#define PTP_DPC_CANON_EOS_PictureStyleUserSet3	0xD162
#define PTP_DPC_CANON_EOS_PictureStyleParam1	0xD170
#define PTP_DPC_CANON_EOS_PictureStyleParam2	0xD171
#define PTP_DPC_CANON_EOS_PictureStyleParam3	0xD172
#define PTP_DPC_CANON_EOS_HighISOSettingNoiseReduction	0xD178
#define PTP_DPC_CANON_EOS_MovieServoAF		0xD179
#define PTP_DPC_CANON_EOS_ContinuousAFValid	0xD17A
#define PTP_DPC_CANON_EOS_Attenuator		0xD17B
#define PTP_DPC_CANON_EOS_UTCTime		0xD17C
#define PTP_DPC_CANON_EOS_Timezone		0xD17D
#define PTP_DPC_CANON_EOS_Summertime		0xD17E
#define PTP_DPC_CANON_EOS_FlavorLUTParams	0xD17F
#define PTP_DPC_CANON_EOS_CustomFunc1		0xD180
#define PTP_DPC_CANON_EOS_CustomFunc2		0xD181
#define PTP_DPC_CANON_EOS_CustomFunc3		0xD182
#define PTP_DPC_CANON_EOS_CustomFunc4		0xD183
#define PTP_DPC_CANON_EOS_CustomFunc5		0xD184
#define PTP_DPC_CANON_EOS_CustomFunc6		0xD185
#define PTP_DPC_CANON_EOS_CustomFunc7		0xD186
#define PTP_DPC_CANON_EOS_CustomFunc8		0xD187
#define PTP_DPC_CANON_EOS_CustomFunc9		0xD188
#define PTP_DPC_CANON_EOS_CustomFunc10		0xD189
#define PTP_DPC_CANON_EOS_CustomFunc11		0xD18a
#define PTP_DPC_CANON_EOS_CustomFunc12		0xD18b
#define PTP_DPC_CANON_EOS_CustomFunc13		0xD18c
#define PTP_DPC_CANON_EOS_CustomFunc14		0xD18d
#define PTP_DPC_CANON_EOS_CustomFunc15		0xD18e
#define PTP_DPC_CANON_EOS_CustomFunc16		0xD18f
#define PTP_DPC_CANON_EOS_CustomFunc17		0xD190
#define PTP_DPC_CANON_EOS_CustomFunc18		0xD191
#define PTP_DPC_CANON_EOS_CustomFunc19		0xD192
#define PTP_DPC_CANON_EOS_CustomFunc19		0xD192
#define PTP_DPC_CANON_EOS_InnerDevelop		0xD193
#define PTP_DPC_CANON_EOS_MultiAspect		0xD194
#define PTP_DPC_CANON_EOS_MovieSoundRecord	0xD195
#define PTP_DPC_CANON_EOS_MovieRecordVolume	0xD196
#define PTP_DPC_CANON_EOS_WindCut		0xD197
#define PTP_DPC_CANON_EOS_ExtenderType		0xD198
#define PTP_DPC_CANON_EOS_OLCInfoVersion	0xD199
#define PTP_DPC_CANON_EOS_UnknownPropD19A	0xD19A /*found in Canon EOS 5D M3*/
#define PTP_DPC_CANON_EOS_UnknownPropD19C	0xD19C /*found in Canon EOS 5D M3*/
#define PTP_DPC_CANON_EOS_UnknownPropD19D	0xD19D /*found in Canon EOS 5D M3*/
#define PTP_DPC_CANON_EOS_CustomFuncEx		0xD1a0
#define PTP_DPC_CANON_EOS_MyMenu		0xD1a1
#define PTP_DPC_CANON_EOS_MyMenuList		0xD1a2
#define PTP_DPC_CANON_EOS_WftStatus		0xD1a3
#define PTP_DPC_CANON_EOS_WftInputTransmission	0xD1a4
#define PTP_DPC_CANON_EOS_HDDirectoryStructure	0xD1a5
#define PTP_DPC_CANON_EOS_BatteryInfo		0xD1a6
#define PTP_DPC_CANON_EOS_AdapterInfo		0xD1a7
#define PTP_DPC_CANON_EOS_LensStatus		0xD1a8
#define PTP_DPC_CANON_EOS_QuickReviewTime	0xD1a9
#define PTP_DPC_CANON_EOS_CardExtension		0xD1aa
#define PTP_DPC_CANON_EOS_TempStatus		0xD1ab
#define PTP_DPC_CANON_EOS_ShutterCounter	0xD1ac
#define PTP_DPC_CANON_EOS_SpecialOption		0xD1ad
#define PTP_DPC_CANON_EOS_PhotoStudioMode	0xD1ae
#define PTP_DPC_CANON_EOS_SerialNumber		0xD1af
#define PTP_DPC_CANON_EOS_EVFOutputDevice	0xD1b0
#define PTP_DPC_CANON_EOS_EVFMode		0xD1b1
#define PTP_DPC_CANON_EOS_DepthOfFieldPreview	0xD1b2
#define PTP_DPC_CANON_EOS_EVFSharpness		0xD1b3
#define PTP_DPC_CANON_EOS_EVFWBMode		0xD1b4
#define PTP_DPC_CANON_EOS_EVFClickWBCoeffs	0xD1b5
#define PTP_DPC_CANON_EOS_EVFColorTemp		0xD1b6
#define PTP_DPC_CANON_EOS_ExposureSimMode	0xD1b7
#define PTP_DPC_CANON_EOS_EVFRecordStatus	0xD1b8
#define PTP_DPC_CANON_EOS_LvAfSystem		0xD1ba
#define PTP_DPC_CANON_EOS_MovSize		0xD1bb
#define PTP_DPC_CANON_EOS_LvViewTypeSelect	0xD1bc
#define PTP_DPC_CANON_EOS_MirrorDownStatus	0xD1bd
#define PTP_DPC_CANON_EOS_MovieParam		0xD1be
#define PTP_DPC_CANON_EOS_MirrorLockupState	0xD1bf
#define PTP_DPC_CANON_EOS_FlashChargingState	0xD1C0
#define PTP_DPC_CANON_EOS_AloMode		0xD1C1
#define PTP_DPC_CANON_EOS_FixedMovie		0xD1C2
#define PTP_DPC_CANON_EOS_OneShotRawOn		0xD1C3
#define PTP_DPC_CANON_EOS_ErrorForDisplay	0xD1C4
#define PTP_DPC_CANON_EOS_AEModeMovie		0xD1C5
#define PTP_DPC_CANON_EOS_BuiltinStroboMode	0xD1C6
#define PTP_DPC_CANON_EOS_StroboDispState	0xD1C7
#define PTP_DPC_CANON_EOS_StroboETTL2Metering	0xD1C8
#define PTP_DPC_CANON_EOS_ContinousAFMode	0xD1C9
#define PTP_DPC_CANON_EOS_MovieParam2		0xD1CA
#define PTP_DPC_CANON_EOS_StroboSettingExpComposition		0xD1CB
#define PTP_DPC_CANON_EOS_MovieParam3		0xD1CC
#define PTP_DPC_CANON_EOS_LVMedicalRotate	0xD1CF
#define PTP_DPC_CANON_EOS_Artist		0xD1d0
#define PTP_DPC_CANON_EOS_Copyright		0xD1d1
#define PTP_DPC_CANON_EOS_BracketValue		0xD1d2
#define PTP_DPC_CANON_EOS_FocusInfoEx		0xD1d3
#define PTP_DPC_CANON_EOS_DepthOfField		0xD1d4
#define PTP_DPC_CANON_EOS_Brightness		0xD1d5
#define PTP_DPC_CANON_EOS_LensAdjustParams	0xD1d6
#define PTP_DPC_CANON_EOS_EFComp		0xD1d7
#define PTP_DPC_CANON_EOS_LensName		0xD1d8
#define PTP_DPC_CANON_EOS_AEB			0xD1d9
#define PTP_DPC_CANON_EOS_StroboSetting		0xD1da
#define PTP_DPC_CANON_EOS_StroboWirelessSetting	0xD1db
#define PTP_DPC_CANON_EOS_StroboFiring		0xD1dc
#define PTP_DPC_CANON_EOS_LensID		0xD1dd
#define PTP_DPC_CANON_EOS_LCDBrightness		0xD1de
#define PTP_DPC_CANON_EOS_CADarkBright		0xD1df

/* Nikon extension device property codes */
#define PTP_DPC_NIKON_ShootingBank			0xD010
#define PTP_DPC_NIKON_ShootingBankNameA 		0xD011
#define PTP_DPC_NIKON_ShootingBankNameB			0xD012
#define PTP_DPC_NIKON_ShootingBankNameC			0xD013
#define PTP_DPC_NIKON_ShootingBankNameD			0xD014
#define PTP_DPC_NIKON_ResetBank0			0xD015
#define PTP_DPC_NIKON_RawCompression			0xD016
#define PTP_DPC_NIKON_WhiteBalanceAutoBias		0xD017
#define PTP_DPC_NIKON_WhiteBalanceTungstenBias		0xD018
#define PTP_DPC_NIKON_WhiteBalanceFluorescentBias	0xD019
#define PTP_DPC_NIKON_WhiteBalanceDaylightBias		0xD01A
#define PTP_DPC_NIKON_WhiteBalanceFlashBias		0xD01B
#define PTP_DPC_NIKON_WhiteBalanceCloudyBias		0xD01C
#define PTP_DPC_NIKON_WhiteBalanceShadeBias		0xD01D
#define PTP_DPC_NIKON_WhiteBalanceColorTemperature	0xD01E
#define PTP_DPC_NIKON_WhiteBalancePresetNo		0xD01F
#define PTP_DPC_NIKON_WhiteBalancePresetName0		0xD020
#define PTP_DPC_NIKON_WhiteBalancePresetName1		0xD021
#define PTP_DPC_NIKON_WhiteBalancePresetName2		0xD022
#define PTP_DPC_NIKON_WhiteBalancePresetName3		0xD023
#define PTP_DPC_NIKON_WhiteBalancePresetName4		0xD024
#define PTP_DPC_NIKON_WhiteBalancePresetVal0		0xD025
#define PTP_DPC_NIKON_WhiteBalancePresetVal1		0xD026
#define PTP_DPC_NIKON_WhiteBalancePresetVal2		0xD027
#define PTP_DPC_NIKON_WhiteBalancePresetVal3		0xD028
#define PTP_DPC_NIKON_WhiteBalancePresetVal4		0xD029
#define PTP_DPC_NIKON_ImageSharpening			0xD02A
#define PTP_DPC_NIKON_ToneCompensation			0xD02B
#define PTP_DPC_NIKON_ColorModel			0xD02C
#define PTP_DPC_NIKON_HueAdjustment			0xD02D
#define PTP_DPC_NIKON_NonCPULensDataFocalLength		0xD02E	/* Set FMM Manual */
#define PTP_DPC_NIKON_NonCPULensDataMaximumAperture	0xD02F	/* Set F0 Manual */
#define PTP_DPC_NIKON_ShootingMode			0xD030
#define PTP_DPC_NIKON_JPEG_Compression_Policy		0xD031
#define PTP_DPC_NIKON_ColorSpace			0xD032
#define PTP_DPC_NIKON_AutoDXCrop			0xD033
#define PTP_DPC_NIKON_FlickerReduction			0xD034
#define PTP_DPC_NIKON_RemoteMode			0xD035
#define PTP_DPC_NIKON_VideoMode				0xD036
#define PTP_DPC_NIKON_EffectMode			0xD037
#define PTP_DPC_NIKON_CSMMenuBankSelect			0xD040
#define PTP_DPC_NIKON_MenuBankNameA			0xD041
#define PTP_DPC_NIKON_MenuBankNameB			0xD042
#define PTP_DPC_NIKON_MenuBankNameC			0xD043
#define PTP_DPC_NIKON_MenuBankNameD			0xD044
#define PTP_DPC_NIKON_ResetBank				0xD045
#define PTP_DPC_NIKON_A1AFCModePriority			0xD048
#define PTP_DPC_NIKON_A2AFSModePriority			0xD049
#define PTP_DPC_NIKON_A3GroupDynamicAF			0xD04A
#define PTP_DPC_NIKON_A4AFActivation			0xD04B
#define PTP_DPC_NIKON_FocusAreaIllumManualFocus		0xD04C
#define PTP_DPC_NIKON_FocusAreaIllumContinuous		0xD04D
#define PTP_DPC_NIKON_FocusAreaIllumWhenSelected 	0xD04E
#define PTP_DPC_NIKON_FocusAreaWrap			0xD04F /* area sel */
#define PTP_DPC_NIKON_VerticalAFON			0xD050
#define PTP_DPC_NIKON_AFLockOn				0xD051
#define PTP_DPC_NIKON_FocusAreaZone			0xD052
#define PTP_DPC_NIKON_EnableCopyright			0xD053
#define PTP_DPC_NIKON_ISOAuto				0xD054
#define PTP_DPC_NIKON_EVISOStep				0xD055
#define PTP_DPC_NIKON_EVStep				0xD056 /* EV Step SS FN */
#define PTP_DPC_NIKON_EVStepExposureComp	0xD057
#define PTP_DPC_NIKON_ExposureCompensation	0xD058
#define PTP_DPC_NIKON_CenterWeightArea		0xD059
#define PTP_DPC_NIKON_ExposureBaseMatrix	0xD05A
#define PTP_DPC_NIKON_ExposureBaseCenter	0xD05B
#define PTP_DPC_NIKON_ExposureBaseSpot		0xD05C
#define PTP_DPC_NIKON_LiveViewAFArea		0xD05D /* FIXME: AfAtLiveview? */
#define PTP_DPC_NIKON_AELockMode			0xD05E
#define PTP_DPC_NIKON_AELAFLMode			0xD05F
#define PTP_DPC_NIKON_LiveViewAFFocus		0xD061
#define PTP_DPC_NIKON_MeterOff				0xD062
#define PTP_DPC_NIKON_SelfTimer				0xD063
#define PTP_DPC_NIKON_MonitorOff			0xD064
#define PTP_DPC_NIKON_ImgConfTime			0xD065
#define PTP_DPC_NIKON_AutoOffTimers			0xD066
#define PTP_DPC_NIKON_AngleLevel			0xD067
#define PTP_DPC_NIKON_D1ShootingSpeed		0xD068 /* continous speed low */
#define PTP_DPC_NIKON_D2MaximumShots		0xD069
#define PTP_DPC_NIKON_ExposureDelayMode		0xD06A
#define PTP_DPC_NIKON_LongExposureNoiseReduction	0xD06B
#define PTP_DPC_NIKON_FileNumberSequence	0xD06C
#define PTP_DPC_NIKON_ControlPanelFinderRearControl	0xD06D
#define PTP_DPC_NIKON_ControlPanelFinderViewfinder	0xD06E
#define PTP_DPC_NIKON_D7Illumination		0xD06F
#define PTP_DPC_NIKON_NrHighISO				0xD070
#define PTP_DPC_NIKON_SHSET_CH_GUID_DISP	0xD071
#define PTP_DPC_NIKON_ArtistName			0xD072
#define PTP_DPC_NIKON_CopyrightInfo			0xD073
#define PTP_DPC_NIKON_FlashSyncSpeed		0xD074
#define PTP_DPC_NIKON_FlashShutterSpeed		0xD075	/* SB Low Limit */
#define PTP_DPC_NIKON_E3AAFlashMode			0xD076
#define PTP_DPC_NIKON_E4ModelingFlash		0xD077
#define PTP_DPC_NIKON_BracketSet			0xD078	/* Bracket Type? */
#define PTP_DPC_NIKON_E6ManualModeBracketing		0xD079	/* Bracket Factor? */
#define PTP_DPC_NIKON_BracketOrder			0xD07A
#define PTP_DPC_NIKON_E8AutoBracketSelection		0xD07B	/* Bracket Method? */
#define PTP_DPC_NIKON_BracketingSet			0xD07C
#define PTP_DPC_NIKON_F1CenterButtonShootingMode	0xD080
#define PTP_DPC_NIKON_CenterButtonPlaybackMode		0xD081
#define PTP_DPC_NIKON_F2Multiselector		0xD082
#define PTP_DPC_NIKON_F3PhotoInfoPlayback	0xD083	/* MultiSelector Dir */
#define PTP_DPC_NIKON_F4AssignFuncButton	0xD084  /* CMD Dial Rotate */
#define PTP_DPC_NIKON_F5CustomizeCommDials	0xD085  /* CMD Dial Change */
#define PTP_DPC_NIKON_ReverseCommandDial	0xD086  /* CMD Dial FN Set */
#define PTP_DPC_NIKON_ApertureSetting		0xD087  /* CMD Dial Active */
#define PTP_DPC_NIKON_MenusAndPlayback		0xD088  /* CMD Dial Active */
#define PTP_DPC_NIKON_F6ButtonsAndDials		0xD089  /* Universal Mode? */
#define PTP_DPC_NIKON_NoCFCard				0xD08A	/* Enable Shutter? */
#define PTP_DPC_NIKON_CenterButtonZoomRatio	0xD08B
#define PTP_DPC_NIKON_FunctionButton2		0xD08C
#define PTP_DPC_NIKON_AFAreaPoint			0xD08D
#define PTP_DPC_NIKON_NormalAFOn			0xD08E
#define PTP_DPC_NIKON_CleanImageSensor		0xD08F
#define PTP_DPC_NIKON_ImageCommentString	0xD090
#define PTP_DPC_NIKON_ImageCommentEnable	0xD091
#define PTP_DPC_NIKON_ImageRotation			0xD092
#define PTP_DPC_NIKON_ManualSetLensNo		0xD093
#define PTP_DPC_NIKON_MovScreenSize			0xD0A0
#define PTP_DPC_NIKON_MovVoice				0xD0A1
#define PTP_DPC_NIKON_MovMicrophone			0xD0A2
#define PTP_DPC_NIKON_MovFileSlot			0xD0A3
#define PTP_DPC_NIKON_MovRecProhibitCondition		0xD0A4
#define PTP_DPC_NIKON_ManualMovieSetting	0xD0A6
#define PTP_DPC_NIKON_MovQuality			0xD0A7
#define PTP_DPC_NIKON_LiveViewScreenDisplaySetting	0xD0B2
#define PTP_DPC_NIKON_MonitorOffDelay		0xD0B3
#define PTP_DPC_NIKON_Bracketing			0xD0C0
#define PTP_DPC_NIKON_AutoExposureBracketStep		0xD0C1
#define PTP_DPC_NIKON_AutoExposureBracketProgram	0xD0C2
#define PTP_DPC_NIKON_AutoExposureBracketCount		0xD0C3
#define PTP_DPC_NIKON_WhiteBalanceBracketStep		0xD0C4
#define PTP_DPC_NIKON_WhiteBalanceBracketProgram	0xD0C5
#define PTP_DPC_NIKON_LensID				0xD0E0
#define PTP_DPC_NIKON_LensSort				0xD0E1
#define PTP_DPC_NIKON_LensType				0xD0E2
#define PTP_DPC_NIKON_FocalLengthMin		0xD0E3
#define PTP_DPC_NIKON_FocalLengthMax		0xD0E4
#define PTP_DPC_NIKON_MaxApAtMinFocalLength	0xD0E5
#define PTP_DPC_NIKON_MaxApAtMaxFocalLength	0xD0E6
#define PTP_DPC_NIKON_FinderISODisp			0xD0F0
#define PTP_DPC_NIKON_AutoOffPhoto			0xD0F2
#define PTP_DPC_NIKON_AutoOffMenu			0xD0F3
#define PTP_DPC_NIKON_AutoOffInfo			0xD0F4
#define PTP_DPC_NIKON_SelfTimerShootNum		0xD0F5
#define PTP_DPC_NIKON_VignetteCtrl			0xD0F7
#define PTP_DPC_NIKON_AutoDistortionControl	0xD0F8
#define PTP_DPC_NIKON_SceneMode				0xD0F9
#define PTP_DPC_NIKON_SceneMode2			0xD0FD
#define PTP_DPC_NIKON_SelfTimerInterval		0xD0FE
#define PTP_DPC_NIKON_ExposureTime			0xD100	/* Shutter Speed */
#define PTP_DPC_NIKON_ACPower				0xD101
#define PTP_DPC_NIKON_WarningStatus			0xD102
#define PTP_DPC_NIKON_MaximumShots			0xD103 /* remain shots (in RAM buffer?) */
#define PTP_DPC_NIKON_AFLockStatus			0xD104
#define PTP_DPC_NIKON_AELockStatus			0xD105
#define PTP_DPC_NIKON_FVLockStatus			0xD106
#define PTP_DPC_NIKON_AutofocusLCDTopMode2	0xD107
#define PTP_DPC_NIKON_AutofocusArea			0xD108
#define PTP_DPC_NIKON_FlexibleProgram		0xD109
#define PTP_DPC_NIKON_LightMeter			0xD10A	/* Exposure Status */
#define PTP_DPC_NIKON_RecordingMedia		0xD10B	/* Card or SDRAM */
#define PTP_DPC_NIKON_USBSpeed				0xD10C
#define PTP_DPC_NIKON_CCDNumber				0xD10D
#define PTP_DPC_NIKON_CameraOrientation		0xD10E
#define PTP_DPC_NIKON_GroupPtnType			0xD10F
#define PTP_DPC_NIKON_FNumberLock			0xD110
#define PTP_DPC_NIKON_ExposureApertureLock	0xD111	/* shutterspeed lock*/
#define PTP_DPC_NIKON_TVLockSetting			0xD112
#define PTP_DPC_NIKON_AVLockSetting			0xD113
#define PTP_DPC_NIKON_IllumSetting			0xD114
#define PTP_DPC_NIKON_FocusPointBright		0xD115
#define PTP_DPC_NIKON_ExternalFlashAttached	0xD120
#define PTP_DPC_NIKON_ExternalFlashStatus	0xD121
#define PTP_DPC_NIKON_ExternalFlashSort		0xD122
#define PTP_DPC_NIKON_ExternalFlashMode		0xD123
#define PTP_DPC_NIKON_ExternalFlashCompensation		0xD124
#define PTP_DPC_NIKON_NewExternalFlashMode	0xD125
#define PTP_DPC_NIKON_FlashExposureCompensation		0xD126
#define PTP_DPC_NIKON_HDRMode				0xD130
#define PTP_DPC_NIKON_HDRHighDynamic		0xD131
#define PTP_DPC_NIKON_HDRSmoothing			0xD132
#define PTP_DPC_NIKON_OptimizeImage			0xD140
#define PTP_DPC_NIKON_Saturation			0xD142
#define PTP_DPC_NIKON_BW_FillerEffect		0xD143
#define PTP_DPC_NIKON_BW_Sharpness			0xD144
#define PTP_DPC_NIKON_BW_Contrast			0xD145
#define PTP_DPC_NIKON_BW_Setting_Type		0xD146
#define PTP_DPC_NIKON_Slot2SaveMode			0xD148
#define PTP_DPC_NIKON_RawBitMode			0xD149
#define PTP_DPC_NIKON_ActiveDLighting		0xD14E /* was PTP_DPC_NIKON_ISOAutoTime */
#define PTP_DPC_NIKON_FlourescentType		0xD14F
#define PTP_DPC_NIKON_TuneColourTemperature	0xD150
#define PTP_DPC_NIKON_TunePreset0			0xD151
#define PTP_DPC_NIKON_TunePreset1			0xD152
#define PTP_DPC_NIKON_TunePreset2			0xD153
#define PTP_DPC_NIKON_TunePreset3			0xD154
#define PTP_DPC_NIKON_TunePreset4			0xD155
#define PTP_DPC_NIKON_BeepOff				0xD160
#define PTP_DPC_NIKON_AutofocusMode			0xD161
#define PTP_DPC_NIKON_AFAssist				0xD163
#define PTP_DPC_NIKON_PADVPMode				0xD164	/* iso auto time */
#define PTP_DPC_NIKON_ImageReview			0xD165
#define PTP_DPC_NIKON_AFAreaIllumination	0xD166
#define PTP_DPC_NIKON_FlashMode				0xD167
#define PTP_DPC_NIKON_FlashCommanderMode	0xD168
#define PTP_DPC_NIKON_FlashSign				0xD169
#define PTP_DPC_NIKON_ISO_Auto				0xD16A
#define PTP_DPC_NIKON_RemoteTimeout			0xD16B
#define PTP_DPC_NIKON_GridDisplay			0xD16C
#define PTP_DPC_NIKON_FlashModeManualPower	0xD16D
#define PTP_DPC_NIKON_FlashModeCommanderPower		0xD16E
#define PTP_DPC_NIKON_AutoFP				0xD16F
#define PTP_DPC_NIKON_DateImprintSetting	0xD170
#define PTP_DPC_NIKON_DateCounterSelect		0xD171
#define PTP_DPC_NIKON_DateCountData			0xD172
#define PTP_DPC_NIKON_DateCountDisplaySetting		0xD173
#define PTP_DPC_NIKON_RangeFinderSetting	0xD174
#define PTP_DPC_NIKON_CSMMenu				0xD180
#define PTP_DPC_NIKON_WarningDisplay		0xD181
#define PTP_DPC_NIKON_BatteryCellKind		0xD182
#define PTP_DPC_NIKON_ISOAutoHiLimit		0xD183
#define PTP_DPC_NIKON_DynamicAFArea			0xD184
#define PTP_DPC_NIKON_ContinuousSpeedHigh	0xD186
#define PTP_DPC_NIKON_InfoDispSetting		0xD187
#define PTP_DPC_NIKON_PreviewButton			0xD189
#define PTP_DPC_NIKON_PreviewButton2		0xD18A
#define PTP_DPC_NIKON_AEAFLockButton2		0xD18B
#define PTP_DPC_NIKON_IndicatorDisp			0xD18D
#define PTP_DPC_NIKON_CellKindPriority		0xD18E
#define PTP_DPC_NIKON_BracketingFramesAndSteps		0xD190
#define PTP_DPC_NIKON_LiveViewMode			0xD1A0
#define PTP_DPC_NIKON_LiveViewDriveMode		0xD1A1
#define PTP_DPC_NIKON_LiveViewStatus		0xD1A2
#define PTP_DPC_NIKON_LiveViewImageZoomRatio		0xD1A3
#define PTP_DPC_NIKON_LiveViewProhibitCondition		0xD1A4
#define PTP_DPC_NIKON_LiveViewMovieMode		0xD1AC /* ? */
#define PTP_DPC_NIKON_ExposureDisplayStatus	0xD1B0
#define PTP_DPC_NIKON_ExposureIndicateStatus		0xD1B1
#define PTP_DPC_NIKON_InfoDispErrStatus		0xD1B2
#define PTP_DPC_NIKON_ExposureIndicateLightup		0xD1B3
#define PTP_DPC_NIKON_FlashOpen				0xD1C0
#define PTP_DPC_NIKON_FlashCharged			0xD1C1
#define PTP_DPC_NIKON_FlashMRepeatValue		0xD1D0
#define PTP_DPC_NIKON_FlashMRepeatCount		0xD1D1
#define PTP_DPC_NIKON_FlashMRepeatInterval	0xD1D2
#define PTP_DPC_NIKON_FlashCommandChannel	0xD1D3
#define PTP_DPC_NIKON_FlashCommandSelfMode	0xD1D4
#define PTP_DPC_NIKON_FlashCommandSelfCompensation	0xD1D5
#define PTP_DPC_NIKON_FlashCommandSelfValue	0xD1D6
#define PTP_DPC_NIKON_FlashCommandAMode		0xD1D7
#define PTP_DPC_NIKON_FlashCommandACompensation		0xD1D8
#define PTP_DPC_NIKON_FlashCommandAValue	0xD1D9
#define PTP_DPC_NIKON_FlashCommandBMode		0xD1DA
#define PTP_DPC_NIKON_FlashCommandBCompensation		0xD1DB
#define PTP_DPC_NIKON_FlashCommandBValue	0xD1DC
#define PTP_DPC_NIKON_ApplicationMode		0xD1F0
#define PTP_DPC_NIKON_ActiveSlot			0xD1F2
#define PTP_DPC_NIKON_ActivePicCtrlItem		0xD200
#define PTP_DPC_NIKON_ChangePicCtrlItem		0xD201


/* Nikon V1 (or WU adapter?) Trace */
/* d241 - gets string "Nikon_WU2_0090B5123C61" */
#define PTP_DPC_NIKON_D241					0xD241
/* d244 - gets a single byte 0x00 */
#define PTP_DPC_NIKON_D244					0xD244
/* d247 - gets 3 bytes 0x01 0x00 0x00 */
#define PTP_DPC_NIKON_D247					0xD247
/* d250 - gets a string "0000123C61" */
#define PTP_DPC_NIKON_D250					0xD250
/* d251 - gets a 0x0100000d */
#define PTP_DPC_NIKON_D251					0xD251

/* this is irregular, as it should be -0x5000 or 0xD000 based */
#define PTP_DPC_NIKON_1_ISO					0xF002
#define PTP_DPC_NIKON_1_ImageCompression	0xF009
#define PTP_DPC_NIKON_1_ImageSize			0xF00A
#define PTP_DPC_NIKON_1_WhiteBalance		0xF00C
#define PTP_DPC_NIKON_1_LongExposureNoiseReduction	0xF00D
#define PTP_DPC_NIKON_1_HiISONoiseReduction	0xF00E
#define PTP_DPC_NIKON_1_MovQuality			0xF01C

/* Fuji specific */
#define PTP_DPC_FUJI_ColorTemperature		0xD017
#define PTP_DPC_FUJI_Quality				0xD018
#define PTP_DPC_FUJI_ReleaseMode			0xD201
#define PTP_DPC_FUJI_FocusAreas				0xD206
#define PTP_DPC_FUJI_AELock					0xD213
#define PTP_DPC_FUJI_Aperture				0xD218
#define PTP_DPC_FUJI_ShutterSpeed			0xD219

/* Microsoft/MTP specific */
#define PTP_DPC_MTP_SecureTime				0xD101
#define PTP_DPC_MTP_DeviceCertificate		0xD102
#define PTP_DPC_MTP_RevocationInfo			0xD103
#define PTP_DPC_MTP_SynchronizationPartner	0xD401
#define PTP_DPC_MTP_DeviceFriendlyName		0xD402
#define PTP_DPC_MTP_VolumeLevel				0xD403
#define PTP_DPC_MTP_DeviceIcon				0xD405
#define PTP_DPC_MTP_SessionInitiatorInfo	0xD406
#define PTP_DPC_MTP_PerceivedDeviceType		0xD407
#define PTP_DPC_MTP_PlaybackRate            0xD410
#define PTP_DPC_MTP_PlaybackObject          0xD411
#define PTP_DPC_MTP_PlaybackContainerIndex  0xD412
#define PTP_DPC_MTP_PlaybackPosition        0xD413
#define PTP_DPC_MTP_PlaysForSureID          0xD131

/* Zune specific property codes */
#define PTP_DPC_MTP_Zune_UnknownVersion		0xD181

/* Olympus */
#define PTP_DPC_OLYMPUS_ResolutionMode		0xD102
#define PTP_DPC_OLYMPUS_FocusPriority		0xD103
#define PTP_DPC_OLYMPUS_DriveMode			0xD104
#define PTP_DPC_OLYMPUS_DateTimeFormat		0xD105
#define PTP_DPC_OLYMPUS_ExposureBiasStep	0xD106
#define PTP_DPC_OLYMPUS_WBMode				0xD107
#define PTP_DPC_OLYMPUS_OneTouchWB			0xD108
#define PTP_DPC_OLYMPUS_ManualWB			0xD109
#define PTP_DPC_OLYMPUS_ManualWBRBBias		0xD10A
#define PTP_DPC_OLYMPUS_CustomWB			0xD10B
#define PTP_DPC_OLYMPUS_CustomWBValue		0xD10C
#define PTP_DPC_OLYMPUS_ExposureTimeEx		0xD10D
#define PTP_DPC_OLYMPUS_BulbMode			0xD10E
#define PTP_DPC_OLYMPUS_AntiMirrorMode		0xD10F
#define PTP_DPC_OLYMPUS_AEBracketingFrame	0xD110
#define PTP_DPC_OLYMPUS_AEBracketingStep	0xD111
#define PTP_DPC_OLYMPUS_WBBracketingFrame	0xD112
#define PTP_DPC_OLYMPUS_WBBracketingRBFrame	0xD112 /* dup ? */
#define PTP_DPC_OLYMPUS_WBBracketingRBRange	0xD113
#define PTP_DPC_OLYMPUS_WBBracketingGMFrame	0xD114
#define PTP_DPC_OLYMPUS_WBBracketingGMRange	0xD115
#define PTP_DPC_OLYMPUS_FLBracketingFrame	0xD118
#define PTP_DPC_OLYMPUS_FLBracketingStep	0xD119
#define PTP_DPC_OLYMPUS_FlashBiasCompensation		0xD11A
#define PTP_DPC_OLYMPUS_ManualFocusMode		0xD11B
#define PTP_DPC_OLYMPUS_RawSaveMode			0xD11D
#define PTP_DPC_OLYMPUS_AUXLightMode		0xD11E
#define PTP_DPC_OLYMPUS_LensSinkMode		0xD11F
#define PTP_DPC_OLYMPUS_BeepStatus			0xD120
#define PTP_DPC_OLYMPUS_ColorSpace			0xD122
#define PTP_DPC_OLYMPUS_ColorMatching		0xD123
#define PTP_DPC_OLYMPUS_Saturation			0xD124
#define PTP_DPC_OLYMPUS_NoiseReductionPattern		0xD126
#define PTP_DPC_OLYMPUS_NoiseReductionRandom		0xD127
#define PTP_DPC_OLYMPUS_ShadingMode			0xD129
#define PTP_DPC_OLYMPUS_ISOBoostMode		0xD12A
#define PTP_DPC_OLYMPUS_ExposureIndexBiasStep		0xD12B
#define PTP_DPC_OLYMPUS_FilterEffect		0xD12C
#define PTP_DPC_OLYMPUS_ColorTune			0xD12D
#define PTP_DPC_OLYMPUS_Language			0xD12E
#define PTP_DPC_OLYMPUS_LanguageCode		0xD12F
#define PTP_DPC_OLYMPUS_RecviewMode			0xD130
#define PTP_DPC_OLYMPUS_SleepTime			0xD131
#define PTP_DPC_OLYMPUS_ManualWBGMBias		0xD132
#define PTP_DPC_OLYMPUS_AELAFLMode			0xD135
#define PTP_DPC_OLYMPUS_AELButtonStatus		0xD136
#define PTP_DPC_OLYMPUS_CompressionSettingEx		0xD137
#define PTP_DPC_OLYMPUS_ToneMode			0xD139
#define PTP_DPC_OLYMPUS_GradationMode		0xD13A
#define PTP_DPC_OLYMPUS_DevelopMode			0xD13B
#define PTP_DPC_OLYMPUS_ExtendInnerFlashMode		0xD13C
#define PTP_DPC_OLYMPUS_OutputDeviceMode	0xD13D
#define PTP_DPC_OLYMPUS_LiveViewMode		0xD13E
#define PTP_DPC_OLYMPUS_LCDBacklight		0xD140
#define PTP_DPC_OLYMPUS_CustomDevelop		0xD141
#define PTP_DPC_OLYMPUS_GradationAutoBias	0xD142
#define PTP_DPC_OLYMPUS_FlashRCMode			0xD143
#define PTP_DPC_OLYMPUS_FlashRCGroupValue	0xD144
#define PTP_DPC_OLYMPUS_FlashRCChannelValue	0xD145
#define PTP_DPC_OLYMPUS_FlashRCFPMode		0xD146
#define PTP_DPC_OLYMPUS_FlashRCPhotoChromicMode		0xD147
#define PTP_DPC_OLYMPUS_FlashRCPhotoChromicBias		0xD148
#define PTP_DPC_OLYMPUS_FlashRCPhotoChromicManualBias	0xD149
#define PTP_DPC_OLYMPUS_FlashRCQuantityLightLevel	0xD14A
#define PTP_DPC_OLYMPUS_FocusMeteringValue	0xD14B
#define PTP_DPC_OLYMPUS_ISOBracketingFrame	0xD14C
#define PTP_DPC_OLYMPUS_ISOBracketingStep	0xD14D
#define PTP_DPC_OLYMPUS_BulbMFMode			0xD14E
#define PTP_DPC_OLYMPUS_BurstFPSValue		0xD14F
#define PTP_DPC_OLYMPUS_ISOAutoBaseValue	0xD150
#define PTP_DPC_OLYMPUS_ISOAutoMaxValue		0xD151
#define PTP_DPC_OLYMPUS_BulbLimiterValue	0xD152
#define PTP_DPC_OLYMPUS_DPIMode				0xD153
#define PTP_DPC_OLYMPUS_DPICustomValue		0xD154
#define PTP_DPC_OLYMPUS_ResolutionValueSetting		0xD155
#define PTP_DPC_OLYMPUS_AFTargetSize		0xD157
#define PTP_DPC_OLYMPUS_LightSensorMode		0xD158
#define PTP_DPC_OLYMPUS_AEBracket			0xD159
#define PTP_DPC_OLYMPUS_WBRBBracket			0xD15A
#define PTP_DPC_OLYMPUS_WBGMBracket			0xD15B
#define PTP_DPC_OLYMPUS_FlashBracket		0xD15C
#define PTP_DPC_OLYMPUS_ISOBracket			0xD15D
#define PTP_DPC_OLYMPUS_MyModeStatus		0xD15E

/* Sony A900 */
#define PTP_DPC_SONY_DPCCompensation		0xD200
#define PTP_DPC_SONY_DRangeOptimize			0xD201
#define PTP_DPC_SONY_ImageSize				0xD203
#define PTP_DPC_SONY_ShutterSpeed			0xD20D
#define PTP_DPC_SONY_ColorTemp				0xD20F
#define PTP_DPC_SONY_CCFilter				0xD210
#define PTP_DPC_SONY_AspectRatio			0xD211
#define PTP_DPC_SONY_ExposeIndex			0xD216
#define PTP_DPC_SONY_PictureEffect			0xD21B
#define PTP_DPC_SONY_ABFilter				0xD21C
#define PTP_DPC_SONY_ISO					0xD21E /* ? */
/* also seen: D2C3 D2C4 */
/* semi control opcodes */
#define PTP_DPC_SONY_Movie					0xD2C8 /* ? */
#define PTP_DPC_SONY_StillImage				0xD2C7 /* ? */



/* Casio EX-F1 */
#define PTP_DPC_CASIO_MONITOR		0xD001 
#define PTP_DPC_CASIO_STORAGE		0xD002 //Not reported by DeviceInfo?
#define PTP_DPC_CASIO_UNKNOWN_1		0xD004
#define PTP_DPC_CASIO_UNKNOWN_2		0xD005
#define PTP_DPC_CASIO_UNKNOWN_3		0xD007
#define PTP_DPC_CASIO_RECORD_LIGHT	0xD008
#define PTP_DPC_CASIO_UNKNOWN_4		0xD009
#define PTP_DPC_CASIO_UNKNOWN_5		0xD00A
#define PTP_DPC_CASIO_MOVIE_MODE	0xD00B
#define PTP_DPC_CASIO_HD_SETTING	0xD00C
#define PTP_DPC_CASIO_HS_SETTING	0xD00D
#define PTP_DPC_CASIO_CS_HIGH_SPEED	0xD00F
#define PTP_DPC_CASIO_CS_UPPER_LIMIT	0xD010
#define PTP_DPC_CASIO_CS_SHOT		0xD011
#define PTP_DPC_CASIO_UNKNOWN_6		0xD012
#define PTP_DPC_CASIO_UNKNOWN_7		0xD013
#define PTP_DPC_CASIO_UNKNOWN_8		0xD015
#define PTP_DPC_CASIO_UNKNOWN_9		0xD017
#define PTP_DPC_CASIO_UNKNOWN_10	0xD018
#define PTP_DPC_CASIO_UNKNOWN_11	0xD019
#define PTP_DPC_CASIO_UNKNOWN_12	0xD01A
#define PTP_DPC_CASIO_UNKNOWN_13	0xD01B
#define PTP_DPC_CASIO_UNKNOWN_14	0xD01C
#define PTP_DPC_CASIO_UNKNOWN_15	0xD01D
#define PTP_DPC_CASIO_UNKNOWN_16	0xD020
#define PTP_DPC_CASIO_UNKNOWN_17	0xD030
#define PTP_DPC_CASIO_UNKNOWN_18	0xD080

/* Device Property Form Flag */

#define PTP_DPFF_None				0x00
#define PTP_DPFF_Range				0x01
#define PTP_DPFF_Enumeration		0x02

/* Device Property GetSet type */
#define PTP_DPGS_Get				0x00
#define PTP_DPGS_GetSet				0x01


/* PTP USB Bulk-Pipe container */
/* USB bulk max max packet length for high speed endpoints */
#define PTP_USB_BULK_HS_MAX_PACKET_LEN	512
#define PTP_USB_BULK_HDR_LEN			(2*sizeof(uint32)+2*sizeof(uint16))
#define PTP_USB_BULK_PAYLOAD_LEN		(PTP_USB_BULK_HS_MAX_PACKET_LEN-PTP_USB_BULK_HDR_LEN)
#define PTP_USB_BULK_REQ_LEN			(PTP_USB_BULK_HDR_LEN+5*sizeof(uint32))

/* Transaction data phase description */
#define PTP_DP_NODATA				0x0000	/* no data phase */
#define PTP_DP_SENDDATA				0x0001	/* sending data */
#define PTP_DP_GETDATA				0x0002	/* receiving data */
#define PTP_DP_DATA_MASK			0x00ff	/* data phase mask */

/* Number of PTP Request phase parameters */
#define PTP_RQ_PARAM0				0x0000	/* zero parameters */
#define PTP_RQ_PARAM1				0x0100	/* one parameter */
#define PTP_RQ_PARAM2				0x0200	/* two parameters */
#define PTP_RQ_PARAM3				0x0300	/* three parameters */
#define PTP_RQ_PARAM4				0x0400	/* four parameters */
#define PTP_RQ_PARAM5				0x0500	/* five parameters */

struct PTPContainer {
    uint16 Code;
    uint32 SessionID;
    uint32 TransactionID;
    /* params  may be of any type of size less or equal to uint32_t */
    uint32 Param1;
    uint32 Param2;
    uint32 Param3;
    /* events can only have three parameters */
    uint32 Param4;
    uint32 Param5;
    /* the number of meaningfull parameters */
    uint8 Nparam;
};

/* USB container types */
enum PTPContainerType {
    PTP_USB_CONTAINER_UNDEFINED = 0x0000,
    PTP_USB_CONTAINER_COMMAND = 0x0001,
    PTP_USB_CONTAINER_DATA = 0x0002,
    PTP_USB_CONTAINER_RESPONSE = 0x0003,
    PTP_USB_CONTAINER_EVENT = 0x0004
};

struct PTPUSBBulkContainer {
    uint32 Length;
    uint16 Type;
    uint16 Code;
    uint32 TransactionID;

    union {
        struct {
            uint32 Param1;
            uint32 Param2;
            uint32 Param3;
            uint32 Param4;
            uint32 Param5;
        } params;
        unsigned char data[PTP_USB_BULK_PAYLOAD_LEN];
    } payload;
} __attribute__((packed));

/* PTP USB Asynchronous Event Interrupt Data Format */
struct PTPUSBEventItem {
    uint16 Type;
    uint16 Code;
    uint32 TransactionID;
    uint32 Param1;
    uint32 Param2;
    uint32 Param3;
}  __attribute__((packed));


struct PTPUSBEventsContainer {
    uint32 Length;
    PTPUSBEventItem event[1];
};

struct PTPUSBEventContainerRaw {
    uint32 Length __attribute__((packed));
    uint16 Type __attribute__((packed));
    uint16 Code __attribute__((packed));
    uint32 TransactionID __attribute__((packed));
    uint32 Param1 __attribute__((packed));
    uint32 Param2 __attribute__((packed));
    uint32 Param3 __attribute__((packed));
} __attribute__((packed));

struct PTPUSBEventNikon {
    uint16 Code __attribute__((packed));
    uint32 Param1 __attribute__((packed));
} __attribute__((packed));

struct PTPUSBEventContainerNikonRaw {
    uint16 Length __attribute__((packed));
    PTPUSBEventNikon event[1];
} __attribute__((packed));

/* PTP device info structure (returned by GetDevInfo) */
struct _PTPDeviceInfoRaw {
    uint16 StandardVersion __attribute__((packed));
    uint32 VendorExtensionID __attribute__((packed));
    uint16 VendorExtensionVersion __attribute__((packed));
    uint8 VendorExtensionDescLen;
    char *VendorExtensionDesc;
    uint16 FunctionalMode;
    uint32 OperationsSupported_len;
    uint16 *OperationsSupported;
    uint32 EventsSupported_len;
    uint16 *EventsSupported;
    uint32 DevicePropertiesSupported_len;
    uint16 *DevicePropertiesSupported;
    uint32 CaptureFormats_len;
    uint16 *CaptureFormats;
    uint32 ImageFormats_len;
    uint16 *ImageFormats;
    char *Manufacturer;
    char *Model;
    char *DeviceVersion;
    char *SerialNumber;
};
typedef struct _PTPDeviceInfoRaw PTPDeviceInfoRaw;

/* PTP storageIDs structure (returned by GetStorageIDs) */
struct PTPStorageIDs {
    uint32 n;
    uint32 *Storage;
};

/* PTP StorageInfo structure (returned by GetStorageInfo) */
struct PTPStorageInfo {
    uint16 StorageType __attribute__((packed));
    uint16 FilesystemType __attribute__((packed));
    uint16 AccessCapability __attribute__((packed));
    uint64 MaxCapability __attribute__((packed));
    uint64 FreeSpaceInBytes __attribute__((packed));
    uint32 FreeSpaceInImages __attribute__((packed));
    char *StorageDescription;
    char *VolumeLabel;
};

/* PTP object handles structure (returned by GetObjectHandles) */
struct PTPObjectHandles {
    uint32 n;
    uint32 *Handler;
};

#define PTP_HANDLER_SPECIAL	0xffffffff
#define PTP_HANDLER_ROOT	0x00000000

/* PTP object info structure (returned by GetObjectInfo) */
struct PTPObjectInfo {
    uint32 StorageID;
    uint16 ObjectFormat;
    uint16 ProtectionStatus;
    uint32 ObjectCompressedSize;
    uint16 ThumbFormat;
    uint32 ThumbCompressedSize;
    uint32 ThumbPixWidth;
    uint32 ThumbPixHeight;
    uint32 ImagePixWidth;
    uint32 ImagePixHeight;
    uint32 ImageBitDepth;
    uint32 ParentObject;
    uint16 AssociationType;
    uint32 AssociationDesc;
    uint32 SequenceNumber;
    char *Filename;
    time_t CaptureDate;
    time_t ModificationDate;
    char *Keywords;
};

struct _PTPObjectInfoRaw {
    uint32 StorageID __attribute__((packed));
    uint16 ObjectFormat __attribute__((packed));
    uint16 ProtectionStatus __attribute__((packed));
    uint32 ObjectCompressedSize __attribute__((packed));
    uint16 ThumbFormat __attribute__((packed));
    uint32 ThumbCompressedSize __attribute__((packed));
    uint32 ThumbPixWidth __attribute__((packed));
    uint32 ThumbPixHeight __attribute__((packed));
    uint32 ImagePixWidth __attribute__((packed));
    uint32 ImagePixHeight __attribute__((packed));
    uint32 ImageBitDepth __attribute__((packed));
    uint32 ParentObject __attribute__((packed));
    uint16 AssociationType __attribute__((packed));
    uint32 AssociationDesc __attribute__((packed));
    uint32 SequenceNumber __attribute__((packed));
    char StringList[10];
} __attribute__((packed));
typedef _PTPObjectInfoRaw PTPObjectInfoRaw;


#endif	/* PTPDEFINITIONS_H */

