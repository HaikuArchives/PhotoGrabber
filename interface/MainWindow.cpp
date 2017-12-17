/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/

//  Includes
#include <stdio.h>
#include <View.h>
#include <Screen.h>
#include <app/Messenger.h>
#include <Layout.h>

// Local includes
#include <LayoutBuilder.h>
#include "MainWindow.h"
#include "core_global.h"
#include "logger.h"
#include "../framework/plugin_fw/PluginInterface.h"
//
//  External variables
bool tbExpanded;

FILE *lfmainw;
//		
//  MainWindow:: Constructor
 BeCam_MainWindow::BeCam_MainWindow(BLooper *syscore, int devtype) 
  : BWindow(BRect(0, 0, 100, 100),"PhotoGrabber",B_DOCUMENT_WINDOW, B_WILL_ACCEPT_FIRST_CLICK),
    becam_connected(false),
    devicetype(devtype),
    systemcore(syscore),
    aboutWindow(NULL),
    configWindow(NULL)
{
    BScreen screen;
    BRect r = screen.Frame();
    r.left = (r.left + r.Width() - WINDOW_WIDTH_MAIN) / 2;
    r.top = (r.top + r.Height() - WINDOW_HEIGHT_MAIN) / 2;
    r.right = r.left + WINDOW_WIDTH_MAIN;
    r.bottom = r.top + WINDOW_HEIGHT_MAIN;

    ResizeTo(r.Width(), r.Height());
    MoveTo(r.LeftTop());
    float minWidth, maxWidth, minHeight, maxHeight;
    GetSizeLimits(&minWidth,&maxWidth,&minHeight,&maxHeight);
    SetSizeLimits(WINDOW_WIDTH_MAIN, maxWidth, WINDOW_HEIGHT_MAIN, maxHeight);

    // Add the Menubar
    BLayoutBuilder::Menu<>(fMenubar = new BMenuBar(Bounds(), "menu_bar"))
        .AddMenu(_T("File"))
            .AddItem(_T("About"), B_ABOUT_REQUESTED)
            .AddSeparator()
            .AddItem(_T("Preferences"), CONFIG, 'P')
            .AddSeparator()
            .AddItem(_T("Quit"), B_QUIT_REQUESTED, 'Q')
		.End()
		.AddMenu(becam_actionsMenu = new BMenu(_T("Actions")))
			.AddItem(_T("Connect"), CAM_CONNECT)
			.AddItem(_T("Disconnect"), CAM_DISCON)
		.End()
		.AddMenu(_T("View"))
    		.AddItem(sortTitleMenu = new BMenuItem(_T("Sort By Title"), new BMessage(CAM_SORT_TITLE)))
    		.AddItem(sortDateMenu = new BMenuItem(_T("Sort By Date"), new BMessage(CAM_SORT_DATE)))
    	.End()
    #ifdef DEBUG	
        .AddMenu(_T("Debugging"))
        	.AddItem(_T("Open Statuswindow"), OPN_STATUS)
            .AddItem("Show Layout", SHOW_LAYOUT)
            .AddMenu("Status Dock")
                .AddItem("Hidden", SHOW_SD_HIDDEN)
                .AddItem("Init", SHOW_SD_INIT)
                .AddItem("Connected", SHOW_SD_CONNECTED)
                .AddItem("Download", SHOW_SD_DOWNLOAD)
            .End()
        .End()
    #endif
	.End();
	AddChild(fMenubar);

    fStatusDock = new StatusDock("statusdock");
    fGridView = new GridView("gridview");	
    fGridScrollView = new BScrollView("becam_scrollview", fGridView,
            B_FOLLOW_ALL_SIDES, false, true, B_PLAIN_BORDER);

    BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
        .SetExplicitAlignment(BAlignment(B_ALIGN_USE_FULL_WIDTH, B_ALIGN_USE_FULL_HEIGHT))
    	.Add(fGridScrollView)
    	.Add(fStatusDock)
    .End();
    fGridScrollView->SetExplicitAlignment(BAlignment(B_ALIGN_USE_FULL_WIDTH, B_ALIGN_TOP));
    fStatusDock->SetExplicitAlignment(BAlignment(B_ALIGN_USE_FULL_WIDTH, B_ALIGN_BOTTOM));
    Layout(true);
    ShowStatusDock(MODE_INIT, _T("Please connect your digital camera."));    
    fGridView->MakeFocus(true);
    

    // Get the main debug function;	
    LogDebug("MAINWINDOW - Window created.");
    
}

