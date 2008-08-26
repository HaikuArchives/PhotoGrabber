/*****************************************************************
* Copyright (c) 2004-2008,	Ramshankar, Jan-Rixt Van Hoye		 *
* All rights reserved.											 *
* Distributed under the terms of the MIT License.                *
 *****************************************************************/
//
// File defenition
#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include <interface/View.h>
#include <interface/ScrollView.h>
#include <interface/Region.h>

#include "Item.h"

class GridView;
class BMessenger;
//
//	Struct 
struct list_tracking_data 
{
	GridView	*view;
	BPoint		start;
};

class GridView : public BView
{
	public:
								GridView (BRect rect, const char* name, 
									uint32 resizeMask = B_FOLLOW_ALL,
									uint32 flags = B_WILL_DRAW);
		virtual					~GridView ();

		virtual void			FrameResized (float newWidth, float newHeight);
		virtual void			Draw (BRect rect);
		virtual void			KeyDown (const char* bytes, int32 numBytes);
		virtual void			MouseUp (BPoint pt);
		virtual void			MouseDown (BPoint pt);
		virtual void			TargetedByScrollView (BScrollView* scrollView);
		virtual void			AttachedToWindow ();

		void					SetTarget (BMessenger& messenger);
		void					AddItem (BeCam_Item* item);
		void					RemoveItem(BeCam_Item* item);
		void					AddItemFast (BeCam_Item* item);
		void					RemoveItemFast (BeCam_Item* item);
		void					AddList (BList& listOfGridItemPtrs);
		BRect					ItemRect (int32 index);
		void					Select (int32 index);
		void					Select (int32 fromIndex, int32 toIndex);
		void 					SelectAll ();
		void					Deselect (int32 index);
		void					Deselect (int32 fromIndex, int32 toIndex);
		void 					DeselectAll ();
		void					DeleteAllItems ();
		void					ScrollToSelection ();
		void					UpdateScrollView();
		int32					CurrentSelection(int32 index = 0);
		bool					IsItemSelected(int32 index);
		void					MakeEmpty();
		int						GetNumberOfSelectedItems();
		
		BeCam_Item*		 		ItemAt (int32 index) const;
		BeCam_Item				*SelectedItem () const;

		int32					CountColumns () const;
		int32					CountRows () const;
		int32					CountItems() const;
		void					SetSelectionCurveRadius (uint8 radius);
		uint8					SelectionCurveRadius () const;
		void					SendKeyStrokesTo (BLooper* looper,
										BHandler* handler = NULL);
				
	private:
		friend class			Item;
		
		void					DrawContent (BRect updateRect);
		bool					HandleKeyMovement (const char* bytes, int32 numBytes);
		float					ItemHeight () const;
		float					ItemWidth () const;
		float					ItemMargin () const;
		int32					IndexOf(BPoint point) const;
		static status_t 		TrackItem(list_tracking_data *data);
	 	void					ActionCopy(BMessage * request);
		
		static float			fItemHeight;	// max height of bmp
		static float			fItemWidth;		// max width of bmp
		static float			fItemMargin;	// Margin
		
		int32					fCachedColumnCount,
								fSelectedItemIndex;
		int8					fSelectionRadius;
		BList*					fItemList;
		BeCam_Item*				fSelectedItem;
		BScrollView*			fScrollView;
		BMessage*				fSelectMsg;
		BMessage*				fInvokeMsg;
		BMessenger*				fTargetMessenger;
		BLooper					*fKeyTargetLooper;
		BHandler				*fKeyTargetHandler;
};

#endif
