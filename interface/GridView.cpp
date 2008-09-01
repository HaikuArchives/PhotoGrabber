/*****************************************************************
* Copyright (c) 2004-2008,	Ramshankar, Jan-Rixt Van Hoye		 *
* All rights reserved.											 *
* Distributed under the terms of the MIT License.                *
 *****************************************************************/

#include <app/Messenger.h>
#include <app/Looper.h>
#include <app/Handler.h>
#include <interface/Bitmap.h>
#include <interface/ScrollView.h>
#include <interface/Window.h>
#include <support/Debug.h>
#include <support/List.h>
#include <support/String.h>

#include "GridView.h"
#include "debug.h"
#include "core_global.h"

FILE *lfgridv;

#define kDRAG_SLOP		4

float GridView::fItemWidth	= 160;	// max width of a bitmap
float GridView::fItemHeight	= 120;	// max height of a bitmap
float GridView::fMinHorizItemMargin = 20;	// horizontal margin
float GridView::fMinVertItemMargin = 20;	// vertical margin
//
//	GridView :: Constructor
GridView::GridView (BRect rect, const char* name, uint32 resize,uint32 flags)
: BView (rect, name, resize, flags | B_FRAME_EVENTS)
	,fCachedColumnCount (-1)
	,fSelectedItemIndex (-1)
	,fSelectionRadius (4)
	,fScrollView (NULL)
	,fTargetMessenger (NULL)
	,fKeyTargetLooper (NULL)
	,fKeyTargetHandler (NULL)
{
	rgb_color color_background = ui_color(B_MENU_BACKGROUND_COLOR);
	SetViewColor(color_background);
	fItemList = new BList();
	fHorizItemMargin = fMinHorizItemMargin;
	fVertItemMargin = fMinVertItemMargin;
	fSelectedItemIndex = 0;
	fLastSelectedItemIndex = 0;
}

//
//	GridView :: Destructor
GridView::~GridView()
{
	DeleteAllItems ();
	delete fItemList;
	fItemList = NULL;
	
	if (fTargetMessenger)
	{
		delete fTargetMessenger;
		fTargetMessenger = NULL;
	}
}

//
//	GridView :: Set Target
void GridView::SetTarget (BMessenger& messenger)
{
	fTargetMessenger = new BMessenger (messenger);
}

//
//	GridView :: Add Item
void GridView::AddItem (BeCam_Item* item)
{
	#ifdef DEBUG
		lfgridv = fopen(INTF_LOGFILE,"a");	
		fprintf(lfgridv,"GRIDVIEW - Add Item\n");
		fclose(lfgridv);
	#endif
	AddItemFast (item);
	Invalidate();
	UpdateScrollView();
}

//
//	GridView :: Add Item Fast
inline void GridView::AddItemFast (BeCam_Item* item)
{
	fItemList->AddItem (reinterpret_cast<void*>(item));
}
//
//	GridView :: Remove Item
void GridView::RemoveItem(BeCam_Item* item)
{
	#ifdef DEBUG
		lfgridv = fopen(INTF_LOGFILE,"a");	
		fprintf(lfgridv,"GRIDVIEW - Remove Item\n");
		fclose(lfgridv);
	#endif
	RemoveItemFast (item);
	Invalidate();
	UpdateScrollView();
}
//
//	GridView :: Remove Item Fast
inline void GridView::RemoveItemFast (BeCam_Item* item)
{
	fItemList->RemoveItem (reinterpret_cast<void*>(item));
}
//
//	GridView :: AddList
void GridView::AddList (BList& list)
{
	int32 count = list.CountItems();
	for (int32 i = 0; i < count; i++)
	{
		AddItemFast ((BeCam_Item*)list.ItemAtFast(i));
		Invalidate();
	}
	fCachedColumnCount = -1;
	UpdateScrollView();
}

//
// GridView :: Draw
void GridView::Draw (BRect rect)
{
	#ifdef DEBUG
		lfgridv = fopen(INTF_LOGFILE,"a");	
		fprintf(lfgridv,"GRIDVIEW - Draw\n");
		fclose(lfgridv);
	#endif
	DrawContent (rect);
	return BView::Draw (rect);
}