void
BeCam_MainWindow::adaptActionMenu(bool enabled) {
    becam_actionsMenu->RemoveItems(2, 3);
    if (enabled) {
        if (devicetype == TYPE_USB) {
            becam_actionsMenu->ItemAt(0)->SetEnabled(false);
            becam_actionsMenu->ItemAt(1)->SetEnabled(false);

            BMessage reply;
            BMessage message(CHECK_FUNCTION);
            message.AddUInt32("function", fn_takePicture);
            BMessenger messenger(NULL, systemcore);
            messenger.SendMessage(&message, &reply);
            bool canTakePicture = reply.GetBool("present", false);

            if (canTakePicture) {
                becam_actionsMenu->AddSeparatorItem();
                becam_actionsMenu->AddItem(new BMenuItem(_T("Take Picture"), new BMessage(CAM_TAKE_SHOT)));
            }
        } else {
            becam_actionsMenu->ItemAt(0)->SetEnabled(true);
            becam_actionsMenu->ItemAt(1)->SetEnabled(false);
        }
    } else {
        becam_actionsMenu->ItemAt(0)->SetEnabled(false);
        becam_actionsMenu->ItemAt(1)->SetEnabled(false);
    }
}
//
// MainWindow:: Add item to the listview
void 
BeCam_MainWindow::addItem(class PicItem *item) {
    Lock();
    fGridView->AddItem(item);
    Unlock();
}
//
// MainWindow:: Add item to the listview
void BeCam_MainWindow::removeItem(class PicItem *item) {
    Lock();
    fGridView->RemoveItem(item);
    Unlock();
}
//
// MainWindow:: Clear the listview
void 
BeCam_MainWindow::clearItems() {
    Lock();
    fGridView->MakeEmpty();
    Unlock();
}
//
// MainWindow:: Download the selected items
void BeCam_MainWindow::downloadSelectedItems(entry_ref copyToDir, const char *fileName) {
    //
    if(fGridView->SelectedIndex() >= 0) {
        items_data *data = new items_data();
        data->downloadDir = copyToDir;
        data->window = this;
        //data->Debug = Debug;
        resume_thread(spawn_thread((status_t(*)(void*))DownloadItems,"download_items",B_DISPLAY_PRIORITY,data));
    } else {
        BAlert *myAlert = new BAlert(_T("Download"), _T("Please select some files."),_T("Ok"), NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);
        myAlert->SetShortcut(0, B_ENTER);
        myAlert->Go();
    }
}
//
// MainWindow:: Download Items thread function
status_t BeCam_MainWindow::DownloadItems(items_data *data) {
    BObjectList<PicItem> selection;
    selection.AddList(data->window->fGridView->Selected());
    int32 totalSelected = selection.CountItems();
    int32 count = 0;
    entry_ref refentry;
    char    tmpBuffer[100];
    BMessenger coreMessenger(data->window->systemcore);
    refentry = data->downloadDir;
	
    // Show the status dock
    snprintf(tmpBuffer, 100, "Downloading number %u of the %ld selected files",
             1, totalSelected);
    data->window->ShowStatusDock(MODE_DOWNLOAD, tmpBuffer, totalSelected);
	
    // Update the status dock
    for (int32 selIndex = 0; selIndex < totalSelected; selIndex++) {
    	snprintf(tmpBuffer, 100, "Downloading number %u of the %ld selected files",
                 selIndex + 1, totalSelected);
        data->window->UpdateStatusDock(1, tmpBuffer);
		
        // Get selected item
        PicItem* item = selection.ItemAt(selIndex);

        // Send a message to the camera interface to get the selected item
        BMessage* coreMsg = new BMessage(DOWN_ITEM);
        coreMsg->AddInt32("itemhandle", item->Handle());
        coreMsg->AddRef("copyToDir", &refentry);
        BMessage reply;
        coreMessenger.SendMessage(coreMsg, &reply);
        if(reply.what == DOWN_ITEM_OK) {
            count++;	
        } else {
            BAlert *myAlert = new BAlert(_T("Error"), 
                    _T("Failed to copy an item."), _T("Ok"), 
                    NULL, NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);	
            myAlert->SetShortcut(0, B_ENTER);
            myAlert->Go();
            break;
    	}
    }
    // Hide the status dock
    data->window->ShowStatusDock(MODE_HIDDEN);
    selection.MakeEmpty(false);
    delete(data);
}
//
// MainWindow:: Remove the selected items
void 
BeCam_MainWindow::removeSelectedItems() {
    LogDebug("MAINWINDOW - Begin remove Items.");
	
    if (fGridView->SelectedIndex() >= 0) {
        // Ask the user if he/she is sure to remove the files.
        BAlert *myAlert = new BAlert(_T("Remove files"), _T("Are you sure you want to erase the selected files?"),_T("No"), _T("Yes"),NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);
        myAlert->SetShortcut(0, B_ESCAPE);
        uint32 button_index = myAlert->Go();
        if(button_index == 1) {
            items_data *data = new items_data();
            data->window = this;
            resume_thread(spawn_thread((status_t(*)(void*))RemoveItems,"remove_items",B_DISPLAY_PRIORITY,data));
        }
    } else {	
        BAlert *myAlert = new BAlert(_T("Remove"), _T("Please select some files."),_T("Ok"), NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);	
        myAlert->SetShortcut(0, B_ENTER);
        myAlert->Go();
    }
    LogDebug("MAINWINDOW - End remove Items.");
}
//
//	MainWindow:: Remove Items thread function
status_t 
BeCam_MainWindow::RemoveItems(items_data *data) {
    status_t status = B_OK;
    BMessage *cam_message;
    BObjectList<PicItem> selection;
    selection.AddList(data->window->fGridView->Selected());
    
    PicItem *selectedItem;
    for (listIndex i = 0; i < selection.CountItems(); i++) {
        PicItem* item = selection.ItemAt(i);
        // Send a message to the camera 
    	// to get the selected item
        cam_message = new BMessage(REM_ITEM);
        cam_message->AddInt32("itemhandle", item->Handle());
        // Wait until the item has been downloaded
        BMessenger messenger(NULL, data->window->systemcore);
        BMessage reply;
        messenger.SendMessage(cam_message, &reply);
	    delete cam_message;
        if (reply.what == REM_ITEM_OK) 
            data->window->fGridView->RemoveItem(selectedItem);
        else {
            status = B_ERROR;
            break;
        }
    }
    selection.MakeEmpty(false);
    return status;
}
//
//	MainWindow:: Error messages
int 
BeCam_MainWindow::logMainWindowError(int ErrorMes) {
    const char *errorMessage;
    switch(ErrorMes) {
	case MAINW_DEV_NO_FIND:
	    errorMessage = "MAIN WINDOW: No PTP device is present.";	
	    break;
	default:
            errorMessage = "MAIN WINDOW: An unexpected error occured.";
    }
    // write the errorMessage into the logfile
    LogDebug(errorMessage);
	
    return(ErrorMes);
}

