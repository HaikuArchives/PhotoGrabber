/*
****************************************************************
* Copyright (c) 2004-2010,	Ramshankar, Jan-Rixt Van Hoye	   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
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
#include "logger.h"
#include "core_global.h"
#include "MainWindow.h"
#include <Message.h>

using namespace std;
#include <iterator>

//
//	GridView :: Constructor
GridView::GridView (const char* name, BMessage* InvokeMsg, BMessage* SelectMsg)
  : BControl(name, NULL, NULL, B_WILL_DRAW | B_FRAME_EVENTS | B_SCROLL_VIEW_AWARE),
    BObjectList<PicItem>(),
    fSelected(),
    fColumnCount(-1),
    fRowCount(0),
    fScrollView(NULL),
    fTargetMessenger(),
    fKeyTargetLooper(NULL),
    fKeyTargetHandler(NULL),
    fItemSize(DEFAULT_ITEM_WIDTH, DEFAULT_ITEM_HEIGHT),
    fMinItemSpacing(DEFAULT_ITEM_SPACING, DEFAULT_ITEM_SPACING),
    fItemSpacing(fMinItemSpacing),
    fSelectionRadius(DEFAULT_SELECTION_RADIUS),
    fSelectMsg(SelectMsg),
    fInvokeMsg(InvokeMsg)
{
    SetViewUIColor(B_PANEL_BACKGROUND_COLOR, B_DARKEN_3_TINT);
    LogDebug("GRIDVIEW - View Created.");
}

//
//	GridView :: Destructor
GridView::~GridView() {
    PicList::MakeEmpty();
	if (fSelectMsg) delete fSelectMsg;
    if (fInvokeMsg) delete fInvokeMsg;

    LogDebug("GRIDVIEW - View Destroyed.");
}

void
GridView::AttachedToWindow() {
    fScrollView = dynamic_cast<BScrollView*>(Parent());
    BFont font = be_plain_font;
    font.SetFace(B_BOLD_FACE);
    SetFont(&font);
    Layout();
}
bool
GridView::AddItem(PicItem* item) {
    bool added = PicList::AddItem(item);
    if (!Layout())
        InvalidateItem(IndexOf(item));
    return added;
}

bool
GridView::RemoveItem(PicItem* item) {
    return (RemoveItemAt(IndexOf(item)));
}

PicItem*
GridView::RemoveItemAt(listIndex index) {
    PicItem* removed = PicList::RemoveItemAt(index);
    if (removed) {
        fSelected.RemoveItem(removed, false);
        if (!Layout())
            InvalidateItem(index);
    }
    return (removed);
}

void
GridView::MakeEmpty() {
    PicList::MakeEmpty(true);
    fSelected.MakeEmpty(false);
    Layout();
}

void
GridView::AddList (BObjectList<PicItem>* list) {
    PicList::AddList(list);
    Layout();
}

//
// GridView :: Draw
void
GridView::Draw(BRect updateRect) {
    SetHighColor (ViewColor());
    FillRect(updateRect);

    if (IsEmpty()) return;

    BRect bounds = Frame();

    int32 column = 0;
    int32 row = 0;

    for (int32 i = 0; i < CountItems(); i++) {
        PicItem *item = ItemAt(i);
        BRect itemRect = ItemRect(i);
        if(updateRect.Intersects(itemRect.InsetByCopy(
                -fItemSpacing.width / 2,
                -fItemSpacing.height / 2)))
            DrawItem(item, itemRect);
    }
}
//
// GridView :: GetTop
float
GridView::GetTop(int32 row) {
    return fRowTops[row];
}
//
//	GridView :: GetRowHeight
float
GridView::GetRowHeight(int32 row) {
    return fRowTops[row + 1] - fRowTops[row - 1];
}
//
// GridView :: FrameResized
void
GridView::FrameResized (float newWidth, float newHeight) {
    LogDebug("GRIDVIEW - FrameResized width: %d - height: %d.", newWidth, newHeight);
    Layout();
    if (Bounds().Width() > newWidth)
        ResizeTo(newWidth, Bounds().Height());
}

bool
GridView::Layout() {
    LogDebug("LAYOUT");
    bool    needsRedraw = false;
    int32   count = CountItems(),
            newColumnCount,
            newRowCount;

    BSize   newItemSpacing(fMinItemSpacing);
    BSize   newItemSize(0, 0);

    vector<float> newRowTops;

    font_height fontHeight;
    GetFontHeight(&fontHeight);
    fDetailHeight = ceil(fontHeight.ascent + fontHeight.leading);
    fDetailSpacing = ceil((fontHeight.ascent + fontHeight.leading) / 3);

    BRect bounds = Bounds();
    BRect frame = Frame();
    if (frame.Width() != bounds.Width()) {
        this->ResizeTo(frame.Width(), bounds.Height());
    }

    for (listIndex i = 0; i < CountItems(); i++) {
        PicItem* item = ItemAt(i);
        if (item->ThumbWidth() > newItemSize.width)
            newItemSize.width = item->ThumbWidth();
        if (item->ThumbHeight() > newItemSize.height)
            newItemSize.height = item->ThumbHeight();

    }
    if (newItemSize.width == 0) newItemSize.width = DEFAULT_ITEM_WIDTH;
    if (newItemSize.height == 0) newItemSize.height = DEFAULT_ITEM_HEIGHT;

    newColumnCount = (int32)(bounds.Width() /
            (newItemSize.width + fMinItemSpacing.width));

    if (newColumnCount < 1) newColumnCount = 1;

    newRowCount = (count + newColumnCount - 1) / newColumnCount;
    newItemSpacing.width = ceil((bounds.Width() - newColumnCount * newItemSize.width) /
            newColumnCount);
    newItemSpacing.height = fMinItemSpacing.height + fDetailSpacing + fDetailHeight;

    float lastTop = 0;
    newRowTops.push_back(lastTop);
    for (int32 row = 0; row <= newRowCount; row++) {
        float newItemHeight = 0;
        for (int32 col = 0; col < newColumnCount; col++) {
            int32 index = row * newColumnCount + col;
            PicItem* item = ItemAt(index);
            if (item) {
                if (item->ThumbHeight() > newItemHeight)
                    newItemHeight = item->ThumbHeight();
            } else break;
        }
        lastTop += (newItemHeight + fItemSpacing.height + fDetailSpacing + fDetailHeight);
        newRowTops.push_back(lastTop);
    }

    if (newColumnCount != fColumnCount ||
        newRowCount != fRowCount ||
        newItemSpacing != fItemSpacing ||
        newItemSize != fItemSize ||
        !(newRowTops == fRowTops)) {
        fColumnCount            = newColumnCount;
        fRowCount               = newRowCount;
        fItemSpacing            = newItemSpacing;
        fRowTops                = newRowTops;
        fItemSize               = newItemSize;
        fRowsPerPage            = (int32)(bounds.Height() / fItemSize.height);
        Invalidate();

        BScrollBar *vertbar = fScrollView->ScrollBar(B_VERTICAL);
        float scrollRange = fRowTops[fRowCount] - Bounds().Height();
        vertbar->SetRange (0, (scrollRange > 0) ? scrollRange : 0);
        vertbar->SetSteps(fItemSize.height + fItemSpacing.height, fItemSize.height + fItemSpacing.height);
        SetExplicitMinSize(BSize(fItemSize.width + fItemSpacing.width + B_V_SCROLL_BAR_WIDTH,
                fItemSize.height + fItemSpacing.height));
        return true;
    } else
        return false;
}

void
GridView::DrawItem(PicItem* item, BRect frame) {
    bool enabled  = true;

    rgb_color color_selected = {0xee, 0xc9, 0x00, 0xff};
    rgb_color color_enabled = {0x00, 0x00, 0x00, 0xff};
    rgb_color color_disabled = {0x77, 0x00, 0x00, 0xff};
    rgb_color color_border = enabled ? color_enabled : color_disabled;
    rgb_color color_picture = {0x8b, 0x8b, 0x83, 0xff};

    if (!frame.IsValid())
        frame = ItemRect(IndexOf(item));

    SetHighColor(ViewColor());
    FillRect(frame);

    BRect thumbRect(frame);
    thumbRect.InsetBy(fItemSpacing.width / 2, fItemSpacing.height / 2);
    thumbRect.bottom -= (fDetailSpacing + fDetailHeight);
    BBitmap* thumbnail = item->ThumbBitmap();
    if (thumbnail) {
        BRect picRect   = thumbnail->Bounds();
        thumbRect.InsetBy((thumbRect.Width() - picRect.Width()) / 2,
                                        (thumbRect.Height() - picRect.Height()) / 2);

        DrawBitmap(thumbnail, thumbnail->Bounds(), thumbRect);
    } else {
        SetHighColor(color_picture);
        FillRect(thumbRect);
    }

    SetHighColor(color_border);
    StrokeRect(thumbRect);

    bool selected = (fSelected.IndexOf(item)>=0);
    if (selected) {
        SetPenSize(3);
        SetHighColor(color_selected);
        StrokeRoundRect(thumbRect.InsetByCopy(-1, -1), fSelectionRadius, fSelectionRadius);
        SetPenSize(1);
    }

    float stringWidth = StringWidth(item->Name());
    BPoint descPos(frame.left + (frame.Width() - stringWidth) / 2, frame.bottom - fItemSpacing.height / 2);
    DrawString(item->Name(), descPos);
}

void
GridView::InvalidateItem(listIndex index) {
    Invalidate(ItemRect(index));
}

// Returns Rectangle including the maximum thumbnail area
BRect
GridView::ItemRect(listIndex index) {
    int32 column = (index % fColumnCount);
    int32 row = index / fColumnCount;
    return BRect(column * (fItemSpacing.width + fItemSize.width), fRowTops[row],
                 (column + 1) * (fItemSpacing.width + fItemSize.width) - 1, fRowTops[row + 1] - 1);
}
//
//	GridView :: Count Columns
int32
GridView::CountColumns () const {
    return fColumnCount;
}
//
//	GridView :: CountRows
int32
GridView::CountRows () const {
    return fRowCount;
}
//
//	GridView :: KeyDown
void
GridView::KeyDown (const char* bytes, int32 numBytes) {
    if (!Window() || !Window()->IsActive() || !IsEnabled())
        return;

    bool keyHandled = HandleKeyMovement(bytes, numBytes);
    if (fKeyTargetLooper) {
        BMessage *curMessage = Window()->CurrentMessage();
        curMessage->AddBool("key_handled", keyHandled);
        fKeyTargetLooper->PostMessage(curMessage);
    }
}

//
//	GridView :: MouseDown
void
GridView::MouseDown(BPoint point) {
    LogDebug("GRIDVIEW - Mouse Down.");

    if (!Window() || !Window()->IsActive() || !IsEnabled())
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
    listIndex index = IndexAt(point);
    if (index < 0 || index >= CountItems())
        return;

    if (clickCount == 1 && button == B_PRIMARY_MOUSE_BUTTON) {
       	int32 modifs = modifiers();
		// select this item
      	if ((modifs & B_COMMAND_KEY))
			Select(index, true);
      	else if((modifs & B_SHIFT_KEY)) {
            SelectRange(IndexOf(fSelected.FirstItem()), index);
      	} else
            Select(index);
        // create a structure of
        // useful data
        list_tracking_data *data = new list_tracking_data();
        data->start = point;
        data->view = this;
        //data->Debug = Debug;
        // spawn a thread that watches
        // the mouse to see if a drag
        // should occur.  this will free
        // up the window for more
        // important tasks
        resume_thread(spawn_thread((thread_func)TrackItem, "list_tracking", B_DISPLAY_PRIORITY, data));
        //return;
    } else if(clickCount == 1 && button == B_SECONDARY_MOUSE_BUTTON) {
        ItemAt(index)->RotateThumb(90);
    	Select(index);
        if (!Layout())
            InvalidateItem(index);
    }
    previousPoint = point;
    ScrollToSelection();
}

//
//	GridView :: ScrollToSelection
void
GridView::ScrollToSelection() {
    LogDebug("GRIDVIEW - Scroll to selection.");

    if (fSelected.IsEmpty())
        return;

    float scrollPosition = 0;
    float scrollRange = 0;
    if (fScrollView) {
        BScrollBar *vertbar = fScrollView->ScrollBar (B_VERTICAL);
        if (vertbar) {
            scrollPosition = vertbar->Value();
            float scrollMin;
            vertbar->GetRange(&scrollMin, &scrollRange);
        }
    }

    BRect rect = ItemRect(IndexOf(fSelected.LastItem()));
    if (rect.bottom >= scrollPosition + Bounds().Height())		// down
        ScrollTo(0, rect.top - (Bounds().Height() - rect.Height() - fItemSpacing.height));
    else if (rect.top <= scrollPosition)				// up
        ScrollTo (0, rect.top );
    LogDebug("GRIDVIEW - Scrolled to selection.");
}
//
//	GridView :: Handle KeyMovement
bool
GridView::HandleKeyMovement (const char* bytes, int32 /* _unused */) {
    LogDebug("GRIDVIEW - Handle KeyMovement.");

    int32 firstSelected = fSelected.IsEmpty() ? 0 : IndexOf(fSelected.FirstItem());
    int32 lastSelected = fSelected.IsEmpty() ? 0 : IndexOf(fSelected.LastItem());
    int32 newSelected;
    bool extend = (modifiers() & B_SHIFT_KEY);
    bool keyHandled = false;
    switch (bytes[0]) {
        case B_RIGHT_ARROW:
            keyHandled = true;
            newSelected = lastSelected + 1;
            if (lastSelected < CountItems()) {
                if (extend) {
                    if (firstSelected > lastSelected)
                        Deselect(lastSelected);
                    else
                        Select(newSelected, true);
                } else
                    Select(newSelected);
            }
            break;

        case B_LEFT_ARROW:
            keyHandled = true;
            newSelected = lastSelected - 1;
            if (newSelected >= 0) {
                if (extend)
                    SelectRange(firstSelected, newSelected);
                else
                    Select(newSelected);
            }
            break;

        case B_DOWN_ARROW:
            keyHandled = true;

            newSelected = lastSelected + fColumnCount;
            if (newSelected < CountItems()) {
                if (extend)
                    SelectRange(firstSelected, newSelected);
                else
                    Select(newSelected);
                ScrollToSelection();
            }
            break;

        case B_UP_ARROW:
            keyHandled = true;
            newSelected = lastSelected - fColumnCount;
            if (newSelected >= 0) {
                if (extend)
                    SelectRange(firstSelected, newSelected);
                else
                    Select(newSelected);
                ScrollToSelection ();
            }
            break;

        case B_HOME:
            keyHandled = true;
            Select(0);
            ScrollToSelection ();
            break;

        case B_END:
            keyHandled = true;
            Select(CountItems() - 1);
            ScrollToSelection ();
            break;

        case B_PAGE_DOWN:
            keyHandled = true;

            newSelected = lastSelected + fRowsPerPage * fColumnCount;
            while (newSelected >= CountItems()) newSelected -= fColumnCount;
            Select(newSelected);
            ScrollToSelection();
            break;

        case B_PAGE_UP:
            keyHandled = true;

            newSelected = lastSelected - fRowsPerPage * fColumnCount;
            while (newSelected < 0) newSelected += fColumnCount;
            Select (newSelected);
            ScrollToSelection ();
            break;

    case B_ESCAPE:
            keyHandled = true;

            DeselectAll ();
            break;

    case B_DELETE: {
            keyHandled = true;

            BMessage *message = new BMessage(REM_ITEMS);
            Window()->PostMessage(message);
            break;
        }
    }
    return keyHandled;
}