//
// GridView :: DrawContent
void GridView::DrawContent (BRect /*_unused*/)
{

	#ifdef DEBUG
		lfgridv = fopen(INTF_LOGFILE,"a");	
		fprintf(lfgridv,"GRIDVIEW - Draw Content\n");
		fclose(lfgridv);
	#endif
	
	SetHighColor (ViewColor());
	if (fItemList == NULL || fItemList->CountItems() == 0)
	{
		FillRect (Bounds());
		return;
	}
	
	BRect bounds = Frame();
	//int32 columnCount = CountColumns();
	int32 columnCount = CountColumnsWithMinHorizItemMargin();
	
	BRect toEraseRect;
	if (fCachedColumnCount > columnCount)
	{
		toEraseRect.Set (columnCount * (ItemWidth() + ItemHorizMargin()), 0, bounds.right, bounds.bottom);
		FillRect (toEraseRect);
	}
	else
	{
		if (columnCount < 1)
			columnCount = 1;
		int32 rowCount = (int32)ceil ((double)(fItemList->CountItems()) / (double)columnCount);
			
		int32 remainingInRow = fItemList->CountItems() % columnCount;
		toEraseRect.Set (0,rowCount * (ItemHeight() + ItemVertMargin()),bounds.right, bounds.bottom);
		FillRect (toEraseRect); 
		
		// Erase partial row content (those items have been moved left thus needs erasing)
		if (remainingInRow > 0)
		{
			toEraseRect.Set (remainingInRow * (ItemWidth() + ItemHorizMargin()), (rowCount - 1) * (ItemHeight() + ItemVertMargin()), bounds.right, rowCount * (ItemHeight() + + ItemVertMargin()));
			FillRect (toEraseRect);
		}
	}
	
	fCachedColumnCount = columnCount;
	float horizMargin = CalculateHorizMargin(Bounds().Width());
	SetHorizItemMargin(horizMargin);
	
	int32 x = 0;
	int32 y = 0;
	
	for (int32 i = 0; i < fItemList->CountItems(); i++)
	{
		BeCam_Item *item = (BeCam_Item*)(fItemList->ItemAt (i));
		if (item == NULL)
			break;	
		
		BRect itemRect;
		//
		itemRect.left = x * (ItemWidth() + ItemHorizMargin());
		itemRect.top = y * (ItemHeight() + ItemVertMargin()); 	
		itemRect.right = itemRect.left + ItemWidth() + ItemHorizMargin();
		itemRect.bottom = itemRect.top + ItemHeight() + ItemVertMargin();
		// Draw the item
		item->DrawItem(this, itemRect, true);
		//
		x++;
		if (x >= columnCount)
		{
			x = 0;
			y++;
		}
	}
}

//
// GridView :: FrameResized
void GridView::FrameResized (float newWidth, float newHeight)
{
	Draw (BRect (0, 0, 0, 0));	
	UpdateScrollView();
	return BView::FrameResized (newWidth, newHeight);
}

//
// GridView :: ItemRect
BRect GridView::ItemRect (int32 index)
{
	#ifdef DEBUG
		lfgridv = fopen(INTF_LOGFILE,"a");	
		fprintf(lfgridv,"GRIDVIEW - Item Rect of index %d\n",index);
		fclose(lfgridv);
	#endif
	
	int32 columnCount = CountColumnsWithMinHorizItemMargin();
	int32 x = (index % columnCount);
	int32 y = index / columnCount;

	BRect itemRect;
	
	itemRect.left = x * (ItemWidth () + ItemHorizMargin());
	itemRect.top = y * (ItemHeight () + ItemVertMargin());
	itemRect.right = itemRect.left + ItemWidth() + ItemHorizMargin();
	itemRect.bottom = itemRect.top + ItemHeight() + ItemVertMargin();
	return itemRect;
}

//
//	GridView :: Count Columns
int32 GridView::CountColumns () const
{
	return (int32)floor(Bounds().Width()/ (ItemWidth() + ItemHorizMargin()));
}

//
//	GridView :: Count Columns with the minimum horizontal margin
int32 GridView::CountColumnsWithMinHorizItemMargin () const
{
	return (int32)floor(Bounds().Width()/ (ItemWidth() + fMinHorizItemMargin));
}
//
//	GridView :: CountRows
int32 GridView::CountRows () const
{
	return (int32)ceil ((double)(fItemList->CountItems()) / (double)CountColumnsWithMinHorizItemMargin());
}

