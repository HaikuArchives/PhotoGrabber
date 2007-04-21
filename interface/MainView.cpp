/*****************************************************************
 * Copyright (c) 2004-2005,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
//  
//	Includes
#include "MainView.h"

//
//	MainView:: Constructor
BeCam_MainView::BeCam_MainView(BRect rect, char *name)
	   	   : BView(rect, name, B_FOLLOW_ALL_SIDES, 0)
{

}
//
//	MainView:: Attached to the window
void BeCam_MainView::AttachedToWindow() 
{
	SetFont(be_bold_font);
	SetFontSize(24);
}
//
// 	MainView:: Draw
void BeCam_MainView::Draw(BRect)
{

}
//
//	MainView:: MouseDown
void BeCam_MainView::MouseDown(BPoint where)
{
	
}
