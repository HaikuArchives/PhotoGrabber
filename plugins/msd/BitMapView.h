/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
//
// File defenition
#ifndef THUMB_H
#define THUMB_H
//
// Includes
#include <Bitmap.h>
#include <Rect.h>
#include <View.h>


//const uint32 MY_DRAG_ACTION2 = 'drag';

class BitmapView : public BView
{
	public:
		BitmapView(BRect frame);
		virtual ~BitmapView();
	
		virtual void AttachedToWindow();
		virtual void Draw(BRect updateRect);
		virtual void MessageReceived(BMessage*);
		//virtual void KeyDown(const char *bytes, int32 numBytes);
		//virtual void MouseDown( BPoint where );
		void LoadBitmap(char *filename);
	
	protected:
		BBitmap *bitmap;
		BRect frame;
};

#endif