//
//	GridView :: KeyDown
void GridView::KeyDown (const char* bytes, int32 numBytes)
{
	if (!Window() || Window()->IsActive() == false)
		return;

	bool keyHandled = HandleKeyMovement (bytes, numBytes);
	if (fKeyTargetLooper)
	{
		BMessage *curMessage = Window()->CurrentMessage();
		curMessage->AddBool ("key_handled", keyHandled);
		fKeyTargetLooper->PostMessage (curMessage);
	}
	
	return BView::KeyDown (bytes, numBytes);
}

//
//	GridView :: MouseDown
void GridView::MouseDown (BPoint point)
{
	#ifdef DEBUG
		lfgridv = fopen(INTF_LOGFILE,"a");	
		fprintf(lfgridv,"GRIDVIEW - Mouse Down\n");
		fclose(lfgridv);
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
	
	// Check if the user clicked once and intends to drag
	int index = IndexOf(point);
	
	if (clickCount == 1)
    {
       	int32 modifs = modifiers();
		// select this item
      	if ((modifs & B_COMMAND_KEY))
      	{
			#ifdef DEBUG
				lfgridv = fopen(INTF_LOGFILE,"a");	
				fprintf(lfgridv,"GRIDVIEW - Option Key down\n");
				fclose(lfgridv);
			#endif
			Select(index,true);
      	}      	
      	else if((modifs & B_SHIFT_KEY))
      	{
      		Select(fSelectedItemIndex,index,true);
      	}
      	else
      		Select(index);
        // create a structure of
        // useful data
        list_tracking_data *data = new list_tracking_data();
        data->start = point;
        data->view = this;
        // spawn a thread that watches
        // the mouse to see if a drag
        // should occur.  this will free
        // up the window for more
        // important tasks
        resume_thread(spawn_thread((status_t(*)(void*))TrackItem,"list_tracking",B_DISPLAY_PRIORITY,data));
        //return;
    }
	previousPoint = point;
	ScrollToSelection ();
	return BView::MouseDown (point);
}

//
//	GridView :: MouseUp
void GridView::MouseUp (BPoint point)
{
	return BView::MouseUp (point);
}


//
//	GridView :: Count Items
int32 GridView::CountItems() const
{
	return fItemList->CountItems();
}

//
//	GridView :: Delete All Items
void GridView::DeleteAllItems ()
{
	for (int32 i = 0; i < fItemList->CountItems(); i++)
	{
		BeCam_Item *item = reinterpret_cast<BeCam_Item*>(fItemList->RemoveItem (0L));
		delete item;
		item = NULL;
	}

	fItemList->MakeEmpty();
	Draw (Bounds());
}

//
//	GridView :: Targeted By ScrollView
void GridView::TargetedByScrollView (BScrollView* scrollView)
{
	fScrollView = scrollView;
	UpdateScrollView();
}

//
// GridView :: AttachedToWindow
void GridView::AttachedToWindow()
{
	UpdateScrollView ();
	return BView::AttachedToWindow();
}

//
//	GridView :: Updated ScrollView
void GridView::UpdateScrollView ()
{	
	#ifdef DEBUG
		lfgridv = fopen(INTF_LOGFILE,"a");	
		fprintf(lfgridv,"GRIDVIEW - Update ScrollView\n");
		fclose(lfgridv);
	#endif
	if (fScrollView)
	{
		BScrollBar *vertbar = fScrollView->ScrollBar (B_VERTICAL);
		if (vertbar)
		{
			float maxV = CountRows() * (ItemHeight() + ItemVertMargin());

			if (maxV - Bounds().Height() > 0)
				vertbar->SetRange (0, maxV - Bounds().Height());
			else
				vertbar->SetRange (0, 0);
			
			vertbar->SetSteps ((ItemHeight() + ItemVertMargin()) / 2, (ItemHeight() + ItemVertMargin()));
		}
	}
}

