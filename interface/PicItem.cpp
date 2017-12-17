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

#include "PicItem.h"
#include "logger.h"


//	Item

PicItem::PicItem(ItemData *data) 
  : itemdata(data)
{
}

//
//		Item

PicItem::PicItem() 
  : itemdata(NULL)
{
}

//
//		Item destructor

PicItem::~PicItem() {
    if (itemdata) {
        if (itemdata->ItemThumbBitmap) 
            delete itemdata->ItemThumbBitmap;
        delete(itemdata);
    }
}
//
//	Item::Get Name
const char* PicItem::Name()
{
	return itemdata->ItemName.String();
}
//
//	Item::Get Thumb Bitmap
BBitmap* PicItem::ThumbBitmap()
{
	return itemdata->ItemThumbBitmap;
}
//
//	Item::Get Item
char* PicItem::ItemPath()
{
	return itemdata->ItemPath;
}
//
//	Item::Get Date
const char* PicItem::Date()
{
	return itemdata->ItemDate.String();
}
//
//	Item::Get Name
uint32 PicItem::Handle()
{
	return itemdata->ItemHandle;
}
//
//	Item::Get Size
uint32 PicItem::Size()
{
	return itemdata->ItemSize;
}
//
//	Item::Get Orientation
uint32 PicItem::Orientation()
{
	return itemdata->ItemOrientation;
}
//
//	Item::Height
float PicItem::Height() const
{
	return itemdata->ItemYres ;
}
//
//	Item::Width
float PicItem::Width() const
{
	return itemdata->ItemXres;
}
//
//	Item::ThumbHeight
float PicItem::ThumbHeight() const
{
	return itemdata->ItemThumbYres;
}
//
//	Item::ThumbWidth
float PicItem::ThumbWidth() const
{
	return itemdata->ItemThumbXres;
}

//
//	Item:: RotateThumb
void 
PicItem::RotateThumb(uint32 angle) {
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
