/*****************************************************************
 * Copyright (c) 2004-2008,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
// 
//		Includes
#include <stdio.h>
//		Local includes

#include "Item.h"
#include "debug.h"

FILE *lfitem;

//	Item

BeCam_Item::BeCam_Item(ItemData *data) : BListItem()
{
	itemdata = data;
	fHeight = 160;
	fWidth = 120;
}

//
//		Item destructor

BeCam_Item::BeCam_Item() : BListItem()
{
	// nothing yet
}

//
//		Item destructor

BeCam_Item::~BeCam_Item()
{
	//nothing yet
}

//
//		Item::DrawItem

void BeCam_Item::DrawItem(BView *owner, BRect frame, bool complete) 
{
	#ifdef DEBUG
		lfitem = fopen(INTF_LOGFILE,"a");	
		fprintf(lfitem,"ITEM - Draw Item\n");
		fclose(lfitem);
	#endif
	rgb_color color;
	rgb_color color_selected = {0xee, 0xc9, 0x00, 0xff};
	rgb_color color_enabled = {0x00, 0x00, 0x00, 0xff};
	rgb_color color_disabled = {0x77, 0x00, 0x00, 0xff};
	rgb_color color_picture = {0x8b, 0x8b, 0x83, 0xff};
	rgb_color color_background = owner->ViewColor();
	BRect thumbRect;
	BRect itemRect = BRect(0,0,(frame.right - frame.left),(frame.bottom - frame.top));
	BBitmap* thumbnail = GetThumbBitmap();
	BBitmap *itemBitmap;
	BView *itemView;
	
	itemBitmap = new BBitmap(itemRect,B_RGB_32_BIT,TRUE);
	itemView = new BView(itemBitmap->Bounds(),"itemRect",B_FOLLOW_ALL,B_WILL_DRAW);
	itemBitmap->AddChild(itemView);
	float xItemBitmapPos = (frame.right - frame.left)/2;
	float yItemBitmapPos = (frame.bottom - frame.top)/2;
	
	
	float xPos = frame.left + (frame.right - frame.left)/2;
	float yPos = frame.top + (frame.bottom - frame.top)/2;
	
	if(thumbnail)
	{
		thumbRect=thumbnail->Bounds();
		float thumbWidth = thumbRect.right - thumbRect.left;
		float thumbHeight = thumbRect.bottom - thumbRect.top;
		int8 radius = 3;
		
		BRect selectionRect;
		
		owner->SetDrawingMode( B_OP_ALPHA );
		itemBitmap->Lock();
		if(complete)
		{	
			itemView->SetHighColor(owner->ViewColor());
			itemView->SetLowColor(owner->ViewColor());
			itemView->FillRect (itemView->Bounds());
			itemView->StrokeRect (itemView->Bounds());
		}
		if (IsSelected())
		{
			selectionRect.left = xItemBitmapPos - (thumbWidth/2) - 3;
			selectionRect.right = xItemBitmapPos + (thumbWidth/2) + 3;
			selectionRect.top = yItemBitmapPos - (thumbHeight/2) - 2;
			selectionRect.bottom = yItemBitmapPos + (thumbHeight/2) + 4;
			itemView->SetHighColor (color_selected);
			itemView->SetLowColor (color_selected);
		}
		else
		{
			selectionRect.left = xItemBitmapPos - (thumbWidth/2) - 2;
			selectionRect.right = xItemBitmapPos + (thumbWidth/2) + 2;
			selectionRect.top = yItemBitmapPos - (thumbHeight/2) - 1;
			selectionRect.bottom = yItemBitmapPos + (thumbHeight/2) + 3;
			itemView->SetHighColor (color_picture);
			itemView->SetLowColor (color_picture);
		}
		
		itemView->FillRoundRect (selectionRect, radius, radius);
		itemView->StrokeRoundRect (selectionRect, radius, radius);
		
		if (IsEnabled())
			itemView->SetHighColor(color_enabled);
		else
			itemView->SetHighColor(color_disabled);
		
		
		// Move the pen to draw
		itemView->MovePenTo(xItemBitmapPos -(thumbWidth/2),yItemBitmapPos -(thumbHeight/2));
		if(thumbRect.right < thumbRect.bottom)
			thumbRect.right = thumbRect.bottom;
		thumbRect.left = xItemBitmapPos - (thumbWidth/2);
		thumbRect.right = xItemBitmapPos + (thumbWidth/2);
		thumbRect.top = yItemBitmapPos - (thumbHeight/2);
		thumbRect.bottom = yItemBitmapPos + (thumbHeight/2);
		//	Draw the thumbnail
		itemView->DrawBitmapAsync(thumbnail,thumbRect);
		itemView->Sync();
		itemBitmap->Unlock();
		
		owner->DrawBitmapAsync(itemBitmap,frame);
		owner->Sync();
		owner->SetDrawingMode( B_OP_COPY );
	}
	else
	{
		// Move the pen to draw
		owner->MovePenTo(frame.left,frame.top);
		
		thumbRect.left=0;
		thumbRect.right=159;
		thumbRect.top=0;
		thumbRect.bottom=119;
		//	Draw a string	
		owner->DrawString("No thumbnail");
	}
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
	return fHeight;
}
//
//	Item::Width
float BeCam_Item::Width() const
{
	return fWidth;
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
