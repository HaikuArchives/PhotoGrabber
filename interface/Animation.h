/*****************************************************************
 * Copyright (c) 2004-2007,	PhotoGrabber-team					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************
 *																 *																 *
 *  Description: A View that draws an animation		     		 *   	 								 *
 *																 *	
 *****************************************************************/
//
//	Includes
#include <interface/View.h>
#include <app/Messenger.h>
#include <interface/Point.h>
#include <vector.h>
#include <String.h>
#include <app/MessageRunner.h>
//
//	Local includes
#include "intf_global.h"
#include "debug.h"
//
//
class BBitmap;
//
//	Class Animation
class Animation : public BView
{
	public:
						Animation( BRect _frame );
						~Animation();
		
		virtual	void	MessageReceived( BMessage* _msg );
		virtual	void	AttachedToWindow();		
		virtual	void	Draw( BRect _frame );
				void	StartAnimation();
				void	AnimationStep();
				void	SetImgFrontName(BString frontname);
				void	SetImgExtension(BString extension);
		
	private:		
	
		BMessenger			fMsg;
		BMessageRunner		*fMsgRun;
		size_t				fAnimationPos;
		BPoint				fDrawPos;
		vector<BBitmap*>	fAnimationFrames;
		BString				fImgFrontName;	
		BString				fImgExtension;	
};
