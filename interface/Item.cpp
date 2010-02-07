/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//		Includes
#include <stdio.h>
//		Local includes

#include "Item.h"
#include "debug.h"


//	Item

BeCam_Item::BeCam_Item(ItemData *data, void(*debugfunction)(const char *,...)) : BListItem()
{
	Debug = (*debugfunction);
	itemdata = data;
	fThumbDetailsGap = 5.0f;
	fFontHeight = 12.0f;
}

//
//		Item

BeCam_Item::BeCam_Item() : BListItem()
{
	// nothing yet
}

//
//		Item destructor

BeCam_Item::~BeCam_Item()
{
	Debug("ITEM - Delete Item\n");
	
	if(itemdata)
	{
		if(itemdata->ItemThumbBitmap)
		{
			Debug("ITEM - Free thumb bitmap\n");
			
			free(itemdata->ItemThumbBitmap);
			itemdata->ItemThumbBitmap = NULL;
		}
		delete(itemdata);
		itemdata = NULL;
	}
}

//
//		Item::DrawItem

void BeCam_Item::DrawItem(BView *owner, BRect frame, bool complete) 
{
	Debug("ITEM - Draw Item\n");
	
	rgb_color color;
	rgb_color color_selected = {0xee, 0xc9, 0x00, 0xff};
	rgb_color color_enabled = {0x00, 0x00, 0x00, 0xff};
	rgb_color color_disabled = {0x77, 0x00, 0x00, 0xff};
	rgb_color color_picture = {0x8b, 0x8b, 0x83, 0xff};
	rgb_color color_background = owner->ViewColor();
	int8 radius = 3;
	BRect selectionRect;
	BRect thumbRect;
	BRect itemRect = BRect(0,0,(frame.right - frame.left),(frame.bottom - frame.top));
	BBitmap* thumbnail = GetThumbBitmap();
	float thumbWidth = ThumbWidth();
	float thumbHeight = ThumbHeight();
	BFont font = be_plain_font;
	font_height fontHeight;
	
	BBitmap *itemBitmap = new BBitmap(itemRect,B_RGB_32_BIT,TRUE);
	BView *itemView = new BView(itemBitmap->Bounds(),"itemRect",B_FOLLOW_ALL,B_WILL_DRAW);
	itemBitmap->AddChild(itemView);
	
	float xItemBitmapPos = floor((frame.right - frame.left)/2);
	float yItemBitmapPos = floor((frame.bottom - frame.top)/2);
	
	float xPos = frame.left + floor((frame.right - frame.left)/2);
	float yPos = frame.top + floor((frame.bottom - frame.top)/2);
	
	// Begin drawing	
	owner->SetDrawingMode( B_OP_ALPHA );
	itemBitmap->Lock();
	if(complete)
	{	
		itemView->SetHighColor(owner->ViewColor());
		itemView->SetLowColor(owner->ViewColor());
		itemView->FillRect (itemView->Bounds());
		itemView->StrokeRect (itemView->Bounds());
	}
	
	itemView->FillRoundRect (selectionRect, radius, radius);
	itemView->StrokeRoundRect (selectionRect, radius, radius);
	
	if (IsEnabled())
		itemView->SetHighColor(color_enabled);
	else
		itemView->SetHighColor(color_disabled);
	
	// Text Font
	font.SetFace(B_BOLD_FACE);
    font.SetSize(fFontHeight);
    itemView->SetFont(&font);
	itemView->GetFontHeight (&fontHeight);
	//fFontHeight = fontHeight.ascent + fontHeight.descent - 4;
		
	if(thumbnail)
	{	
		// There is a thumbnail, so draw it.
		thumbRect = thumbnail->Bounds();
		if(thumbRect.right < thumbRect.bottom)
			thumbRect.right = thumbRect.bottom;
		thumbRect.left = xItemBitmapPos - floor(thumbWidth/2);
		thumbRect.right = xItemBitmapPos + floor(thumbWidth/2);
		thumbRect.top = yItemBitmapPos - floor(thumbHeight/2);
		thumbRect.bottom = yItemBitmapPos + floor(thumbHeight/2);
		//
		itemView->DrawBitmapAsync(thumbnail,thumbRect);
		thumbRect.right += 1; // bug Haiku    
		float fStringWidth = itemView->StringWidth(itemdata->ItemName.String());
		itemView->MovePenTo(thumbRect.left,thumbRect.bottom + fThumbDetailsGap + fFontHeight);
		//
		itemView->SetHighColor (color_picture);
		itemView->DrawString(itemdata->ItemName.String());
		
	}
	else
	{
		// There is no thumbnail, so draw the file name in the middle.
        float fStringWidth = itemView->StringWidth(itemdata->ItemName.String());
		itemView->MovePenTo(xItemBitmapPos - fStringWidth/2,yItemBitmapPos + fFontHeight/2);
		if(thumbRect.right < thumbRect.bottom)
			thumbRect.right = thumbRect.bottom;
		thumbRect.left = xItemBitmapPos - floor(thumbWidth/2);
		thumbRect.right = xItemBitmapPos + floor(thumbWidth/2);
		thumbRect.top = yItemBitmapPos - floor(thumbHeight/2);
		thumbRect.bottom = yItemBitmapPos + floor(thumbHeight/2);
		//
		itemView->SetHighColor (color_picture);
		itemView->DrawString(itemdata->ItemName.String());
	}	
	
	if(!IsSelected())
	{
		itemView->SetHighColor (color_picture);
		itemView->SetLowColor (color_picture);
	}
	else
	{
		itemView->SetPenSize(3);
		itemView->SetHighColor (color_selected);
		itemView->SetLowColor (color_selected);
	}
	
	itemView->StrokeRect (thumbRect);
	itemView->Sync();
	itemBitmap->Unlock();
	
	owner->DrawBitmapAsync(itemBitmap,frame);
	owner->Sync();
	owner->SetDrawingMode( B_OP_COPY );
	
	delete(itemBitmap);
	fRegion.Include(thumbRect);
}

