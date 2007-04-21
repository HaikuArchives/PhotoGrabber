/*****************************************************************
 * Copyright (c) 2004-2005,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
//
// File defenition
#ifndef LISTVIEW_H
#define LISTVIEW_H

//		Includes
#include <ScrollView.h>
#include <PopUpMenu.h>
//
// local Includes

#include "Item.h"
//
//	Struct 
struct list_tracking_data 
{
	BListView		*view;
	BPoint			start;
};
//
//		PTP wrapper class
class BeCam_ListView : public BListView
{
	public:
						BeCam_ListView(BRect r);
		virtual void	MessageReceived(BMessage* message);
		virtual void 	MouseDown(BPoint point);
				void	SelectAll();
				int		GetNumberOfSelectedItems();
	 private:	
	 	static status_t TrackItem(list_tracking_data *data);
	 	void			ActionCopy(BMessage * request);
	 	BeCam_Item		*selectedItem;
		
};
//
// ListView error messages
#define	LISTV_OFFSET				555
#define LISTV_DEV_NO_FIND			LISTV_OFFSET + 1

#endif

