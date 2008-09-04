/*****************************************************************
 * Copyright (c) 2004-2008,	PhotoGrabber-team					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************
 *																 *																 *
 *  Description: A View that draws an animation		     		 *    	 								 *
 *																 *	
 *****************************************************************/
//
//	Local includes
#include "Animation.h"
//
//	Includes
#include <interface/Bitmap.h>
#include <interface/Window.h>
#include <translation/TranslationUtils.h>
//
//	Globals
FILE *lfanimation;
static const uint32 skDelayMessage = 'delm';
static const uint32	skSpacing = 5;
//
//	Animation :: Constructor
Animation::Animation(BRect _frame): BView( _frame, "Animation", B_FOLLOW_ALL, B_WILL_DRAW )
{	
	Hide();
}
//
//	Animation :: Destructor
Animation::~Animation()
{
	fAnimationFrames.clear();
}
//						
// Animation :: MessageReceived
void Animation::MessageReceived(BMessage* message)
{
	#ifdef DEBUG
	lfanimation = fopen(INTF_LOGFILE,"a");	
	fprintf(lfanimation,"ANIMATION - Delay message received\n");
	fclose(lfanimation);
	#endif
	
	switch(message->what)
	{
		case skDelayMessage:
		{
			AnimationStep();
			break;
		}
		default :
			BView::MessageReceived(message);	
			break;	
	}
}
//
//	Animation :: StartAnimation
void Animation::StartAnimation()
{
	#ifdef DEBUG
		lfanimation = fopen(INTF_LOGFILE,"a");	
		fprintf(lfanimation,"ANIMATION - Start animation\n");
		fclose(lfanimation);
	#endif
	Show();
	fAnimationPos = 0;
	fMsg = new BMessenger(this);
	fMsgRun = new BMessageRunner(*fMsg,new BMessage(skDelayMessage),150000);
}
//
//	Animation :: StopAnimation
void Animation::StopAnimation()
{
	#ifdef DEBUG
		lfanimation = fopen(INTF_LOGFILE,"a");	
		fprintf(lfanimation,"ANIMATION - Stop animation\n");
		fclose(lfanimation);
	#endif
	Hide();
	if(fMsgRun)
		delete(fMsgRun);
	if(fMsg->IsValid())
		delete(fMsg);
}
//
//	Animation :: AnimationStep
void Animation::AnimationStep()
{
	if(!IsHidden())
	{
		++fAnimationPos;
		// reset animation
		if( fAnimationPos > fAnimationFrames.size() - 1 )
		{
			fAnimationPos = 0;
		}
		// invalidate only the rectangle where we move the transfer icon
		Invalidate();
	}	
}
//
//	Animation :: Set the image front name
void Animation::SetImgFrontName(BString frontname)
{
	fImgFrontName = frontname;
}
//
//	Animation :: Set the image file extension
void Animation::SetImgExtension(BString extension)
{
	fImgExtension = extension;
}
//
//	Animation :: AttachedToWindow()
void Animation::AttachedToWindow()
{
	#ifdef DEBUG
		lfanimation = fopen(INTF_LOGFILE,"a");	
		fprintf(lfanimation,"ANIMATION - Attach to window\n");
		fclose(lfanimation);
	#endif
	BView::AttachedToWindow();
	SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );	
	bool done = false;
	// load images for the animation
	for( size_t i = 0; false == done; ++i )
	{
		BString lImgName = fImgFrontName;
		lImgName << i << fImgExtension;
		BBitmap* aniframe = BTranslationUtils::GetBitmap('PNG ', lImgName.String());
		if( NULL != aniframe )
			fAnimationFrames.push_back(aniframe);	
		else
			done = true;
	}
	fDrawPos.x = ( Bounds().Width() - fAnimationFrames[0]->Bounds().Width() ) / 2; 
	fDrawPos.y = ( Bounds().Height() - fAnimationFrames[0]->Bounds().Height() ) / 2;
	
}
//
//	Animation :: Draw
void Animation::Draw(BRect frame)
{
	BView::Draw(frame);
	SetDrawingMode( B_OP_ALPHA );
	SetHighColor(ViewColor());
	SetLowColor(ViewColor());
	FillRect(Bounds());
	DrawBitmap(fAnimationFrames[fAnimationPos], fDrawPos);
	SetDrawingMode( B_OP_COPY );				
}
