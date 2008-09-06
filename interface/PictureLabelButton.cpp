/*****************************************************************
 * Copyright (c) 2004-2008,	PhotoGrabber-team, Jan-Rixt Van Hoye *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/

//
//	Local includes
#include "PictureLabelButton.h"
//
//	Includes
#include <TranslationUtils.h>
#include <Window.h>
#include <Bitmap.h>
//
//	Globals
FILE *lfpiclabelbut;
//
//	PictureLabelButton :: Constructor
PictureLabelButton::PictureLabelButton(BRect rect, 
				const char* name,
				const char *label,
				BBitmap *bitmapOn,
				BBitmap *bitmapOff,
				BMessage *message, 
				uint32 resize,
				uint32 flags)
: BControl (rect, name, label, message,resize, flags)
{	
	buttonBitmapOn = bitmapOn;
	buttonBitmapOff = bitmapOff;
}
//
//	PictureLabelButton :: Destructor
PictureLabelButton::~PictureLabelButton()
{
	delete(buttonBitmapOn);
	delete(buttonBitmapOff);
}
//						
// PictureLabelButton :: MessageReceived
void PictureLabelButton::MessageReceived(BMessage* message)
{
	#ifdef DEBUG
		lfpiclabelbut = fopen(INTF_LOGFILE,"a");	
		fprintf(lfpiclabelbut,"ANIMATION - Delay message received\n");
		fclose(lfpiclabelbut);
	#endif
	
	switch(message->what)
	{
		default :
			BControl::MessageReceived(message);	
			break;	
	}
}
//
//	PictureLabelButton :: AttachedToWindow()
void PictureLabelButton::AttachedToWindow()
{
	BControl::AttachedToWindow();	
	// low color will now be the parents view color
	SetViewColor(B_TRANSPARENT_COLOR);
}
//
//	PictureLabelButton :: KeyDown()
void PictureLabelButton::KeyDown(const char *bytes, int32 numBytes)
{
	if (*bytes == B_ENTER || *bytes == B_SPACE) 
	{
		if (!IsEnabled())
			return;
		SetValue(B_CONTROL_ON);
		// make sure the user saw that
		Window()->UpdateIfNeeded();
		snooze(25000);
		Invoke();

	} 
	else
		BControl::KeyDown(bytes, numBytes);
}
//
//	PictureLabelButton :: Draw
void PictureLabelButton::Draw(BRect frame)
{
	rgb_color color_border = {0x8b, 0x8b, 0x83, 0xff};
	rgb_color color_label_disabled = color_border;
	rgb_color color_label_enabled = {0x00, 0x00, 0x00, 0xff};
	rgb_color color_background= ui_color(B_PANEL_BACKGROUND_COLOR);
	//
	BString text = Label();
	//
	SetDrawingMode(B_OP_ALPHA);
	SetHighColor(color_background);
	SetLowColor(color_background);
	FillRect (Bounds());
	SetHighColor(color_border);
	SetLowColor(color_border);
	float picHeight = PLB_MIN_BITMAP_HEIGHT;
	float picWidth = PLB_MIN_BITMAP_WIDTH;
	if(buttonBitmapOn != NULL)
	{
		picHeight = buttonBitmapOn->Bounds().Height();
		picWidth = buttonBitmapOn->Bounds().Width();
	}
	int margin = 6;
	BFont font = be_plain_font;
	font.SetSize(11);
    font_height fontHeight;
    SetFont(&font);
	GetFontHeight (&fontHeight);
	float fFontHeight = 0;
	float fStringWidth = 0;
	if(text.String() != NULL)
	{
		fFontHeight = fontHeight.ascent + fontHeight.descent;
		fStringWidth = StringWidth(text.String());
	}
	BRect picButtonRect = Bounds();
	BRect picRect;
	picRect.left = picButtonRect.left + floor((picButtonRect.right - picButtonRect.left)/2) - picWidth/2;
	picRect.right = picRect.left + picWidth;
	picRect.top = picButtonRect.top + ceil((picButtonRect.bottom-picButtonRect.top - fFontHeight- margin)/2) -picHeight/2;
	picRect.bottom = picRect.top + picHeight;
	if(IsEnabled())
	{
		if(buttonBitmapOn != NULL )
			DrawBitmap(buttonBitmapOn,picRect);
		else
			StrokeRoundRect(picRect,2,2);
	}
	else
	{
		if(buttonBitmapOff != NULL )
			DrawBitmap(buttonBitmapOff,picRect);
		else
			StrokeRoundRect(picRect,2,2);
	}
	if(text.String() != NULL)
	{
		MovePenTo(floor((picButtonRect.right - picButtonRect.left)/2) - floor(fStringWidth/2),picButtonRect.bottom - margin);
		if(IsEnabled())
		{
			SetHighColor(color_label_enabled);
			SetLowColor(color_label_enabled);
		}
		else
		{
			SetHighColor(color_label_disabled);
			SetLowColor(color_label_disabled);
		}
		DrawString(text.String());
	}
	SetDrawingMode( B_OP_COPY );			
}
//
//	PictureLabelButton :: Set Target
void PictureLabelButton::SetTarget (BHandler* target)
{
	buttonTargetHandler = target;
}
//
//	PictureLabelButton :: MouseDown
void PictureLabelButton::MouseDown (BPoint point)
{
	#ifdef DEBUG
		lfpiclabelbut = fopen(INTF_LOGFILE,"a");	
		fprintf(lfpiclabelbut,"PICTURELABELBUTTON - Mouse Down\n");
		fclose(lfpiclabelbut);
	#endif
	if (!IsEnabled())
		return;
	SetValue(B_CONTROL_ON);
	if (Window()->Flags() & B_ASYNCHRONOUS_CONTROLS) 
	{
 		SetTracking(true);
 		SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS);
 	} 
 	else 
 	{
		BRect bounds = Bounds();
		uint32 buttons;

		do 
		{
			Window()->UpdateIfNeeded();
			snooze(40000);
			GetMouse(&point, &buttons, true);
 			bool inside = bounds.Contains(point);
			if ((Value() == B_CONTROL_ON) != inside)
				SetValue(inside ? B_CONTROL_ON : B_CONTROL_OFF);
		} 
		while (buttons != 0);

		if (Value() == B_CONTROL_ON)
			Invoke();
	}
}

//
//	PictureLabelButton :: MouseUp
void PictureLabelButton::MouseUp (BPoint point)
{
	//return BView::MouseUp (point);
	if (!IsTracking())
		return;

	if (Bounds().Contains(point))
		Invoke();
	
	SetTracking(false);
}