//
//	GridView :: Handle KeyMovement
bool GridView::HandleKeyMovement (const char* bytes, int32 /* _unused */)
{
	#ifdef DEBUG
		lfgridv = fopen(INTF_LOGFILE,"a");	
		fprintf(lfgridv,"GRIDVIEW - Handle KeyMovement\n");
		fclose(lfgridv);
	#endif
	bool keyHandled = false;
	int32 modifs = modifiers();
	switch (bytes[0])
	{
		case B_RIGHT_ARROW:
		{
			keyHandled = true;
			if((modifs & B_SHIFT_KEY))
				Select (fSelectedItemIndex,fLastSelectedItemIndex < CountItems() - 1 ? fLastSelectedItemIndex + 1 : CountItems() - 1,true);
			else
				Select (fSelectedItemIndex < CountItems() - 1 ? fSelectedItemIndex + 1 : CountItems() - 1);
			ScrollToSelection ();
			break;
		}

		case B_LEFT_ARROW:
		{
			keyHandled = true;
			if((modifs & B_SHIFT_KEY))
				Select (fSelectedItemIndex,fLastSelectedItemIndex > 0 ? fLastSelectedItemIndex - 1 : 0,true);
			else
				Select (fSelectedItemIndex > 0 ? fSelectedItemIndex - 1 : 0);
			ScrollToSelection ();
			break;
		}
		
		case B_DOWN_ARROW:
		{
			keyHandled = true;
			
			int32 index = CountColumnsWithMinHorizItemMargin() + fLastSelectedItemIndex;
			if((modifs & B_SHIFT_KEY))
				Select (fSelectedItemIndex,index <= CountItems() - 1 ? index : CountItems() - 1,true);
			else
				Select (index <= CountItems() - 1 ? index : fSelectedItemIndex);
			ScrollToSelection ();
			break;
		}

		case B_UP_ARROW:
		{
			keyHandled = true;
			
			int32 index = fLastSelectedItemIndex - CountColumnsWithMinHorizItemMargin();
			if((modifs & B_SHIFT_KEY))
				Select (fSelectedItemIndex,index >= 0 ? index : 0,true);
			else
				Select (index >= 0 ? index : fSelectedItemIndex);
			ScrollToSelection ();
			break;
		}

		case B_HOME:
		{
			keyHandled = true;

			Select (0);
			ScrollToSelection ();
			break;
		}
		
		case B_END:
		{
			keyHandled = true;

			Select (CountItems() - 1);
			ScrollToSelection ();
			break;
		}
		
		case B_PAGE_DOWN:
		{
			keyHandled = true;

			int8 rowsPerPage = (int8)(Bounds().Height() / ItemHeight());
			int32 index = fSelectedItemIndex + rowsPerPage * CountColumnsWithMinHorizItemMargin();
			int32 finalRowIndex = (CountRows() - 1) * CountColumnsWithMinHorizItemMargin();
			int32 whichColumn = fSelectedItemIndex % CountColumnsWithMinHorizItemMargin();

			Select (index <= CountItems() - 1 ? index : MIN (finalRowIndex + whichColumn, CountItems() - 1));
			ScrollToSelection ();
			break;
		}
		
		case B_PAGE_UP:
		{
			keyHandled = true;

			int8 rowsPerPage = (int8)(Bounds().Height() / ItemHeight());
			int32 index = fSelectedItemIndex - rowsPerPage * CountColumnsWithMinHorizItemMargin();
			int32 whichColumn = fSelectedItemIndex % CountColumnsWithMinHorizItemMargin();

			Select (index >= 0 ? index : whichColumn);
			ScrollToSelection ();
			break;
		}
		case B_ESCAPE:
		{
			keyHandled = true;

			DeselectAll ();
			break;
		}
		case B_DELETE:
		{
			keyHandled = true;
			
			BMessage *message = new BMessage(REM_ITEMS);
			Window()->PostMessage(message);
			break;
		}
	}
	return keyHandled;
}

//
//	GridView :: ScrollToSelection
void GridView::ScrollToSelection ()
{
	if (!fSelectedItem)
		return;
	
	float currentPosition = 0;
	if (fScrollView)
	{
		BScrollBar *vertbar = fScrollView->ScrollBar (B_VERTICAL);
		if (vertbar)
			currentPosition = vertbar->Value();
	}

	BRect rect = ItemRect (fSelectedItemIndex);
	if (rect.bottom >= currentPosition + Bounds().Height())		// down
	{
		ScrollTo (0, rect.top + 2 - (Bounds().Height() - ItemHeight() -ItemVertMargin()));
		printf ("%f\n", rect.top + 2 - (Bounds().Height() - ItemHeight() - ItemVertMargin()));
	}	
	else if (rect.top <= currentPosition)				// up
		ScrollTo (0, rect.top - 4);
}

