/*****************************************************************
 * Copyright (c) 2004-2007,	PhotoGrabber-team					 *
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
#include "debug.h"
//
//	Includes
#include <stdio.h>
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
	// Nothing yet
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
	fMsg = BMessenger(this);
#ifdef _ZETA_OS_
		
	fMsg.SendDelayedMessage( BMessage(skDelayMessage), 30000 );			
#else
	fMsgRun = new BMessageRunner(fMsg,new BMessage(skDelayMessage),150000);
#endif
}
//
//	Animation :: AnimationStep
void Animation::AnimationStep()
{
	if( false == IsHidden() )
	{
		++fAnimationPos;
		// reset animation
		if( fAnimationPos > fAnimationFrames.size() - 1 )
		{
			fAnimationPos = 0;
		}
		// invalidate only the rectangle where we move the transfer icon
		Invalidate();
	#ifdef _ZETA_OS_	
		fMsg.SendDelayedMessage( BMessage( skDelayMessage ), 150000 );	
	#endif
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
#ifdef _ZETA_OS_	
	SetDoubleBuffering( B_UPDATE_INVALIDATED );
#endif	
	bool done = false;
	#ifdef DEBUG
	lfanimation = fopen(INTF_LOGFILE,"a");	
	fprintf(lfanimation,"ANIMATION - Begin loading images\n");
	fclose(lfanimation);
	#endif
	// load images for the animation
	for( size_t i = 0; false == done; ++i )
	{
		BString lImgName = fImgFrontName;
		lImgName << i << fImgExtension;
	#ifdef DEBUG
		lfanimation = fopen(INTF_LOGFILE,"a");	
		fprintf(lfanimation,"ANIMATION - image: %s\n",lImgName.String());
		fclose(lfanimation);
	#endif
		BBitmap* aniframe = BTranslationUtils::GetBitmap('PNG ', lImgName.String());
		if( NULL != aniframe )
			fAnimationFrames.push_back(aniframe);	
		else
			done = true;
	}
	#ifdef DEBUG
	lfanimation = fopen(INTF_LOGFILE,"a");	
	fprintf(lfanimation,"ANIMATION - images loaded\n");
	fclose(lfanimation);
	#endif
	fDrawPos.x = ( Bounds().Width() - fAnimationFrames[0]->Bounds().Width() ) / 2; 
	fDrawPos.y = ( Bounds().Height() - fAnimationFrames[0]->Bounds().Height() ) / 2;
	#ifdef DEBUG
	lfanimation = fopen(INTF_LOGFILE,"a");	
	fprintf(lfanimation,"ANIMATION - Attached to window\n");
	fclose(lfanimation);
	#endif
}
//
//	Animation :: Draw
void Animation::Draw(BRect _frame)
{
	BView::Draw(_frame);
	// DEBUGGING
	//BRect r = fAnimationFrames[fAnimationPos]->Bounds();
	//BBitmap *offscreen = new BBitmap(r,B_RGB_32_BIT,TRUE);
	//BView *offview = new BView(r,"off",B_FOLLOW_ALL,B_WILL_DRAW);
	//offview->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	//offscreen->AddChild(offview);
	//offscreen->Lock();
	//r = offview->Bounds();
	//offview->DrawBitmap(fAnimationFrames[fAnimationPos],fDrawPos);
	//offview->Sync();
	//offscreen->Unlock();
	///
	SetDrawingMode( B_OP_ALPHA );
	DrawBitmap( fAnimationFrames[fAnimationPos]/*offscreen*/, fDrawPos );
	SetDrawingMode( B_OP_COPY );
	///
	
	//delete(offscreen);
	//				
}
