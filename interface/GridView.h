/*
****************************************************************
* Copyright (c) 2004-2008,	Ramshankar, Jan-Rixt Van Hoye	   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// File defenition
#ifndef GRIDVIEW_H
#define GRIDVIEW_H
//
// Includes
#include <interface/View.h>
#include <interface/ScrollView.h>
#include <interface/Region.h>
#include <interface/Control.h>
//
// Local Includes
#include "Item.h"
//
// Defines
#define ITEM_NEXT	'INXT'
#define ITEM_BACK	'IBAK'
#define ITEM_SORT_BY_TITLE	0
#define ITEM_SORT_BY_DATE	1
//
// Classes
class GridView;
class BMessenger;
//
//	Struct 
struct list_tracking_data 
{
	GridView	*view;
	BPoint		start;
};

class GridView : public BControl
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
		void					AddList(BList& listOfGridItemPtrs);
		BRect					ItemRect(int32 index);
		void					Select(int32 index, bool extend = false);
		void					Select(int32 fromIndex, int32 toIndex, bool extend = false);
		void 					SelectAll();
		void					SelectNext(int32 modifiers = 0);
		void					SelectPrevious(int32 modifiers = 0);
		void					Deselect(int32 index,bool extend = false);
		void					Deselect(int32 fromIndex, int32 toIndex);
		void 					DeselectAll();
		void					DeleteAllItems();
		void					ScrollToSelection();
		void					UpdateScrollView();
		int32					CurrentSelection(int32 index = 0);
		bool					IsItemSelected(int32 index);
		void					MakeEmpty();
		int						GetNumberOfSelectedItems();	
		BeCam_Item*		 		ItemAt(int32 index) const;
		BeCam_Item				*SelectedItem() const;
		int32					CountColumns() const;
		int32					CountColumnsWithMinHorizItemMargin () const;
		int32					CountRows() const;
		int32					CountItems() const;
		void					SetSelectionCurveRadius(uint8 radius);
		uint8					SelectionCurveRadius() const;
		void					SendKeyStrokesTo(BLooper* looper,BHandler* handler = NULL);
		void					SortItemsBy(int sortType);
				
	private:
		friend class			Item;
		
		void					DrawContent(BRect updateRect);
		float 					GetTop(int32 rowCount,int32 columnCount);
		float 					GetRowHeight(int32 rowCount,int32 columnCount);
		bool					HandleKeyMovement(const char* bytes, int32 numBytes);
		float					ItemHeight() const;
		float					ItemWidth() const;
		float					ItemHorizMargin() const;
		float					ItemVertMargin() const;
		void					SetHorizItemMargin(float margin);
		void					SetVertItemMargin(float margin);
		int32					IndexOf(BPoint point);
		static status_t 		TrackItem(list_tracking_data *data);
	 	void					ActionCopy(BMessage * request);
	 	float					CalculateHorizMargin(float gridWidth) const;
	 	static int				CompareTitles(const void* first, const void* second);
	 	static int				CompareDates(const void* first, const void* second);
		
		float					fItemHeight;	// height of item
		float					fItemWidth;		// width of item
		static float			fMinHorizItemMargin; // min horizontal margin
		static float			fMinVertItemMargin; // min vertical margin
		float					fHorizItemMargin;	// horizontal margin
		float					fVertItemMargin;	// vertical margin
		int32					fCachedColumnCount,fSelectedItemIndex,fLastSelectedItemIndex;
		int8					fSelectionRadius;
		BList*					fItemList;
		BeCam_Item*				fSelectedItem;
		BeCam_Item*				fLastSelectedItem;
		BScrollView*			fScrollView;
		BMessage*				fSelectMsg;
		BMessage*				fInvokeMsg;
		BMessenger*				fTargetMessenger;
		BLooper					*fKeyTargetLooper;
		BHandler				*fKeyTargetHandler;
};

#endif
