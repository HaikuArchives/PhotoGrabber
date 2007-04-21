/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
 
#ifndef _PREF_H_
#define _PREF_H_

#include <String.h>

extern int globalEndpoint;

void LoadSettingsFromFile(void);
void SaveSettingsToFile(void);

#endif
