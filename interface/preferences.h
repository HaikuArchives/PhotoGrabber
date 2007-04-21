/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
//
//		File defenition
#ifndef	PREFERENCES_H
#define PREFERENCES_H
//
// Includes
#include <String.h>
#include <storage/StorageDefs.h>
//
// Local includes
#include "settings.h"
//
//		Functions
SETTINGS LoadSettingsFromFile(void);
void SaveSettingsToFile(SETTINGS pgsettings);

#endif
