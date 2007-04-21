/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
 
//
// File defenition
#ifndef  MSDSCSIPROBE_H
#define MSDSCSIPROBE_H
//
// Includes
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <Directory.h>
#include <Entry.h>
#include <Path.h>
#include <scsi.h>
#include <CAM.h>
#ifdef __ZETA__
	#include <usb/USBKit.h>
#else
	#include "USBKit.h"
#endif
//
// Local Includes
#include "global.h"

char *SCSIProbeMount(const char *device_path, BUSBDevice *usbdev);
bool SCSIUnmount(char *usbDeviceName);
char *SCSIMount(const char *path,BUSBDevice *device);

#endif
