/*****************************************************************
 * Copyright (c) 2004-2008,	PhotoGrabber-team, Jan-Rixt Van Hoye *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/

//
//	Local includes
#include "ActionDock.h"
#include "debug.h"
//
// Includes
#include <interface/Window.h>
#include <TranslationUtils.h>
#include <Bitmap.h>
//
//	Globals
FILE *lfactionbar;
//
//	ActionDock :: Constructor
ActionDock::ActionDock(BRect rect, const char* name, uint32 resize,uint32 flags)
: BView (rect, name, resize, flags | B_FRAME_EVENTS)
{	
	BFont font = be_plain_font;
	//font.SetFace(B_BOLD_FACE);
	font.SetSize(11);
    font_height fontHeight;
    SetFont(&font);
	//
	float picLabelHeight = Bounds().Height();
	float picLabelWidth = 0;
	float margin = 15;
	BRect r = Bounds();
	BBitmap *onBitmap = NULL;
	BBitmap *offBitmap = NULL;
	float labelWidth = 0;
	float bitmapWidth = 0;
	// Create download button
	if(picLabelHeight >= 75)
	{
		onBitmap = BTranslationUtils::GetBitmap('PNG ', "download_64_on");
		offBitmap = BTranslationUtils::GetBitmap('PNG ', "download_64_off");
	}
	else
	{
		onBitmap = BTranslationUtils::GetBitmap('PNG ', "download_48_on");
		offBitmap = BTranslationUtils::GetBitmap('PNG ', "download_48_off");
	}
	labelWidth = StringWidth("Download");
	if(onBitmap != NULL)
		bitmapWidth = onBitmap->Bounds().Width();
	else
		bitmapWidth = PLB_MIN_BITMAP_WIDTH;
	picLabelWidth = labelWidth >= bitmapWidth?labelWidth:bitmapWidth;
	r.left = rect.left + margin;
	r.top = r.top + 1;
	r.right = r.left + picLabelWidth;
	r.bottom = r.top + picLabelHeight;
	downloadButton = new PictureLabelButton(r, "download","Download",onBitmap,offBitmap,new BMessage(DOWN_BUTTON),B_FOLLOW_LEFT|B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW);
	AddChild(downloadButton);
	// Create delete button
	if(picLabelHeight >= 75)
	{
		onBitmap = BTranslationUtils::GetBitmap('PNG ', "delete_64_on");
		offBitmap = BTranslationUtils::GetBitmap('PNG ', "delete_64_off");
	}
	else
	{
		onBitmap = BTranslationUtils::GetBitmap('PNG ', "delete_48_on");
		offBitmap = BTranslationUtils::GetBitmap('PNG ', "delete_48_off");
	}
	labelWidth = StringWidth("Delete");
	if(onBitmap != NULL)
		bitmapWidth = onBitmap->Bounds().Width();
	else
		bitmapWidth = PLB_MIN_BITMAP_WIDTH;
	picLabelWidth = labelWidth >= bitmapWidth?labelWidth:bitmapWidth;
	r.left = r.right + margin;
	r.right = r.left + picLabelWidth;
	deleteButton = new PictureLabelButton(r, "delete","Delete",onBitmap,offBitmap,new BMessage(DEL_BUTTON), B_FOLLOW_LEFT|B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW);
	AddChild(deleteButton);
	// Create next button
	if(picLabelHeight >= 75)
	{
		onBitmap = BTranslationUtils::GetBitmap('PNG ', "next_64_on");
		offBitmap = BTranslationUtils::GetBitmap('PNG ', "next_64_off");
	}
	else
	{
		onBitmap = BTranslationUtils::GetBitmap('PNG ', "next_48_on");
		offBitmap = BTranslationUtils::GetBitmap('PNG ', "next_48_off");
	}
	labelWidth = StringWidth("Next");
	if(onBitmap != NULL)
		bitmapWidth = onBitmap->Bounds().Width();
	else
		bitmapWidth = PLB_MIN_BITMAP_WIDTH;
	picLabelWidth = labelWidth >= bitmapWidth?labelWidth:bitmapWidth;
	r.right = Bounds().right - margin;
	r.left = r.right - picLabelWidth;
	nextButton = new PictureLabelButton(r, "next","Next",onBitmap,offBitmap,new BMessage(NEXT_BUTTON), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW);
	AddChild(nextButton);
	// Create previous button
	if(picLabelHeight >= 75)
	{
		onBitmap = BTranslationUtils::GetBitmap('PNG ', "back_64_on");
		offBitmap = BTranslationUtils::GetBitmap('PNG ', "back_64_off");
	}
	else
	{
		onBitmap = BTranslationUtils::GetBitmap('PNG ', "back_48_on");
		offBitmap = BTranslationUtils::GetBitmap('PNG ', "back_48_off");
	}
	labelWidth = StringWidth("Delete");
	if(onBitmap != NULL)
		bitmapWidth = onBitmap->Bounds().Width();
	else
		bitmapWidth = PLB_MIN_BITMAP_WIDTH;
	picLabelWidth = labelWidth >= bitmapWidth?labelWidth:bitmapWidth;
	r.right = Bounds().right - 2 * margin - picLabelWidth;
	r.left = r.right - picLabelWidth;
	previousButton = new PictureLabelButton(r, "previous","Previous",onBitmap,offBitmap,new BMessage(PREVIOUS_BUTTON), B_FOLLOW_RIGHT|B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW);
	AddChild(previousButton);
	// Create the select view
	selectDirPanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), NULL, B_DIRECTORY_NODE, false,new BMessage(OPEN_FILE_PANEL), NULL, false, true);
}
//
//	ActionDock :: Destructor
ActionDock::~ActionDock()
{
	delete(selectDirPanel);
}
//						
// ActionDock :: MessageReceived
void ActionDock::MessageReceived(BMessage* message)
{
	#ifdef DEBUG
		lfactionbar = fopen(INTF_LOGFILE,"a");	
		fprintf(lfactionbar,"ANIMATION - Delay message received\n");
		fclose(lfactionbar);
	#endif
	
	switch(message->what)
	{
		case OPEN_FILE_PANEL:
		{
			entry_ref dir;
			message->FindRef("refs", &dir);
			BEntry entry(&dir);
			BPath path(&entry);
			if(path.Path() != NULL)
			{
				// Set the path in the text field
			}
			break; 
		}
		case SELECT_PATHMENU:
		{
			selectDirPanel->SetButtonLabel(B_DEFAULT_BUTTON,_T("Select"));	
			selectDirPanel->Window()->SetWorkspaces(B_CURRENT_WORKSPACE);
			selectDirPanel->Show();
			break;
		}
		case DOWN_BUTTON:
			break;
		default :
			BView::MessageReceived(message);	
			break;	
	}
}
//
//	ActionDock :: AttachedToWindow()
void ActionDock::AttachedToWindow()
{
	#ifdef DEBUG
		lfactionbar = fopen(INTF_LOGFILE,"a");	
		fprintf(lfactionbar,"ACTIONBAR - Attached to window\n");
		fclose(lfactionbar);
	#endif
	BView::AttachedToWindow();
	SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );	
}
//
//	ActionDock :: Draw
void ActionDock::Draw(BRect frame)
{
	rgb_color color_border = {0x8b, 0x8b, 0x83, 0xff};
	//rgb_color color_border = {0x00, 0x00, 0x00, 0xff};
	BRect actionBarFrame = Bounds();
	BPoint startPoint,endPoint;
	startPoint.x = actionBarFrame.left;
	startPoint.y = actionBarFrame.top;
	endPoint.x = actionBarFrame.right;
	endPoint.y = actionBarFrame.top;
	SetDrawingMode( B_OP_ALPHA );
	SetHighColor(color_border);
	SetLowColor(color_border);
	SetPenSize(1);
	StrokeLine(startPoint,endPoint);
	SetDrawingMode( B_OP_COPY );
	BView::Draw(BRect(actionBarFrame.left,actionBarFrame.top,actionBarFrame.right,1));					
}
//
// ActionDock :: FrameResized
void ActionDock::FrameResized (float newWidth, float newHeight)
{
	//Draw (BRect (0, 0, newWidth, newHeight));	
	//return BView::FrameResized (newWidth, newHeight);
}
//
// ActionDock :: Hide
void ActionDock::Hide()
{	
	if (!Window() || Window()->IsActive() == false)
	{
		float height = Bounds().Height();
		BMessage *hideMessage = new BMessage(HIDE_ACTIONDOCK);
		hideMessage->AddFloat("height",height);
		Window()->PostMessage(hideMessage); 
	}
	BView::Hide();	
}
//
// ActionDock :: Show
void ActionDock::Show()
{
	if (!Window() || Window()->IsActive() == false)
	{
		float height = Bounds().bottom - Bounds().top;
		BMessage *showMessage = new BMessage(SHOW_ACTIONDOCK);
		showMessage->AddFloat("height",height);
		Window()->PostMessage(showMessage); 
	}
	BView::Show();
	
}