//
//	GridView :: ItemHeight
float GridView::ItemHeight () const
{
	return fItemHeight;
}

//
//	GridView :: ItemWidth
float GridView::ItemWidth () const
{
	return fItemWidth;
}

//
//	GridView :: Horizontal Item Margin
float GridView::ItemHorizMargin () const
{
	return fHorizItemMargin;
}

//
//	GridView :: Vertical Item Margin
float GridView::ItemVertMargin () const
{
	return fVertItemMargin;
}
//
//	GridView :: Set Horizontal Item Margin
void GridView::SetHorizItemMargin (float margin)
{
	fHorizItemMargin = margin;
}
//
//	GridView :: Set Vertical Item Margin
void GridView::SetVertItemMargin (float margin)
{
	fVertItemMargin = margin;
}
//
// GridView :: Selected Item
BeCam_Item* GridView::SelectedItem () const
{
	return fSelectedItem;
}

//
//	GridView :: ItemAt
BeCam_Item* GridView::ItemAt (int32 index) const
{
	return (BeCam_Item*)fItemList->ItemAt (index);
}

//
//	GridView :: SetSelectionCurveRadius
void GridView::SetSelectionCurveRadius (uint8 radius)
{
	fSelectionRadius = (int8)(radius - 1);		// internally subtract one because of how we
										// actually handle it in Draw()
	// So basically if they specify radius of zero (0) meaning no curve, ONLY
	// if we subtract one do we get that effect
	
	fSelectionRadius = MIN (fSelectionRadius, 6);
}

//
//	GridView :: SelectionCurveRadius
uint8 GridView::SelectionCurveRadius () const
{
	return (uint8)(fSelectionRadius + 1);
}

//
//	GridView :: SendKeyStrokesTo
void GridView::SendKeyStrokesTo (BLooper* looper, BHandler* handler)
{
	fKeyTargetLooper = looper;
	fKeyTargetHandler = handler;
}
//
//	GridView :: Select
void GridView::Select (int32 index, bool extend = false)
{
	#ifdef DEBUG
		lfgridv = fopen(INTF_LOGFILE,"a");	
		fprintf(lfgridv,"GRIDVIEW - Select Item\n");
		fclose(lfgridv);
	#endif
	if(!extend && !IsItemSelected(index))
		DeselectAll();
	BeCam_Item *item = (BeCam_Item*)(fItemList->ItemAt(index));
	if(item != NULL)
	{	
		if(!item->IsSelected())
		{
			item->Select();
			item->DrawItem (this, ItemRect (index), true);
			if(!extend)
			{
				fSelectedItem = item;
				fSelectedItemIndex = index;
			}
		}
		fLastSelectedItemIndex = index;
	}
}
//	GridView :: Select From To
void GridView::Select (int32 fromIndex, int32 toIndex, bool extend = false)
{
	BeCam_Item *item;
	int32 startIndex;
	int32 stopIndex;
	if(fromIndex < toIndex)
	{
		startIndex = fromIndex;
		stopIndex = toIndex;
	}
	else
	{
		startIndex = toIndex;
		stopIndex = fromIndex;
	}
	int32 countItems = fItemList->CountItems();
	for(int32 i = 0;i <= countItems;i++)
	{
		if(i < startIndex || i > stopIndex)  
			Deselect(i,extend);
	}
	for(int32 j = startIndex;j <= stopIndex;j++)
		Select(j,extend);
	fLastSelectedItemIndex = toIndex;
}
//
//	GridView:: Select All Items
void 	GridView::SelectAll ()
{
	Select(0,fItemList->CountItems()-1,true);
}

