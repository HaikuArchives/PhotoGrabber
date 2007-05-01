/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/ 
//
// File defenition
#ifndef ITEM_H
#define ITEM_H

//		Includes
#include <ListView.h>
//
//		Local Includes
#include "intf_global.h"
#include "core_global.h"
#include "ItemData.h"

 //		define classes
//
class BeCam_Item : public BListItem 
{
	public:
						BeCam_Item(ItemData *data);
						BeCam_Item();
						~BeCam_Item();
		virtual void 	DrawItem(BView *owner, BRect frame, bool complete = false);
		virtual void 	Update(BView *owner, const BFont *font);
		void 			Rotate(int angle);
		const char* 	GetName();
		const char*		GetDate();
		uint32			GetHandle();
		uint32			GetSize();
		BBitmap*		GetThumbBitmap();
		char*			GetItemPath();
		uint32			GetStatus();
		uint32			GetOrientation();
		
	private:
		ItemData				*itemdata;
};

#endif