void
GridView::SetMinItemSpacing(BSize spacing) {
    if (fMinItemSpacing != spacing) {
        fMinItemSpacing = spacing;
        Layout();
    }
}

BSize
GridView::MinItemSpacing() {
    return fMinItemSpacing;
}

//
// GridView :: Selected Item
PicItem*
GridView::SelectedItem(int32 index) {
    if (index >= 0 && index < fSelected.CountItems())
        return fSelected.ItemAt(index);
}

void
GridView::SetSelectionCurveRadius (uint8 radius)
{
    // internally subtract one because of how we
    // actually handle it in Draw()
	// So basically if they specify radius of zero (0) meaning no curve, ONLY
	// if we subtract one do we get that effect

	fSelectionRadius = MIN (radius - 1, 6);
    if (!fSelected.IsEmpty())
        Invalidate();
}

uint8
GridView::SelectionCurveRadius () const {
    return fSelectionRadius + 1;
}

void
GridView::SendKeyStrokesTo (BLooper* looper, BHandler* handler) {
    fKeyTargetLooper = looper;
    fKeyTargetHandler = handler;
}

//	GridView :: Select
void
GridView::Select(listIndex index, bool extend) {
    LogDebug("GRIDVIEW - Select Item.");

    PicItem *item = ItemAt(index);
    bool isSelected = (fSelected.IndexOf(item) >= 0);

    if (extend) {
        if (!isSelected && item) {
            fSelected.AddItem(item);
            InvalidateItem(index);
        }
    } else {
        DeselectAll();
        if (item) {
            fSelected.AddItem(item);
            InvalidateItem(index);
        }
    }
}