//
//	GridView :: Deselect
void GridView::Deselect(int32 index, bool extend = false)
{
	
	BeCam_Item *item = (BeCam_Item*)(fItemList->ItemAt(index));
	if(item != NULL && item->IsSelected())
	{
		item->Deselect ();
		item->DrawItem (this, ItemRect (index), true);
	}
	if(!extend)
	{
		fSelectedItem = NULL;
		fSelectedItemIndex = -1;
	}
}
//
//	GridView :: Deselect From To
//
void GridView::Deselect(int32 fromIndex, int32 toIndex)
{
	BeCam_Item *item;
	int32 startIndex;
	int32 stopIndex;
	if(fromIndex < toIndex)
	{
		startIndex = fromIndex;
		stopIndex = toIndex;
	}
	else
	{
		startIndex = toIndex;
		stopIndex = fromIndex;
	}
	for(int32 i = startIndex;i <= stopIndex;i++)
		Deselect(i);
		
}
//
//	GridView :: Deselect All
void GridView::DeselectAll()
{
	Deselect(0,fItemList->CountItems()-1);
}
//
//	GridView:: Current Selection
int32	GridView::CurrentSelection(int32 index = 0)
{
	int32 countItems = fItemList->CountItems();
	BeCam_Item* item;
	for(int i = index;i <= countItems;i++)
	{
		item = (BeCam_Item*)fItemList->ItemAt (i);
		if(item != NULL && item->IsSelected())
			return i;
	}
	return -1;
}
//
//	GridView:: Is item selected
bool	GridView::IsItemSelected(int32 index)
{
	BeCam_Item* item = (BeCam_Item*)fItemList->ItemAt (index);
	if(item != NULL)
		return item->IsSelected();
	return false;
}
//
//	GridView:: Make Empty
void	GridView::MakeEmpty()
{
	DeleteAllItems();
}
//
//		GridView::Get the number of selected items
int GridView::GetNumberOfSelectedItems()
{
	int count = CountItems();
	int numberOfSelectedItems = 0;
	for(int index=0;index < count;index++)
		if(IsItemSelected(index))
			numberOfSelectedItems++;
	return numberOfSelectedItems;
}
//
//		GridView::TrackItem
status_t GridView::TrackItem(list_tracking_data *data)
{
    uint32  buttons;
    BPoint  point;
    rgb_color color_items = {0xee, 0x00, 0x00, 0xff};
    rgb_color color_itemstrings = {0xff, 0xff, 0xff, 0xff};
    // we're going to loop as long as the mouse
    //is down and hasn't moved
    // more than kDRAG SLOP pixels
    while (1) 
    {
        // make sure window is still valid
        if (data->view->Window()->Lock()) 
        {
            data->view->GetMouse(&point, &buttons);
            data->view->Window()->Unlock();
        }
        // not?  then why bother tracking
        else
            break;
        // button up?  then don't do anything
        //if (!buttons)
            //break;
        // check to see if mouse has moved more
        // than kDRAG SLOP pixels in any direction
        if ((abs((int)(data->start.x - point.x)) > kDRAG_SLOP) || (abs((int)(data->start.y - point.y)) > kDRAG_SLOP))
        {
            // make sure window is still valid
            if (data->view->Window()->Lock()) 
            {
                BBitmap	 *drag_bits;
                BBitmap	 *src_bits;
                BMessage drag_msg(B_SIMPLE_DATA);
                BView    *offscreen_view;
                int32    index = data->view->CurrentSelection();
                BeCam_Item *item;
                // get the selected item
                item = dynamic_cast<BeCam_Item *>(data->view->ItemAt(index));
                if (item) 
                {
                    // add types 
                    drag_msg.AddString("be:types", B_FILE_MIME_TYPE);
                    //some useful information
                    drag_msg.AddInt32("handle",item->GetHandle());
                    //	add the actions
					drag_msg.AddInt32("be:actions", B_COPY_TARGET);
					drag_msg.AddString("be:clip_name", item->GetName());
                    // we can even include the item
                    //drag_msg.AddRef("entry_ref",item->Ref());
                    // get bitmap from current item
                    src_bits = item->GetThumbBitmap();
                    // make sure bitmap is valid
                    if (src_bits)
                    {
                        // create a new bitmap based on the one in the list (we
                        // can't just use the bitmap we get passed because the
                        // app server owns it after we call DragMessage, besides
                        // we wan't to create that cool semi-transparent look)
                        drag_bits = new BBitmap(src_bits->Bounds(),B_RGBA32, true);
                        // we need a view
                        // so we can draw
                        offscreen_view = new BView(drag_bits->Bounds(), "",B_FOLLOW_NONE, 0);
                        drag_bits->AddChild(offscreen_view);
                        // lock it so we can draw
                        drag_bits->Lock();
                        // fill bitmap with black
                        offscreen_view->SetHighColor(0, 0, 0, 0);
                        offscreen_view->FillRect(offscreen_view->Bounds());
                        // set the alpha level
                        offscreen_view->SetDrawingMode(B_OP_ALPHA);
                        offscreen_view->SetHighColor(0, 0, 0, 128);
                        offscreen_view->SetBlendingMode(B_CONSTANT_ALPHA,B_ALPHA_COMPOSITE);
                        // blend in bitmap
                        offscreen_view->DrawBitmap(src_bits);
                        //
                        int numberOfItemsSelected = data->view->GetNumberOfSelectedItems();
                        BFont font = be_plain_font;
                        font_height fontHeight;
                        font.SetFace(B_BOLD_FACE);
                        offscreen_view->SetFont(&font);
						offscreen_view->GetFontHeight (&fontHeight);
						float fFontHeight = fontHeight.ascent + fontHeight.descent - 4;
						BString numberOfItemsSelectedString;
                        numberOfItemsSelectedString << numberOfItemsSelected;
                        float fStringWidth = data->view->StringWidth(numberOfItemsSelectedString.String());
                        float ellipseX = offscreen_view->Bounds().left + 15;
                        float ellipseY = drag_bits->Bounds().top + 15;
                        if(numberOfItemsSelected > 1)
                        {
                        	offscreen_view->SetHighColor(color_items);
                        	offscreen_view->SetLowColor(color_items);
                        	BRect r(5,5,25,25);
                        	offscreen_view->FillEllipse(r);
                        	offscreen_view->MovePenTo(ellipseX - fStringWidth/2,ellipseY + fFontHeight/2);
                        	offscreen_view->SetHighColor(color_itemstrings);
                        	offscreen_view->SetLowColor(color_itemstrings);
                        	offscreen_view->StrokeEllipse(r);
                        	offscreen_view->DrawString(numberOfItemsSelectedString.String());
                        }
                        //
                        drag_bits->Unlock();
                        // initiate drag from center
                        // of bitmap
                        data->view->DragMessage(&drag_msg, drag_bits, B_OP_ALPHA,BPoint(drag_bits->Bounds().Height()/2,drag_bits->Bounds().Width()/2 ));
                    } // endif src bits
                    else
                    {
                        // no src bitmap?
                        // then just drag a rect
                        data->view->DragMessage(&drag_msg,BRect(0, 0, B_LARGE_ICON - 1,B_LARGE_ICON - 1));
					}
                } // endif item
                data->view->Window()->Unlock();
            } // endif window lock
            break;
        } // endif drag start
        // take a breather
        snooze(10000);
    } // while button
    delete(data);
    return B_NO_ERROR;
}
//
//		GridView :: ActionCopy
void GridView::ActionCopy(BMessage *request)
{
    entry_ref directory;
    request->FindRef("directory", &directory);
  	BMessage *copyRequest = new BMessage(B_COPY_TARGET);
    copyRequest->AddRef("directory",&directory);
	Window()->PostMessage(request);
}
//
//	GridView :: Index Of
int32 GridView::IndexOf(BPoint point) const
{
	int32 rowIndex = (int32)floor (point.y  / (ItemHeight() + ItemVertMargin())); 
	int32 colIndex = (int32)floor (point.x / (ItemWidth() + ItemHorizMargin()));
	int32 itemIndex = colIndex + rowIndex * (CountColumnsWithMinHorizItemMargin());
	return itemIndex;
}
//
//	GridView :: Calculate horizontal margin
float GridView::CalculateHorizMargin(float gridWidth) const
{
	int32 countColumns = CountColumnsWithMinHorizItemMargin();
	if(countColumns < 1)
		countColumns = 1;
	float totalMargin =  gridWidth - (ItemWidth() * countColumns);
	float horizMargin = totalMargin / countColumns;
	return horizMargin;
}