//
//	Item::Update
void BeCam_Item::Update(BView *owner, const BFont *font)
{
	BRect	rect;

	if(GetThumbBitmap())
	{
		rect=GetThumbBitmap()->Bounds();
		SetHeight((rect.bottom)+8);
	}
	else
	{
		SetHeight((119)+8);
	}
}
//
//	Item::Get Name
const char* BeCam_Item::GetName()
{
	return itemdata->ItemName.String();
}
//
//	Item::Get Thumb Bitmap
BBitmap* BeCam_Item::GetThumbBitmap()
{
	return itemdata->ItemThumbBitmap;
}
//
//	Item::Get Item
char* BeCam_Item::GetItemPath()
{
	return itemdata->ItemPath;
}
//
//	Item::Get Date
const char* BeCam_Item::GetDate()
{
	return itemdata->ItemDate.String();
}
//
//	Item::Get Name
uint32 BeCam_Item::GetHandle()
{
	return itemdata->ItemHandle;
}
//
//	Item::Get Size
uint32 BeCam_Item::GetSize()
{
	return itemdata->ItemSize;
}
//
//	Item::Get Orientation
uint32 BeCam_Item::GetOrientation()
{
	return itemdata->ItemOrientation;
}
//
//	Item::Height
float BeCam_Item::Height() const
{
	return itemdata->ItemYres ;
}
//
//	Item::Width
float BeCam_Item::Width() const
{
	return itemdata->ItemXres;
}
//
//	Item::ThumbHeight
float BeCam_Item::ThumbHeight() const
{
	return itemdata->ItemThumbYres;
}
//
//	Item::ThumbWidth
float BeCam_Item::ThumbWidth() const
{
	return itemdata->ItemThumbXres;
}
//
//	Item::ThumbWidth
float BeCam_Item::DetailsHeight() const
{
	return fThumbDetailsGap + fFontHeight;
}
//
//	Item:: Is Clickable Frame
bool BeCam_Item::IsClickableFrame (BPoint point) const
{
	for (int32 i = 0; i < fRegion.CountRects(); i++)
		if (fRegion.RectAt(i).Contains (point))
			return true;
	
	return false;
}
//
//	Item:: RotateThumb
void BeCam_Item::RotateThumb(uint32 angle)
{
	BRect 			tmpRect, bounds;
	BBitmap			*tmpBitmap;
	unsigned char	*srcBits, *dstBits;
	uint32			sRow, dRow, sCol, dCol;
	uint32			sWidth, dWidth, sHeight, dHeight;
	uint32			sbpRow, dbpRow, sbpPix, dbpPix;

	if(itemdata->ItemThumbBitmap)
		tmpRect = itemdata->ItemThumbBitmap->Bounds();
	else
		return;
	
	bounds.left = tmpRect.left;
	bounds.right = tmpRect.bottom; 
	bounds.top = tmpRect.top;
	bounds.bottom = tmpRect.right; 

	sWidth = (uint32)tmpRect.right;
	sHeight = (uint32)tmpRect.bottom;
	dWidth = (uint32)bounds.right;
	dHeight = (uint32)bounds.bottom;

		
	if(!(tmpBitmap = new BBitmap(bounds, itemdata->ItemThumbBitmap->ColorSpace())))
		return;

	srcBits = (unsigned char *)itemdata->ItemThumbBitmap->Bits();
	dstBits =( unsigned char *)tmpBitmap->Bits();
	sbpRow = itemdata->ItemThumbBitmap->BytesPerRow();
	dbpRow = tmpBitmap->BytesPerRow();
		
	// Assume that bytes per pixel is 4 (RGBA) - should check against colorspace
	dbpPix = 4;
	sbpPix = 4;
	
	// Flip to the right
	if(angle == 90)
	{
		for(sRow=0, dCol = dWidth; sRow <= sHeight; sRow++, dCol--)
		{
			for(sCol = 0, dRow = 0; sCol <= sWidth; sCol++, dRow++)
			{
				// Should loop for [bytes per pixel] iterations instead of hardcoded to 4 bytes
				dstBits[(dbpRow * dRow) + (dCol * dbpPix) + 0] = srcBits[(sbpRow * sRow) + (sCol * sbpPix) + 0];
				dstBits[(dbpRow * dRow) + (dCol * dbpPix) + 1] = srcBits[(sbpRow * sRow) + (sCol * sbpPix) + 1];
				dstBits[(dbpRow * dRow) + (dCol * dbpPix) + 2] = srcBits[(sbpRow * sRow) + (sCol * sbpPix) + 2];
				dstBits[(dbpRow * dRow) + (dCol * dbpPix) + 3] = srcBits[(sbpRow * sRow) + (sCol * sbpPix) + 3];
			}
		}
	}

	// Flip to the left
	if(angle == 270)
	{
		for(sRow = 0, dCol = 0; sRow <= sHeight; sRow++, dCol++)
		{
			for(sCol = 0, dRow = dHeight; sCol <= sWidth; sCol++, dRow--)
			{
				// should loop for [bytes per pixel] iterations instead of hardcoded to 4 bytes
				dstBits[(dbpRow * dRow) + (dCol * dbpPix) + 0] = srcBits[(sbpRow * sRow) + (sCol * sbpPix) + 0];
				dstBits[(dbpRow * dRow) + (dCol * dbpPix) + 1] = srcBits[(sbpRow * sRow) + (sCol * sbpPix) + 1];
				dstBits[(dbpRow * dRow) + (dCol * dbpPix) + 2] = srcBits[(sbpRow * sRow) + (sCol * sbpPix) + 2];
				dstBits[(dbpRow * dRow) + (dCol * dbpPix) + 3] = srcBits[(sbpRow * sRow) + (sCol * sbpPix) + 3];
			}
		}
	}

	// Replace the old bitmap and change width and height
	delete(itemdata->ItemThumbBitmap);
	itemdata->ItemThumbBitmap = tmpBitmap;
	itemdata->ItemThumbXres = (uint32)itemdata->ItemThumbBitmap->Bounds().Width();
	itemdata->ItemThumbYres = (uint32)itemdata->ItemThumbBitmap->Bounds().Height();
	itemdata->ItemOrientation = angle;
}