//	GridView :: Select From To
void
GridView::SelectRange(listIndex fromIndex, listIndex toIndex) {
	PicItem *item;
	if (fromIndex > toIndex) {
        for (listIndex i = CountItems() - 1; i >= 0; i--) {
            item = ItemAt(i);
            if (i > fromIndex || i < toIndex) {
                if (fSelected.RemoveItem(item, false))
                    InvalidateItem(i);
            } else {
                if (fSelected.AddItem(item))
                    InvalidateItem(i);
            }
        }
    } else {
        for (listIndex i = 0; i < CountItems(); i++) {
            item = ItemAt(i);
            if (i < fromIndex || i > toIndex) {
                if (fSelected.RemoveItem(item, false))
                    InvalidateItem(i);
            } else {
                if (fSelected.AddItem(item))
                    InvalidateItem(i);
            }
        }
    }
}
//
//	GridView:: Select All Items
void GridView::SelectAll ()
{
    fSelected.MakeEmpty(false);
    for (listIndex i = 0; i < CountItems(); i++)
        fSelected.AddItem(ItemAt(i));
    Invalidate();
}

//
//	GridView :: Deselect
void GridView::Deselect(listIndex index) {
	PicItem *item = ItemAt(index);
    if (fSelected.RemoveItem(item, false)) {
        InvalidateItem(index);
	}
}

