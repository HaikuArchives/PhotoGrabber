/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
//
//		File defenition
#ifndef	SETTINGS_H
#define SETTINGS_H
//
//		Includes
#include <String.h>
//
// Structure
typedef struct {
 bool debugTerminal;    // Debug to the terminal
 bool debugFile;        // Debug to a file
 char defaultDownloadPath[B_FILE_NAME_LENGTH]; // Default path to download the pictures
 char deviceName[B_FILE_NAME_LENGTH];      // Camera name
 char pluginName[B_FILE_NAME_LENGTH];      // Plugin name 
} SETTINGS;

#endif
