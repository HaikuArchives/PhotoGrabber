/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
 
//
// Includes
#include <View.h>
#include <Rect.h>
#include <Message.h>
#include <BitmapStream.h>
#include <File.h>
#include <TranslatorRoster.h>
#include <Bitmap.h>
//
// Local Includes
#include "BitMapView.h"
//
//	BitmapView :: Constructor
BitmapView::BitmapView(BRect frame) :
	BView(frame, "BitmapView", B_FOLLOW_LEFT , B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE)
{
	//bitmap =  bm;
	// The size of the thumbnail
	frame = frame;
}
//
//	BitmapView :: Destructor
BitmapView::~BitmapView()
{
	delete bitmap;
}
//
//	BitmapView :: LoadBitmap
void BitmapView::LoadBitmap(char *filename)
{ 
	BFile file(filename, B_READ_ONLY);
	BTranslatorRoster *roster = BTranslatorRoster::Default();
	BBitmapStream stream;
	BBitmap *result = NULL;
	if (roster->Translate(&file, NULL, NULL, &stream,B_TRANSLATOR_BITMAP) < B_OK)
	{
		return;
	}
	stream.DetachBitmap(&result);
	bitmap = result;
	return;
}
//
//	BitmapView :: AttachedToWindow
void BitmapView::AttachedToWindow()
{
	//nothing yet
}
//
//	BitmapView :: Draw
void BitmapView::Draw(BRect updateRect)
{
	DrawBitmap(bitmap, Bounds());
}
//
//	BitmapView :: KeyDown
/*void BitmapView::KeyDown(const char *bytes, int32 numBytes)
{
	//nothing yet
}*/
//
//	BitmapView :: MosueDown
/*void BitmapView::MouseDown( BPoint where )
{
	//nothing yet
}*/
//
//	BitmapView :: MessageReceived
void BitmapView::MessageReceived(BMessage *msg)
{
   switch(msg->what){
   default:
      BitmapView::MessageReceived(msg);
   }
}

