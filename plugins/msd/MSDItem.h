/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/ 
// 
// File defenition
#ifndef MSDITEM_H
#define MSDITEM_H

//
//		Includes
#include <String.h>

//
//		Local Includes
//#include "global.h"

 //		define classes
//
typedef struct {
		int32			ItemHandle;
		BString			ItemDate;
		uint32			ItemSize;
		uint32			ItemXres;
		uint32			ItemYres;
		BString			ItemName;
		BBitmap			*ItemThumbBitmap;
		uint32			ItemThumbXres;
		uint32			ItemThumbYres;
		BString			ItemPath;
}MSDItem;

#endif