//		BeCam_MainWin::QuitRequested
//

bool BeCam_MainWindow::QuitRequested() {
    systemcore->PostMessage(B_QUIT_REQUESTED);
    return true;
}
//
//	BeCam_MainWindow:: Open the Configuration Window
void BeCam_MainWindow::CreateConfigWindow() {
    float xPos=0,yPos=0;
	
    if (!configWindow) {
		CalculatePos(&xPos, &yPos, CONFIG_WINDOW);
		configWindow = new BeCam_ConfigWindow(xPos, yPos, this);
		configWindow->Show();
    }
}

//
//	BeCam_MainWindow:: Open the About Window
void 
BeCam_MainWindow::CreateAboutWindow() {
    float xPos=0, yPos=0;	
	
    if(!aboutWindow) {
        CalculatePos(&xPos, &yPos, ABOUT_WINDOW);
        aboutWindow = new BeCam_AboutWindow(xPos, yPos, this);
    }
    aboutWindow->Show();
}
//
//	BeCam_MainWindow:: Open the Status Window
void
BeCam_MainWindow::ShowStatusDock(Modus modus, const char *status, float total) {
    BMessage* message = new BMessage(STATDOCK_SHOW);
    message->AddInt32("modus", modus);
    message->AddFloat("total", total);
    message->AddString("statusmessage", status);
    PostMessage(message, fStatusDock);
}
//
//		BeCam_MainWindow::Update the Status Window
void 
BeCam_MainWindow::UpdateStatusDock(uint32 delta, const char *status) {
    BMessage* message = new BMessage(UPDATE_STAT);
    message->AddFloat("count", delta);
    if (status)
        message->AddString("statusmessage", status);
    PostMessage(message, fStatusDock);
}

