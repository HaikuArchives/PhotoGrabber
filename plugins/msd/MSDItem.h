/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/ 
 
// 
// File defenition
#ifndef MSDITEM_H
#define MSDITEM_H

 //		Includes
//#include <SupportKit.h>
//#include <Locale.h>
//#include <Formatter.h>
#include <String.h>

//
//		Local Includes
#include "global.h"

 //		define classes
//
class MSD_Item
{
	public:
							MSD_Item(const char *name, uint32 size, char *date, uint32 Xres, uint32 Yres);
							MSD_Item();
							~MSD_Item();
		const char* 		GetName();
		const char*			GetDate();
		uint32		 		GetHandle();
		uint32				GetSize();
		uint32				GetXRes();
		uint32				GetYRes();
		const char*			GetItemPath();
		void 				setHandle(uint32 handle);
		void				setItemPath(const char *path);
		void 				setName(const char *name);
		void 				setDate(char *date);
		void 				setSize(uint32 size);
		void 				setRes(uint32 Xres, uint32 Yres);
		void 				setXRes(uint32 Xres);
		void 				setYRes(uint32 Yres);

	private:
		uint32			MSDItemHandle;
		BString			MSDItemDate;
		uint32			MSDItemSize;
		uint32			MSDItemXres;
		uint32			MSDItemYres;
		BString			MSDItemName;
		BString			MSDItemPath;
};

#endif
