/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
// 
//		Includes
#include <stdio.h>
//		Local includes

#include "Item.h"

//	Item

BeCam_Item::BeCam_Item(ItemData *data) : BListItem()
{
	itemdata = data;
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
	rgb_color color;
	rgb_color k_select=ui_color(B_MENU_SELECTION_BACKGROUND_COLOR);
	rgb_color kt_ena={0x00, 0x00, 0x00, 0xff};
	rgb_color kt_dis={0x77, 0x00, 0x00, 0xff};
	rgb_color kt_bg=owner->ViewColor();
	BRect	rect;
	font_height height;
	float	fheight;
#ifdef _ZETA_OS_
	char	tmps[40];
	BParamable tmpString = NULL;
#else	
	char tmpString[256];
#endif
	owner->GetFontHeight(&height);
	fheight=(height.ascent+height.descent);

	if (IsSelected() || complete)
	{
		if(IsSelected())
		{
			color = k_select;
		}
		else
		{
			color =  kt_bg;
		}
		owner->SetHighColor(color);
		owner->SetLowColor(color);
		owner->FillRect(frame);
	}

	if (IsEnabled())
	{
		owner->SetHighColor(kt_ena);
	}
	else
	{
		owner->SetHighColor(kt_dis);
	}

	if(GetThumbBitmap())
	{
		rect=GetThumbBitmap()->Bounds();

		if(rect.right<rect.bottom)
		{
			rect.right=rect.bottom;
		}
	}
	else
	{
		rect.left=0;
		rect.right=159;
		rect.top=0;
		rect.bottom=119;
	}

#ifdef _ZETA_OS_	
	tmpString = _TPS("Name: NAME").Replace("NAME",GetName());
#else	
	sprintf(tmpString,"Name: %s",GetName());
#endif	
	owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight));
#ifdef _ZETA_OS_	
	owner->DrawString(tmpString.String());
#else	
	owner->DrawString(tmpString);
#endif
	if((GetOrientation()==6) || (GetOrientation()==8))
	{
	#ifdef _ZETA_OS_	
		tmpString = _TPS("Height: PIXELS pixels.").Replace("PIXELS",BFormatter("%ld",itemdata->ItemYres));	
		owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*3));
		owner->DrawString(tmpString.String());
		tmpString = _TPS("Width: PIXELS pixels.").Replace("PIXELS",BFormatter("%ld",itemdata->ItemXres));
		owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*4));
		owner->DrawString(tmpString.String());
	#else	
		sprintf(tmpString,"Height: %ld pixels.",itemdata->ItemYres);
		owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*3));
		owner->DrawString(tmpString);
		sprintf(tmpString,"Width: %ld pixels.",itemdata->ItemXres);
		owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*4));
		owner->DrawString(tmpString);
	#endif	
		
	}
	else
	{
	#ifdef _ZETA_OS_
		tmpString = _TPS("Height: PIXELS pixels.").Replace("PIXELS",BFormatter("%ld",itemdata->ItemYres));
		owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*3));
		owner->DrawString(tmpString.String());
		tmpString = _TPS("Width: PIXELS pixels.").Replace("PIXELS",BFormatter("%ld",itemdata->ItemXres));
		owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*4));
		owner->DrawString(tmpString.String());
	#else
		sprintf(tmpString,"Height: %ld pixels.",itemdata->ItemYres);
		owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*3));
		owner->DrawString(tmpString);
		sprintf(tmpString,"Width: %ld pixels.",itemdata->ItemXres);		
		owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*4));
		owner->DrawString(tmpString);
	#endif
	}
#ifdef _ZETA_OS_
	tmpString = _TPS("Size: BYTES bytes.").Replace("BYTES",BFormatter("%ld",GetSize()));
	owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*5));
	owner->DrawString(tmpString.String());
	tmpString = _TPS("Date: DATE").Replace("DATE",GetDate());
	owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*6));
	owner->DrawString(tmpString.String());
#else
	sprintf(tmpString,"Size: %ld bytes.",GetSize());
	owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*5));
	owner->DrawString(tmpString);
	sprintf(tmpString,"Date: %s",GetDate());
	owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*6));
	owner->DrawString(tmpString);
#endif
	owner->MovePenTo(frame.left+4, frame.top+4);
	if(GetThumbBitmap())
	{
		owner->DrawBitmap(GetThumbBitmap());
	}
	else
	{
		owner->MovePenTo(frame.left+10+40, frame.top+(60+(fheight/2)));
	#ifdef _ZETA_OS_	
		owner->DrawString(_T("No thumbnail"));
	#else	
		owner->DrawString("No thumbnail");
	#endif
	}

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