void
GridView::DeselectAll() {
    PicItem* item;
    while (item = fSelected.LastItem()) {
        fSelected.RemoveItem(item, false);
        InvalidateItem(IndexOf(item));
    }
}

//
//	GridView:: Current Selection
listIndex
GridView::SelectedIndex(int32 index) {
	if (index < 0 || index >= fSelected.CountItems())
        return -1;

	return IndexOf(fSelected.ItemAt(index));
}

bool
GridView::ItemSelected(listIndex index) {
    if (index < 0 || index >= CountItems())
        return -1;
    return (fSelected.IndexOf(ItemAt(index)) >= 0);
}

bool
GridView::ItemSelected(PicItem* item) {
    return (fSelected.IndexOf(item) >= 0);
}

BObjectList<PicItem>*
GridView::Selected() {
    return &fSelected;
}

//
//		GridView::Get the number of selected items
int32
GridView::CountSelected() {
    return fSelected.CountItems();
}

//		GridView::TrackItem
status_t
GridView::TrackItem(list_tracking_data *data) {
    uint32  buttons;
    BPoint  point;
    rgb_color color_items = {0xee, 0x00, 0x00, 0xff};
    rgb_color color_itemstrings = {0xff, 0xff, 0xff, 0xff};
    // we're going to loop as long as the mouse
    //is down and hasn't moved
    // more than kDRAG SLOP pixels
    while(1) {
        // make sure window is still valid
        LogDebug("GRIDVIEW - TrackItem - Lock the window.");
        if (data->view->Window()->Lock()) {
            data->view->GetMouse(&point, &buttons);
            LogDebug("GRIDVIEW - TrackItem - Unlock the window.");
            data->view->Window()->Unlock();
        } else break; // not?  then why bother tracking
        // button up?  then don't do anything
        if (!buttons)
            break;
        // check to see if mouse has moved more
        // than kDRAG SLOP pixels in any direction
        BPoint delta = data->start - point;
        if (delta.x * delta.x + delta.y * delta.y > kDRAG_SLOP * kDRAG_SLOP) {
            // make sure window is still valid
            LogDebug("GRIDVIEW - TrackItem - Lock the window.");
            if (data->view->Window()->Lock()) {
                BBitmap	 *drag_bits;
                BBitmap	 *src_bits;
                BMessage drag_msg(B_SIMPLE_DATA);
                BView    *offscreen_view;
                PicItem *item = data->view->fSelected.LastItem();
                // get the selected item
                if (item) {
                    // add types
                    drag_msg.AddString("be:types", B_FILE_MIME_TYPE);
                    //some useful information
                    drag_msg.AddInt32("handle",item->Handle());
                    //	add the actions
					drag_msg.AddInt32("be:actions", B_COPY_TARGET);
					drag_msg.AddString("be:clip_name", item->Name());
                    // we can even include the item
                    //drag_msg.AddRef("entry_ref",item->Ref());
                    // get bitmap from current item
                    src_bits = item->ThumbBitmap();
                    // make sure bitmap is valid
                    if (src_bits) {
                        // create a new bitmap based on the one in the list (we
                        // can't just use the bitmap we get passed because the
                        // app server owns it after we call DragMessage, besides
                        // we wan't to create that cool semi-transparent look)
                        drag_bits = new BBitmap(src_bits->Bounds(), B_RGBA32, true);
                        // we need a view
                        // so we can draw
                        offscreen_view = new BView(drag_bits->Bounds(), "",B_FOLLOW_NONE, 0);
                        drag_bits->AddChild(offscreen_view);
                        // lock it so we can draw
                        LogDebug("GRIDVIEW - TrackItem -  Lock the Bitmap.");
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
                        int numberOfItemsSelected = data->view->fSelected.CountItems();
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
                        if(numberOfItemsSelected > 1) {
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
                        LogDebug("GRIDVIEW - TrackItem - Unlock the bitmap.");
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
               LogDebug("GRIDVIEW - TrackItem - Unlock the window.");
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

void
GridView::ActionCopy(BMessage *request) {
    entry_ref directory;
    request->FindRef("directory", &directory);
  	BMessage *copyRequest = new BMessage(B_COPY_TARGET);
    copyRequest->AddRef("directory",&directory);
	Window()->PostMessage(request);
}

listIndex
GridView::IndexAt(BPoint point) {
	int32 row = 0, column = 0;
	for (listIndex itemIndex = 0; itemIndex < CountItems(); itemIndex++) {
		BRect itemRect = ItemRect(itemIndex);
        itemRect.InsetBy(fItemSpacing.width / 2, fItemSpacing.height / 2);
		if (itemRect.Contains(point))
            return itemIndex;
    }
    return -1;
}

void
GridView::SortItemsBy(int sortType) {
	switch(sortType) {
		case ITEM_SORT_BY_TITLE:
			SortItems(GridView::CompareTitles);
			break;
		case ITEM_SORT_BY_DATE:
			SortItems(GridView::CompareDates);
			break;
		default:
			break;
	}
	Invalidate();
}
//
// GridView :: Compare item titles
// return: -1:first<second 0:first==second 1:first>second
int
GridView::CompareTitles(const PicItem* first, const PicItem* second) {
	BString firstTitle(((PicItem*)first)->Name());
	return firstTitle.ICompare(((PicItem*)second)->Name());
}
//
// GridView :: Compare item dates
// return: -1:first<second 0:first==second 1:first>second
int
GridView::CompareDates(const PicItem* first, const PicItem* second) {
	BString firstDate(((PicItem*)first)->Date());
    return firstDate.ICompare(((PicItem*)second)->Date());
}
