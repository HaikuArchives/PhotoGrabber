/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
// 
// Inlcudes
#include <Window.h>
#include <Directory.h>
#include <MenuItem.h>
#include <Mime.h>
#include <BitmapStream.h>
#include <TranslatorRoster.h>
#include <TranslatorFormats.h>
#include <stdio.h>
#include <Alert.h>
//
//	Local Includes
#include "ListView.h"
#include "core_global.h"
#include "debug.h"

FILE *lflistv;
//
// 	External variables
//extern class BeDiGiCamApp *app;
//
//
#define kDRAG_SLOP		4

enum	MESSAGES		{eItemDblClicked = 1000,
						 eItemMenuSelected,
						 eItemDragged};

const rgb_color kLIST_COLOR				= {255, 255, 255, 255};

//		
//	ListView:: Constructor
 BeCam_ListView::BeCam_ListView(BRect r) : BListView(r,"becam_listview",
														B_MULTIPLE_SELECTION_LIST,
														B_FOLLOW_ALL_SIDES,
														B_WILL_DRAW | B_NAVIGABLE | B_FRAME_EVENTS
														)
{

}
//
//		BeCam_ListView::MessageRecieved
void BeCam_ListView::MessageReceived(BMessage* message)
{		
	switch(message->what)
		{
			case B_COPY_TARGET: 
				ActionCopy(message);
				break;
			default:
				BListView::MessageReceived(message);
		}
}
//
//		BeCam_ListView::MouseDown
void BeCam_ListView::MouseDown(BPoint where)
{
	int32 buttons = 0;
	BMessage *message =	Window()->CurrentMessage();
	message->FindInt32("buttons",&buttons);
	//BPopUpMenu	*fMenu;
	
	int index = IndexOf(where);

	if ((index >= 0) && (index < CountItems())) 
	{
		/*// if clicked with second mouse button, let's do a context-sensitive menu
		if (buttons & B_SECONDARY_MOUSE_BUTTON) 
		{
			BPoint	point = where;
			ConvertToScreen(&point);
			// select this item
			Select(index);
			// build menu
			fMenu = new BPopUpMenu("context menu");
			fMenu->SetRadioMode(false);
			fMenu->AddItem(new BMenuItem("Download Item", new BMessage(eItemMenuSelected)));
			fMenu->Go(point, true, false, true);
			return;
		}
		else						
        {*/
        	// clicked with primary button
            int32 clicks;
            // see how many times we've been clicked
			Window()->CurrentMessage()->FindInt32("clicks", &clicks);
            // if we've only been clicked once
            // on this item, see if user
            // intends to drag
            if ((clicks == 1) || (index != CurrentSelection()))
            {
               	int32 modifs = modifiers();
				// select this item
      			if ((modifs & B_OPTION_KEY) || (modifs & B_SHIFT_KEY))
      				Select(index,true);
               	else
                	Select(index);
               	// create a structure of
               	// useful data
               	list_tracking_data *data = new list_tracking_data();
               	data->start = where;
               	data->view = this;
               	// spawn a thread that watches
               	// the mouse to see if a drag
               	// should occur.  this will free
               	// up the window for more
               	// important tasks
               	resume_thread(spawn_thread((status_t(*)(void*))TrackItem,"list_tracking",B_DISPLAY_PRIORITY,data));
               	return;
            }
            						
		/*}
		if(fMenu != NULL)
			delete(fMenu);*/
	}
	// either the user dbl-clicked an item or clicked in an area with no
	// items.  either way, let BListView take care of it
	BListView::MouseDown(where);
}
//
//		BeCam_ListView::Select all Items
void BeCam_ListView::SelectAll()
{
	Select(0,CountItems()-1,true);
}
//
//		BeCam_ListView::Get the number of selected items
int BeCam_ListView::GetNumberOfSelectedItems()
{
	int count = CountItems();
	int numberOfSelectedItems = 0;
	for(int index=0;index < count;index++)
		if(IsItemSelected(index))
			numberOfSelectedItems++;
	return numberOfSelectedItems;
}
//
//		BeCam_ListView::TrackItem
status_t BeCam_ListView::TrackItem(list_tracking_data *data)
{
    uint32  buttons;
    BPoint  point;
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
    // free resource
    //free(data);
    delete(data);
    return B_NO_ERROR;
}
//
//		BeCam_ListView :: ActionCopy
void BeCam_ListView::ActionCopy(BMessage *request)
{
    //BeCam_Item *item;
    // get the selected item TODO:: this should copy all selected files
    
    entry_ref directory;
    request->FindRef("directory", &directory);
  	/*int count = CountItems();
	for(int index=0;index < count;index++)
		if(IsItemSelected(index))
		{
			item = dynamic_cast<BeCam_Item *>(ItemAt(index));
			int handle = item->GetHandle();
    		if (item) 
    		{*/
    			BMessage *copyRequest = new BMessage(B_COPY_TARGET);
    			//copyRequest->AddInt32("handle",handle);
    			copyRequest->AddRef("directory",&directory);
				//app->PostMessage(copyRequest);
			//}
			//delete(item);
		//}
    
    //item = dynamic_cast<BeCam_Item *>(ItemAt(CurrentSelection()));
    //if (item) 
    //{
    	//request->AddInt32("handle",item->GetHandle());
		Window()->PostMessage(request);
	//} 
}
