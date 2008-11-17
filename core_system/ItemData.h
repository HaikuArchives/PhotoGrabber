/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// File defenition
#ifndef ITEMDATA_H
#define ITEMDATA_H

#include <Bitmap.h>
#include <String.h>

typedef struct  {
	uint32		ItemHandle;
	uint32		ItemAttrs;
	uint32		ItemStatus;
	BString		ItemDate;
	uint32		ItemSize;
	uint32		ItemXres;
	uint32		ItemYres;
	uint32		ItemOrientation; 
	BString		ItemName;
	BBitmap		*ItemThumbBitmap;
	uint32		ItemThumbXres;
	uint32		ItemThumbYres;
	char		*ItemPath;
}ItemData;

#endif