//
//	BeCam_MainWindow:: Calculate the windowposition
void 
BeCam_MainWindow::CalculatePos(float *xPos,float *yPos,int winType) {
    float length, height;
    BRect mainRect = Frame();
    length = mainRect.right - mainRect.left;
    height = mainRect.bottom - mainRect.top;
    switch(winType) {
	case ABOUT_WINDOW:
	    *xPos = mainRect.left + (length - WINDOW_WIDTH_ABOUT) / 2;
	    *yPos = mainRect.top + (height - WINDOW_HEIGHT_ABOUT) / 2;
	    break;
	case CONFIG_WINDOW:
	    *xPos = mainRect.left + (length - WINDOW_WIDTH_CONFIG) / 2;
	    *yPos = mainRect.top + (height - WINDOW_HEIGHT_CONFIG) / 2;
	    break;
    }
}
//
//		BeCam_MainWin::MessageRecieved
void 
BeCam_MainWindow::MessageReceived(BMessage* message) {
    const char *product;

    switch(message->what) {
	case B_QUIT_REQUESTED:
	    systemcore->PostMessage(CAM_DISCON);
	    systemcore->PostMessage(B_QUIT_REQUESTED);
	    break;
	case CAM_CONNECTED: {
	    becam_connected = true;
        product = message->FindString("product");
        devicetype = message->GetInt32("type", 0);
        adaptActionMenu(false);
        
        BString msgText(_T("Connected to: "));
        msgText += product;
        ShowStatusDock(MODE_CONNECTED, msgText.String());
	    break;
	}
    case GET_ITEMS_START: 
        ShowStatusDock(MODE_CONNECTED, _T("Processing images..."), message->GetFloat("total", 0));
        break;
        
	case GET_ITEMS_DONE: {
        ShowStatusDock(MODE_CONNECTED, _T("All images received from camera."));
        adaptActionMenu(true);
        ShowStatusDock(MODE_HIDDEN);
        break;
	}
	case CAM_DISCONNECTED: {
	    becam_connected = false;	
	    clearItems();
	    ShowStatusDock(MODE_INIT, _T("Please connect your digital camera."));
        adaptActionMenu(false);
	    break;
	}
	case CAM_SHOT_TAKEN:
            break;
            
	case CAM_TAKE_SHOT:
            systemcore->PostMessage(message);
            break;
            
	case ADD_ITEM: {
	    ItemData  *localItemData;
	    PicItem *localItem;
	    message->FindPointer("item", (void **) &localItemData);
	    localItem = new PicItem(localItemData);
	    addItem(localItem);
        UpdateStatusDock(1);
	    break;
	}
	case CAM_CONNECT: {
	    BMenuItem *connect,*disconnect;
	    BMessenger messenger(NULL,systemcore);
	    BMessage reply;
	    BMessage cam_message(CAM_CONNECT);
	    messenger.SendMessage(&cam_message,&reply);
	    if (reply.what == CAM_CONNECT_OK) {
                systemcore->PostMessage(message);
                connect = becam_actionsMenu->FindItem(_T("Connect"));	
                connect->SetEnabled(false);
                disconnect = becam_actionsMenu->FindItem(_T("Disconnect"));	
                disconnect->SetEnabled(true);
                //
                ShowStatusDock(MODE_CONNECTED, _T("Connecting to the digital camera..."), -1);
            } else {	
                BAlert *myAlert = new BAlert(_T("Connect"), _T("Could not connect to the camera"),_T("Ok"), NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);
                myAlert->SetShortcut(0, B_ENTER);
                myAlert->Go();
            }	
	    break;
	}
	case CAM_DISCON: {	
	    BMenuItem *connect,*disconnect;
	    systemcore->PostMessage(message);	
	    connect = becam_actionsMenu->FindItem(_T("Connect"));
	    connect->SetEnabled(true);	
	    disconnect = becam_actionsMenu->FindItem(_T("Disconnect"));
	    disconnect->SetEnabled(false);
	    break;
	}
	case B_COPY_TARGET: {
	    entry_ref copyToDirDrag;
	    const char *fileName = NULL;
	    LogDebug("MAINWINDOW - Get the save directory.");
	    message->FindRef("directory", &copyToDirDrag);
	    fileName = message->FindString("name");
	    BPath directory;
	    directory = BPath(&copyToDirDrag);
	    LogDebug("MAINWINDOW - The file name is: %s.",fileName);
	    LogDebug("MAINWINDOW - The save directory is: %s.",directory.Path());
	    downloadSelectedItems(copyToDirDrag, fileName);
	    break;
	}
	case REM_ITEMS:	{
	    removeSelectedItems();
	    break;
	}
	case OPN_STATUS: {	
	    char	tmpBuffer[100];
	    snprintf(tmpBuffer, 100, "Downloading number %ld of the %ld selected files", 0, 0);
	    ShowStatusDock(MODE_DOWNLOAD, tmpBuffer, 0);
	    break;
	}
	case B_ABOUT_REQUESTED:
	    CreateAboutWindow();
	    break;
            
	case CONFIG:
	    CreateConfigWindow();
	    break;
            
	case B_SELECT_ALL:
	    fGridView->SelectAll();
	    break;
            
	case GET_CONFIGURATION: {
	    // Get the settings from the system core
	    BMessage reply;
	    BMessage message(GET_CONFIGURATION);
	    BMessenger messenger(NULL,systemcore);
	    messenger.SendMessage(&message,&reply);
	    reply.FindPointer("settings",(void **)&pgsettings);
	    BMenuItem *morePath;
	    break;
	}
	case RELOAD_CONFIGURATION: {
	    becam_connected = false;	
	    clearItems();
	    adaptActionMenu(true);
	    break;
	}
	case STATDOCK_SHOW: {
        this->DispatchMessage(message, fStatusDock);    
	    break;
	}
#ifdef DEBUG
    case SHOW_LAYOUT: 
        for (int32 i = 0; i < CountChildren(); i++)
            FrameView(ChildAt(i));
        FrameLayout(GetLayout());
        break;
        
    case SHOW_SD_HIDDEN:
        ShowStatusDock(MODE_HIDDEN);
        break;
        
    case SHOW_SD_INIT:
        ShowStatusDock(MODE_INIT, "Init Mode");
        break;
        
    case SHOW_SD_CONNECTED:
        ShowStatusDock(MODE_CONNECTED, "Connected Mode");
        break;
        
    case SHOW_SD_DOWNLOAD:
        ShowStatusDock(MODE_DOWNLOAD, "Download Mode", 100);
        break;
        
#endif        
	case STATDOCK_SHOWED: {
        float statusDockHeight = fStatusDock->IsHidden() ? 0 : fStatusDock->Bounds().Height();
	    fGridScrollView->ResizeTo(Bounds().Width(), Bounds().Height() - statusDockHeight - fMenubar->Bounds().Height());
	    BScrollBar *v_scrollbar = fGridScrollView->ScrollBar(B_VERTICAL);
	    v_scrollbar->ResizeTo(v_scrollbar->Frame().Width(), v_scrollbar->Frame().Height() + B_H_SCROLL_BAR_HEIGHT - 2);
        fStatusDock->MoveTo(Bounds().LeftBottom() - fStatusDock->Bounds().LeftBottom());
	    break;
	}
/*        
	case STATDOCK_HIDDEN: {
	    becam_gridview->SetEnabled(true);
	    becam_scrollview->ResizeTo(becam_scrollview->Frame().Width(),becam_scrollview->Bounds().Height() + becam_statusDock->Bounds().Height());
	    BScrollBar *v_scrollbar = becam_scrollview->ScrollBar(B_VERTICAL);
	    v_scrollbar->ResizeTo(v_scrollbar->Frame().Width(),v_scrollbar->Frame().Height() - B_H_SCROLL_BAR_HEIGHT + 2);
	    break;
	}
 */
	case CAM_SORT_TITLE:
	    sortDateMenu->SetMarked(false);
	    sortTitleMenu->SetMarked(true);
	    fGridView->SortItemsBy(ITEM_SORT_BY_TITLE);
	    break;
	case CAM_SORT_DATE:
	    sortDateMenu->SetMarked(true);
	    sortTitleMenu->SetMarked(false);
	    fGridView->SortItemsBy(ITEM_SORT_BY_DATE);
	    break;
	default:
	    BWindow::MessageReceived(message);
    }
}
