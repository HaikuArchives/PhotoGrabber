/*****************************************************************
 * Copyright (c) 2004-2008,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/

// include
#include <File.h>
#include <NodeInfo.h>
// *** Nikon Coolpix 2000 ***
#define VENDOR_ID_NIKON				0x04b0
#define PRODUCT_ID_COOLPIX2000_M	0x0301
#define PRODUCT_ID_COOLPIX2000_P	0x0302
// *** HP PS 318 ***
#define VENDOR_ID_HP				0x03f0
#define PRODUCT_ID_PS_318			0x6302
// *** SONY DSC P8
#define VENDOR_ID_SONY				0x054c
#define PRODUCT_ID_DSC_P8			0x004e
// *** KODAK EASYSHARE LS443
#define VENDOR_ID_KODAK				0x0000
#define PRODUCT_ID_LS443			0x0000

//Jan-Rixt *** Belkin usb 8-in-1 Media Reader & Writer
#define VENDOR_ID_BELKIN			0x55aa
#define PRODUCT_ID_BELKINREADER		0xb012

#define USB_CLASS_PTP	6
#define USB_CLASS_MASS	8

// Type camera
#define	TYPE_PAR		0
#define TYPE_USB		1

