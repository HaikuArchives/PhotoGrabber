/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/

//
// File defenition
#ifndef GLOBAL_H
#define GLOBAL_H

//includes
#include <File.h>
#include <NodeInfo.h>


// defenitions

#define DEBUG_SCREEN			0
#define DEBUG_LOGFILE			1
#define USBBULK_BUFFER			512

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

#define APP_SIG           			 "application/x-vnd.BeDiGiCam"

// *** log-file path ***

#define LOGFILE 					"/boot/var/tmp/BeCam.log"
#define VERSIONNAME					"PhotoGrabber version"
#define VERSIONNUMBER				"2.0"
#define AUTHOR						"By Jan-Rixt Van Hoye aka Jixt (jixt@bebug.be)"
#define DESCRIPTION					"Digital camera download utility."

// Global Messages

#define UPDATE_STAT					'UPST'
#define ABOUT						'ABOU'
#define QUIT						'QUIT'
#define CONFIG						'CONF'
#define CAM_CONNECTED				'CCON'
#define CAM_DISCONNECTED			'DCON'
#define	CAM_CONNECT					'CONN'
#define	CAM_CONNECT_OK				'CONO'
#define CAM_CONNECT_FAIL			'CNFA'
#define	CAM_DISCON					'DISC'
#define CON_MENU					'CONM'
#define DISCON_MENU					'DCNM'
#define ADD_ITEM					'ADDI'
#define REM_ITEM					'REMI'
#define REM_ITEM_OK					'RITO'
#define DOWN_ITEM					'DOWI'
#define DOWN_ITEM_OK				'DITO'
#define GET_ITEMS					'GITM'
#define ADD_PTPCAM					'ADDP'
#define REM_PTPCAM					'REMP'
#define ADD_MSCAM					'ADDM'
#define REM_MSCAM					'REMM'
#define OPEN_FILE_PANEL				'OFPA'
#define	SAVE_CONFIGURATION			'SCON'
#define RELOAD_CONFIGURATION        'RCON'
#define GET_CONFIGURATION			'GCON'
#define	CANCEL						'CANC'
#define CHECK_TERMINAL				'TCHE'
#define CHECK_FILE					'CFIL'
#define CHECK_PTP					'CPTP'
#define CHECK_BDCP					'CBDC'
#define CHECK_MOVIE					'CMOV'
#define SELECT_PATH					'SPAT'
#define	SELECT_PATHMENU				'SPME'
#define	DRAG_PICTURE				'DPIC'
#define GET_DEVTYPE					'DTPY'
#define GET_CAMSTRING				'GCST'
#define GET_PLUG_DETAIL				'GPDT'
#define OPEN_PLUG_WINDOW			'OPWI'
#define SHOW_MAINWINDOW				'SMWI'
//
//		Item status
#define	ITEM_NOT_DOWNLOADED			0
#define	ITEM_DOWNLOADED				1						

#define connected					2
#define disconnected				1

// Type of camera
//#define CAM_PTP						"ptop"
//#define CAM_BDCP					"bdcp"
//#define CAM_MASS					"mass"
#define	TYPE_PAR					0
#define TYPE_USB					1

#endif
