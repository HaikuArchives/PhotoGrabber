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
#include <translation/TranslationUtils.h>
#include <interface/Window.h>
//
//	Globals
FILE *lfpiclabelbut;
//
//	PictureLabelButton :: Constructor
PictureLabelButton::PictureLabelButton(BRect rect, 
				const char* name,
				const char *label,
				BMessage *message, 
				uint32 resize,
				uint32 flags)
: BView (rect, name, resize, flags | B_FRAME_EVENTS)
{	
	buttonLabel = new BString(label);
	buttonMessage = message;
}
//
//	PictureLabelButton :: Destructor
PictureLabelButton::~PictureLabelButton()
{
	// Nothing yet
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
			BView::MessageReceived(message);	
			break;	
	}
}
//
//	PictureLabelButton :: AttachedToWindow()
void PictureLabelButton::AttachedToWindow()
{
	BView::AttachedToWindow();
	rgb_color color_background = {0x42, 0x42, 0x42, 0xff};
	//SetViewColor(color_background);
	SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ));	
	// load button picture
	buttonBitmap = BTranslationUtils::GetBitmap('PNG ', buttonLabel->String());
}
//
//	PictureLabelButton :: Draw
void PictureLabelButton::Draw(BRect frame)
{
	rgb_color color_border = {0x8b, 0x8b, 0x83, 0xff};
	rgb_color color_label = {0x00, 0x00, 0x00, 0xff};
	SetDrawingMode( B_OP_ALPHA );
	SetHighColor(color_border);
	SetLowColor(color_border);
	BFont font = be_plain_font;
	font.SetSize(11);
    font_height fontHeight;
    SetFont(&font);
	GetFontHeight (&fontHeight);
	float fFontHeight = fontHeight.ascent + fontHeight.descent;
	float fStringWidth = StringWidth(buttonLabel->String());
	BRect picButtonRect = Bounds();
	BRect picRect;
	picRect.left = picButtonRect.left + floor((picButtonRect.right - picButtonRect.left)/2) - 24;
	picRect.right = picRect.left + 48;
	picRect.top = picButtonRect.top + ceil((picButtonRect.bottom-picButtonRect.top - fFontHeight- 4)/2) -24;
	picRect.bottom = picButtonRect.bottom - fFontHeight - 4;
	if(buttonBitmap != NULL)
		DrawBitmap(buttonBitmap,picRect);
	else
		StrokeRoundRect(picRect,2,2);
	MovePenTo(floor((picButtonRect.right - picButtonRect.left)/2) - floor(fStringWidth/2),picButtonRect.bottom - 4);
	SetHighColor(color_label);
	SetLowColor(color_label);
	DrawString(buttonLabel->String());
	SetDrawingMode( B_OP_COPY );
	BView::Draw(picButtonRect);				
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
	if (!Window() || Window()->IsActive() == false)
		return;
	
	BMessage* msg = Window()->CurrentMessage();
	int32 clicks = msg->FindInt32 ("clicks");
	int32 button = msg->FindInt32 ("buttons");
	static BPoint previousPoint = point;
	static int32 lastButton = -1;
	static int32 clickCount = 1;

	if ((button == lastButton) && (clicks > 1))
		clickCount++;
	else
		clickCount = 1;

	lastButton = button;
	
	if (clickCount == 1 && button == B_PRIMARY_MOUSE_BUTTON)
    {
    	Window()->PostMessage(buttonMessage); 
	}
	previousPoint = point;
	return BView::MouseDown (point);
}

//
//	PictureLabelButton :: MouseUp
void PictureLabelButton::MouseUp (BPoint point)
{
	return BView::MouseUp (point);
}
