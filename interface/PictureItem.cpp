/*****************************************************************
 * Copyright (c) 2004-2005,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
//
//		Includes
#include <stdio.h>
//		Local includes

#include "PictureItem.h"

//		pic_Item

BeCam_PictureItem::BeCam_PictureItem(char *name, uint32 attrs, uint32 size, uint32 date, BBitmap *bitmap, uint32 Xres, uint32 Yres) : BListItem()
{
	PictureName=name;
	PictureBitmap=bitmap;
	PictureAttrs=attrs;
	PictureSize=size;
	PictureDate=date;
	PictureXres=Xres;
	PictureYres=Yres;
	
}


//		pic_Item destructor

BeCam_PictureItem::~BeCam_PictureItem(void)
{
if(PictureBitmap)
	{
	delete PictureBitmap;
	}
}


//		pic_Item::DrawItem

void BeCam_PictureItem::DrawItem(BView *owner, BRect frame, bool complete) 
{
	rgb_color color;
	rgb_color k_select={0xee, 0xee, 0xff, 0xff};
	rgb_color kt_ena={0xff, 0x00, 0x00, 0xff};
	rgb_color kt_dis={0x77, 0x00, 0x00, 0xff};
	rgb_color kt_bg=owner->ViewColor();
	BRect	rect;
	font_height height;
	float	fheight;
	char	tmps[40];

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

	if(PictureBitmap)
	{
		rect=PictureBitmap->Bounds();

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

	owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight));
	owner->DrawString(PictureName.String());
	
	//char *orient[10]={"", "(Landscape)", "", "", "", "", "(Portrait)", "(Landscape)", "(Portrait)", ""};

	if((PictureOrientation==6) || (PictureOrientation==8))
	{
		sprintf(tmps, "%ld*%ld pixels %ld bytes.", PictureYres, PictureXres, PictureSize);
	}
	else
	{
		sprintf(tmps, "%ld*%ld pixels %ld bytes.", PictureXres, PictureYres, PictureSize);
	}
	owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*2));
	owner->DrawString(tmps);

	//sprintf(tmps, "%s %s", pic_Flash?"(Flash)":"(No flash)", orient[pic_Orientation<10?pic_Orientation:0]);
	//owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*3));
	//owner->DrawString(tmps);

	sprintf(tmps, "%s", asctime(gmtime((const time_t *)&PictureDate)));
	tmps[strlen(tmps)-1]=0;
	owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*4));
	owner->DrawString(tmps);

	sprintf(tmps, "%s", (PictureAttrs&32)?"New picture":"Downloaded");
	owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*5));
	owner->DrawString(tmps);

	sprintf(tmps, "%s", (PictureAttrs&1)?"Delete protected.":"");
	owner->MovePenTo(frame.left+rect.right+10, frame.top+(fheight*6));
	owner->DrawString(tmps);

	owner->MovePenTo(frame.left+4, frame.top+4);
	if(PictureBitmap)
	{
		owner->DrawBitmap(PictureBitmap);
	}
	else
	{
		owner->MovePenTo(frame.left+10+40, frame.top+(60+(fheight/2)));
		owner->DrawString("No thumbnail");
	}

}


//	pic_Item::Update

void BeCam_PictureItem::Update(BView *owner, const BFont *font)
{
	BRect	rect;

	if(PictureBitmap)
	{
		rect=PictureBitmap->Bounds();
		SetHeight((rect.bottom)+8);
	}
	else
	{
		SetHeight((119)+8);
	}
}


//		pic_compareFunc

int PicturecompareFunc(const BeCam_PictureItem **one, const BeCam_PictureItem **two)
{
	if((*one)->PictureDate < (*two)->PictureDate)
	{
		return(1);
	}
	else
	if((*one)->PictureDate > (*two)->PictureDate)
	{
		return(-1);
	}
	else
	{
		return(0);
	}
}


//		pic_Item::Rotate

void BeCam_PictureItem::Rotate(int angle)
{
	BRect 			tmprect, bounds;
	BBitmap			*tmpbitmap;
	unsigned char	*srcbits, *dstbits;
	uint32			srow, drow, scol, dcol;
	uint32			swidth, dwidth, sheight, dheight;
	uint32			sbprow, dbprow, sbppix, dbppix;

	if(PictureBitmap)
	{
		tmprect=PictureBitmap->Bounds();
	}
	else
	{
		return;
	}
	bounds.left=tmprect.left;
	bounds.right=tmprect.bottom;//max(tmprect.bottom, tmprect.right);
	bounds.top=tmprect.top;
	bounds.bottom=tmprect.right;//max(tmprect.right, tmprect.bottom);

	swidth=(uint32)tmprect.right;
	sheight=(uint32)tmprect.bottom;
	dwidth=(uint32)bounds.right;
	dheight=(uint32)bounds.bottom;

		
	if(!(tmpbitmap = new BBitmap(bounds, PictureBitmap->ColorSpace())))
	{
		return;
	}

	srcbits=(unsigned char *)PictureBitmap->Bits();
	dstbits=(unsigned char *)tmpbitmap->Bits();
	sbprow=PictureBitmap->BytesPerRow();
	dbprow=tmpbitmap->BytesPerRow();
		
	// assume that bytes per pixel is 4 (RGBA) - should check against colorspace
	dbppix=4;
	sbppix=4;

	// flip to the right
	if(angle==90)
	{
		for(srow=0, dcol=dwidth; srow<=sheight; srow++, dcol--)
		{
			for(scol=0, drow=0; scol<=swidth; scol++, drow++)
			{
				// should loop for [bytes per pixel] iterations instead of hardcoded to 4 bytes
				dstbits[(dbprow*drow)+(dcol*dbppix)+0]=srcbits[(sbprow*srow)+(scol*sbppix)+0];
				dstbits[(dbprow*drow)+(dcol*dbppix)+1]=srcbits[(sbprow*srow)+(scol*sbppix)+1];
				dstbits[(dbprow*drow)+(dcol*dbppix)+2]=srcbits[(sbprow*srow)+(scol*sbppix)+2];
				dstbits[(dbprow*drow)+(dcol*dbppix)+3]=srcbits[(sbprow*srow)+(scol*sbppix)+3];
			}
		}
	}

	// flip to the left
	if(angle==270)
	{
		for(srow=0, dcol=0; srow<=sheight; srow++, dcol++)
		{
			for(scol=0, drow=dheight; scol<=swidth; scol++, drow--)
			{
				// should loop for [bytes per pixel] iterations instead of hardcoded to 4 bytes
				dstbits[(dbprow*drow)+(dcol*dbppix)+0]=srcbits[(sbprow*srow)+(scol*sbppix)+0];
				dstbits[(dbprow*drow)+(dcol*dbppix)+1]=srcbits[(sbprow*srow)+(scol*sbppix)+1];
				dstbits[(dbprow*drow)+(dcol*dbppix)+2]=srcbits[(sbprow*srow)+(scol*sbppix)+2];
				dstbits[(dbprow*drow)+(dcol*dbppix)+3]=srcbits[(sbprow*srow)+(scol*sbppix)+3];
			}
		}
	}

	// done flipping, make the change
	{
		BRect	rect;

		set_Bitmap(tmpbitmap);
		rect=PictureBitmap->Bounds();
		SetHeight((rect.bottom)+8);
	}
}
