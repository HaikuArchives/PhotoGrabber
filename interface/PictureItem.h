/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/ 
//
// File defenition
#ifndef PICITEM_H
#define PICITEM_H

 //		Includes
#include <ListView.h>
#include <Bitmap.h>
#include <SupportKit.h>

 //		define classes
//
class BeCam_PictureItem : public BListItem 
{
	public:
		BeCam_PictureItem(char *name, uint32 attrs, uint32 size, uint32 date, BBitmap *bitmap, uint32 Xres, uint32 Yres);
		BeCam_PictureItem(void)
			{
			PictureBitmap=0;
			};
		~BeCam_PictureItem(void);
		virtual void DrawItem(BView *owner, BRect frame, bool complete = false);
		virtual void Update(BView *owner, const BFont *font);
		void Rotate(int angle);
		
		const char *GetName(void)
					{
					return(PictureName.String());
					}
		bool Deletable()
				{
				return(!(PictureAttrs & 1));
				}

void BeCam_PictureItem::setvals(char *name, uint32 attrs, uint32 size, uint32 date, BBitmap *bitmap, uint32 Xres, uint32 Yres)
{
	PictureName=name;
	PictureBitmap=bitmap;
	PictureAttrs=attrs;
	PictureSize=size;
	PictureDate=date;
	PictureXres=Xres;
	PictureYres=Yres;
}

void BeCam_PictureItem::set_Bitmap(BBitmap *bitmap)
{
	if(PictureBitmap)
		{
		delete PictureBitmap;
		}

	PictureBitmap=bitmap;
}

void BeCam_PictureItem::set_Name(char *name)
{
	PictureName=name;
}

void BeCam_PictureItem::set_Attrs(uint32 attrs)
{
	PictureAttrs=attrs;
}

void BeCam_PictureItem::set_Date(uint32 date)
{
	PictureDate=date;
}

void BeCam_PictureItem::set_Size(uint32 size)
{
	PictureSize=size;
}

void BeCam_PictureItem::set_Res(uint32 Xres, uint32 Yres)
{
	PictureXres=Xres;
	PictureYres=Yres;
}

void BeCam_PictureItem::set_XRes(uint32 Xres)
{
	PictureXres=Xres;
}

void BeCam_PictureItem::set_YRes(uint32 Yres)
{
	PictureYres=Yres;
}

void BeCam_PictureItem::set_Orientation(uint32 orientation)
{
	PictureOrientation=orientation;
}

	uint32		PictureAttrs;
	uint32		PictureDate;
	uint32		PictureSize;
	uint32		PictureXres;
	uint32		PictureYres;
	uint32		PictureFlash;
	uint32		PictureOrientation;

	private: 
		BString		PictureName;
		BBitmap		*PictureBitmap;
}; 

int PicturecompareFunc(const BeCam_PictureItem **one, const BeCam_PictureItem **two);

#endif
