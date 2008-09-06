/*****************************************************************
 * Copyright (c) 2004-2008,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/

// 
//		Includes
#include <stdio.h>
#include <View.h>
#include <Screen.h>
#include <app/Messenger.h>
//
// Local includes
#include "MainWindow.h"
#include "core_global.h"
#include "PictureLabelButton.h"
//

//
// External variables
bool tbExpanded;
//#pragma export on
extern "C" BWindow * instantiate_mainWindow(BLooper *core, int devtype);
//#pragma export reset
FILE *lfmainw;
//
// Exported function
BWindow* instantiate_mainWindow(BLooper *core,int devtype)
{
	float length,height;
	BScreen screen;
	BRect r = screen.Frame();
	length = r.right - r.left;
	height = r.bottom - r.top;
	r.left = ((r.left + length/2) -(WINDOW_WIDTH_MAIN/2));
	r.top = ((r.top + height/2) -(WINDOW_HEIGHT_MAIN/2));
	r.right = r.left + WINDOW_WIDTH_MAIN;
	r.bottom = r.top + WINDOW_HEIGHT_MAIN;
	BWindow *mainWindow = new BeCam_MainWindow(r,core,devtype);
	mainWindow->Show();
	return mainWindow;
}
//		
//	MainWindow:: Constructor
 BeCam_MainWindow::BeCam_MainWindow(BRect r,BLooper *syscore,int devtype) 
				: BWindow(r,"PhotoGrabber",B_DOCUMENT_WINDOW, B_WILL_DRAW)
{
	#ifdef DEBUG
		lfmainw = fopen(INTF_LOGFILE,"a");	
		fprintf(lfmainw,"MAINWINDOW - Create window\n");
		fclose(lfmainw);
	#endif
	float minWidth,maxWidth,minHeight,maxHeight;
	GetSizeLimits(&minWidth,&maxWidth,&minHeight,&maxHeight);
	SetSizeLimits(WINDOW_WIDTH_MAIN,maxWidth,WINDOW_HEIGHT_MAIN,maxHeight);
	devicetype = devtype;
	systemcore = syscore;
	// initialisation variables
	becam_connected = false;
	aboutWindow = NULL;
	configWindow = NULL;
	//
	// Add the Menubar
	r = Bounds();
	addMenuBar();
	// Add the MainView	
	r.top = becam_menubar->Frame().bottom + 1;
	// Create the MainView	
	becam_view = new BeCam_MainView(r,"BeDiGiCamView");
	// Set the View properties
	rgb_color bg_color=ui_color(B_PANEL_BACKGROUND_COLOR);
	becam_view->SetViewColor(bg_color);
	// Create the Action Dock
	CreateActionDock();
	// Create the Status Dock
	CreateStatusDock();
	//
	r.top=0;
	r.left=0;
	r.right-=15;
	r.bottom= becam_view->Bounds().bottom - 71;
	// Add the Grid View
	becam_gridview = new GridView(r,"gridview", B_FOLLOW_ALL, B_WILL_DRAW);	
	becam_scrollview = new BScrollView(
									"becam_scrollview",
									becam_gridview,
									B_FOLLOW_ALL_SIDES,
									B_WILL_DRAW | B_FRAME_EVENTS,
									false,
									true,
									B_PLAIN_BORDER
									);
    becam_view->AddChild(becam_scrollview);
    becam_gridview->TargetedByScrollView (becam_scrollview);	
	// add view to window
	AddChild(becam_view);
	//Set the focus to the listview
	becam_gridview->MakeFocus(true);
	becam_actionDock->Hide();
	#ifdef DEBUG
		lfmainw = fopen(INTF_LOGFILE,"a");	
		fprintf(lfmainw,"MAINWINDOW - Window created\n");
		fclose(lfmainw);
	#endif
}
//
// MainWindow:: Create the menubar
void BeCam_MainWindow::addMenuBar ()
{
	becam_menubar = new BMenuBar(Bounds(), "menu_bar");
	AddChild(becam_menubar);
	// Add File menu to the menu bar
	becam_fileMenu = new BMenu("File");
	becam_fileMenu->AddItem(new BMenuItem(_T("About") , new BMessage(ABOUT)));
	becam_fileMenu->AddSeparatorItem();
	becam_fileMenu->AddItem(new BMenuItem(_T("Preferences") , new BMessage(CONFIG),'P'));
	becam_fileMenu->AddSeparatorItem();
	becam_fileMenu->AddItem(new BMenuItem(_T("Quit"), new BMessage(QUIT), 'Q'));
	becam_menubar->AddItem(becam_fileMenu);
	// Add Extra menu to the menu bar
	becam_extraMenu = new BMenu(_T("Extra"));
	becam_extraMenu->AddItem(new BMenuItem(_T("Download") , new BMessage(DOWN_BUTTON), 'S'));
	becam_extraMenu->AddItem(new BMenuItem(_T("Delete") , new BMessage(DEL_BUTTON),'D'));
	becam_extraMenu->SetEnabled(false);
	becam_menubar->AddItem(becam_extraMenu);
	//	Add extra BDCP menu
	BMenuItem *disconnect;
	becam_actionsMenu = new BMenu(_T("Actions"));
	becam_actionsMenu->AddItem(new BMenuItem(_T("Connect") , new BMessage(CAM_CONNECT)));
	becam_actionsMenu->AddItem(new BMenuItem(_T("Disconnect") , new BMessage(CAM_DISCON)));
	disconnect = becam_actionsMenu->FindItem(_T("Disconnect"));
	disconnect->SetEnabled(false);
	becam_menubar->AddItem(becam_actionsMenu);
	// Add the test menu
	#ifdef DEBUG	
		BMenu *debuggingMenu = new BMenu(_T("Debugging"));
		debuggingMenu->AddItem(new BMenuItem(_T("Open Statuswindow") , new BMessage(OPN_STATUS)));
		becam_menubar->AddItem(debuggingMenu);
	#endif	
	if(devicetype == TYPE_USB)
		becam_actionsMenu->SetEnabled(false);
}

//
// MainWindow:: Create the Action Dock with the action controls
void BeCam_MainWindow::CreateActionDock ()
{
	BRect r = becam_view->Bounds();
	r.top = r.bottom - 70;
	becam_actionDock = new ActionDock(r,"actiondock", B_FOLLOW_BOTTOM | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW | B_FRAME_EVENTS);
	becam_view->AddChild(becam_actionDock);
}
//
// MainWindow:: Create the Action Bar with the action controls
void BeCam_MainWindow::CreateStatusDock ()
{
	
	BRect r = becam_view->Bounds();
	r.top = r.bottom - 70;
	becam_statusDock = new StatusDock(r,"statusdock", B_FOLLOW_BOTTOM | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW | B_FRAME_EVENTS);
	becam_statusDock->SetStatusMessage("Please connect your camera.");
	becam_statusDock->ShowChildren(MODE_INIT);
	becam_view->AddChild(becam_statusDock);
}
//
// MainWindow:: Add item to the listview
void BeCam_MainWindow::addItem(class BeCam_Item *item)
{
	Lock();
	becam_gridview->AddItem(item);
	Unlock();
}
//
// MainWindow:: Add item to the listview
void BeCam_MainWindow::removeItem(class BeCam_Item *item)
{
	Lock();
	becam_gridview->RemoveItem(item);
	Unlock();
}
//
// MainWindow:: Clear the listview
void BeCam_MainWindow::clearItems()
{
	Lock();
	becam_gridview->MakeEmpty();
	Unlock();
}
//
// MainWindow:: Download the selected items
void BeCam_MainWindow::downloadSelectedItems(entry_ref *copyToDir = NULL)
{
	float totalpics=0;
	char	tmpBuffer[100];
	//
	if(becam_gridview->CurrentSelection() >= 0)
	{
		becam_extraMenu->SetEnabled(false);
		// create the status bar
		totalpics = becam_gridview->GetNumberOfSelectedItems();
		sprintf(tmpBuffer,"Downloading number %ld of the %ld selected files",(uint32)0,(int32)totalpics);
		ShowStatusDock(totalpics, tmpBuffer);
		UpdateStatusDock(0, tmpBuffer);
		// 
		items_data *data = new items_data();
		data->statusDock = becam_statusDock;
		data->totalitems = totalpics;
		data->downloadDir = copyToDir;
        data->gridview = becam_gridview;
        data->window = this;
		resume_thread(spawn_thread((status_t(*)(void*))DownloadItems,"download_items",B_DISPLAY_PRIORITY,data));
		//
		becam_extraMenu->SetEnabled(true);
	}
	else
	{
		BAlert *myAlert = new BAlert(_T("Download"), _T("Please select some files."),_T("Ok"), NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);
		myAlert->SetShortcut(0, B_ENTER);
		myAlert->Go();
	}
}
//
// MainWindow:: Download Items thread function
status_t BeCam_MainWindow::DownloadItems(items_data *data)
{
	entry_ref refentry;
	int count=0,index=0;
	char	tmpBuffer[100];
	BeCam_Item *selectedItem;
	BMessage *message;

	if(data->downloadDir != NULL)
		refentry = *(data->downloadDir);
	while((index = data->gridview->CurrentSelection(index)) >= 0)
	{
		selectedItem = (BeCam_Item *)data->gridview->ItemAt(index);
		// Send a message to the camera interface 
		// to get the selected item
		message = new BMessage(DOWN_ITEM);
		message->AddInt32("itemhandle",(int32)selectedItem->GetHandle());
		if(data->downloadDir != NULL)
			message->AddRef("copyToDir", &refentry);
		else
		{
			BEntry *entry = new BEntry(data->window->pgsettings->defaultDownloadPath);
			entry->GetRef(&refentry);
			message->AddRef("copyToDir", &refentry);
		}
		// Wait untill the item has been downloaded
		BMessenger messenger(NULL,data->window->systemcore);
		BMessage reply;
		messenger.SendMessage(message,&reply);
		if(reply.what == DOWN_ITEM_OK)
		{
			count++;
			sprintf(tmpBuffer,"Downloading number %ld of the %ld selected files",(uint32)count,(int32)(data->totalitems));
			
			BMessenger messenger(data->statusDock);
			message = new BMessage(UPDATE_STAT);
			message->AddFloat("count",1);
			message->AddString("statusmessage",tmpBuffer);
			messenger.SendMessage(message);
		}
		delete(message);
		index++;
	}
	BMessenger messenger(data->window);
	message = new BMessage(HIDE_STATUSDOCK);
	messenger.SendMessage(message);
	delete(message);
}
//
// MainWindow:: Remove the selected items
void BeCam_MainWindow::removeSelectedItems()
{
	//
	#ifdef DEBUG
		lfmainw = fopen(INTF_LOGFILE,"a");	
		fprintf(lfmainw,"MAINWINDOW - Begin remove Items\n");
		fclose(lfmainw);
	#endif
	if(becam_gridview->CurrentSelection() >= 0)
	{
		becam_extraMenu->SetEnabled(false);
		// Ask the user if he/she is sure to remove the files.
		BAlert *myAlert = new BAlert(_T("Remove files"), _T("Are you sure you want to erase the selected files?"),_T("No"), _T("Yes"),NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);
		myAlert->SetShortcut(0, B_ESCAPE);
		uint32 button_index = myAlert->Go();
		if(button_index == 1)
		{
			//
			items_data *data = new items_data();
        	data->gridview = becam_gridview;
        	data->window = this;
			resume_thread(spawn_thread((status_t(*)(void*))RemoveItems,"remove_items",B_DISPLAY_PRIORITY,data));
			//
			becam_extraMenu->SetEnabled(true);	
		}
	}
	else
	{	
		BAlert *myAlert = new BAlert(_T("Remove"), _T("Please select some files."),_T("Ok"), NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);	
		myAlert->SetShortcut(0, B_ENTER);
		myAlert->Go();
	}
	#ifdef DEBUG
		lfmainw = fopen(INTF_LOGFILE,"a");	
		fprintf(lfmainw,"MAINWINDOW - End remove Items\n");
		fclose(lfmainw);
	#endif
			
}
//
//	MainWindow:: Remove Items thread function
status_t BeCam_MainWindow::RemoveItems(items_data *data)
{
	int count=0,index=0;
	BMessage *cam_message;
		
	count = data->gridview->CountItems();
	BeCam_Item *selectedItem;
	while((index = data->gridview->CurrentSelection(index)) >= 0)
	{
		selectedItem = (BeCam_Item *)data->gridview->ItemAt(index);
		// Send a message to the camera 
		// to get the selected item
		cam_message = new BMessage(REM_ITEM);
		cam_message->AddInt32("itemhandle", (int32)selectedItem->GetHandle());
		// Wait untill the item has been downloaded
		BMessenger messenger(NULL,data->window->systemcore);
		BMessage reply;
		messenger.SendMessage(cam_message,&reply);
		if(reply.what == REM_ITEM_OK)
		{
			data->window->removeItem(selectedItem);
			selectedItem = NULL;
			index = 0;
			delete(cam_message);
			continue;
		}
		delete(cam_message);
		index++;
	}
	return B_NO_ERROR;
}
//
//	MainWindow:: Error messages
int BeCam_MainWindow::logMainWindowError(int ErrorMes)
{
	char 				*errorMessage;
	
	switch(ErrorMes)
	{
		case MAINW_DEV_NO_FIND:
			errorMessage = "MAIN WINDOW: No PTP device is present\n";	
			break;
		default:
			errorMessage = "MAIN WINDOW:An unexpected error occured\n";
	}
	// write the errorMessage on the screen
	if(DEBUG_SCREEN)
		printf(errorMessage);
	// write the errorMessage into the logfile
	if(DEBUG_LOGFILE)
	{
		FILE	*file;
		
		file = fopen(INTF_LOGFILE,"a");
		fprintf(file,errorMessage);
		fclose(file);
	}
	return(ErrorMes);
}

//		BeCam_MainWin::QuitRequested
//

bool BeCam_MainWindow::QuitRequested()
{
	systemcore->PostMessage(CAM_DISCON);
	systemcore->PostMessage(B_QUIT_REQUESTED);
	return BWindow::QuitRequested();
}
//
//	BeCam_MainWindow:: Open the Configuration Window
void BeCam_MainWindow::CreateConfigWindow()
{
	float xPos=0,yPos=0;
	
	if(!configWindow)
	{
		CalculatePos(&xPos,&yPos,CONFIG_WINDOW);
		configWindow = new BeCam_ConfigWindow(xPos,yPos,this);
		configWindow->Show();
	}
}

//
//	BeCam_MainWindow:: Open the About Window
void BeCam_MainWindow::CreateAboutWindow()
{
	float xPos=0,yPos=0;	
	
	if(!aboutWindow)
	{
		CalculatePos(&xPos,&yPos,ABOUT_WINDOW);
		aboutWindow = new BeCam_AboutWindow(xPos,yPos,this);
	}
	aboutWindow->Show();
}
//
//	BeCam_MainWindow:: Open the Status Window
void BeCam_MainWindow::ShowStatusDock(float totalbytes, char *message)
{
	becam_actionDock->Hide();
	becam_statusDock->ShowChildren(MODE_DOWNLOAD);
	becam_statusDock->SetMaxStatusBar(totalbytes);
	becam_statusDock->UpdateStatus(0,message);
	becam_statusDock->Show();
}
//
//		BeCam_MainWindow::Update the Status Window
void BeCam_MainWindow::UpdateStatusDock(uint32 delta, char *message)
{
	//nothing yet
}
//
//	BeCam_MainWindow:: Close the Status Window
void BeCam_MainWindow::HideStatusDock(void)
{
	becam_statusDock->Hide();
	becam_statusDock->ShowChildren(MODE_INIT);
	becam_actionDock->Show();
}
//
//	BeCam_MainWindow:: Calculate the windowposition
void BeCam_MainWindow::CalculatePos(float *xPos,float *yPos,int winType)
{
	float length,height;
	BRect mainRect = Frame();
	length = mainRect.right - mainRect.left;
	height = mainRect.bottom - mainRect.top;
	switch(winType)
	{
		case ABOUT_WINDOW:
			*xPos = ((mainRect.left + length/2) -(WINDOW_WIDTH_ABOUT/2));
			*yPos = ((mainRect.top + height/2) - (WINDOW_HEIGHT_ABOUT/2));
			break;
		case CONFIG_WINDOW:
			*xPos = ((mainRect.left + length/2) -(WINDOW_WIDTH_CONFIG/2));
			*yPos = ((mainRect.top + height/2) - (WINDOW_HEIGHT_CONFIG/2));
			break;
	}
}
//
//		BeCam_MainWin::MessageRecieved
void BeCam_MainWindow::MessageReceived(BMessage* message)
{
	const char *product;
			
	switch(message->what)
		{
		
		case QUIT:
			systemcore->PostMessage(CAM_DISCON);
			systemcore->PostMessage(B_QUIT_REQUESTED);
			break;
		case CAM_CONNECTED:
			product = message->FindString("product");
			becam_connected = true;
			becam_extraMenu->SetEnabled(true);
			//becam_downloadPopup->SetEnabled(true);
			becam_statusDock->Hide();
			becam_actionDock->Show();
			break;
		case CAM_DISCONNECTED:
			product = message->FindString("product");
			becam_connected = false;	
			//becam_downloadPopup->SetEnabled(false);
			becam_extraMenu->SetEnabled(false);
			clearItems();
			becam_actionDock->Hide();
			becam_statusDock->SetStatusMessage(_T("Camera disconnected"));
			becam_statusDock->Show();
			break;
		case ADD_ITEM:
		{
			ItemData  *localItemData;
			BeCam_Item *localItem;
			message->FindPointer("item", (void **) &localItemData);
			localItem = new BeCam_Item(localItemData);
			addItem(localItem);
			break;
		}
		case CAM_CONNECT:
		{
			BMenuItem *connect,*disconnect;
			BMessenger messenger(NULL,systemcore);
			BMessage reply;
			BMessage cam_message(CAM_CONNECT);
			messenger.SendMessage(&cam_message,&reply);
			if(reply.what == CAM_CONNECT_OK)
			{
				systemcore->PostMessage(message);
				connect = becam_actionsMenu->FindItem(_T("Connect"));	
				connect->SetEnabled(false);
				disconnect = becam_actionsMenu->FindItem(_T("Disconnect"));	
				disconnect->SetEnabled(true);
				//
				becam_statusDock->SetStatusMessage(_T("Connecting to camera..."));
			}
			else
			{	
				BAlert *myAlert = new BAlert(_T("Connect"), _T("Could not connect to the camera"),_T("Ok"), NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);
				myAlert->SetShortcut(0, B_ENTER);
				myAlert->Go();
			}	
			break;
		}
		case CAM_DISCON:
		{	
			BMenuItem *connect,*disconnect;
			systemcore->PostMessage(message);	
			connect = becam_actionsMenu->FindItem(_T("Connect"));
			connect->SetEnabled(true);	
			disconnect = becam_actionsMenu->FindItem(_T("Disconnect"));
			disconnect->SetEnabled(false);
			break;
		}
		case DOWN_BUTTON:
			downloadSelectedItems();
			break;
		case B_COPY_TARGET:
		{
			entry_ref copyToDirDrag;
			message->FindRef("directory", &copyToDirDrag);
			downloadSelectedItems(&copyToDirDrag);
			break;
		}
		case DEL_BUTTON:
		case REM_ITEMS:
		{
			removeSelectedItems();
			break;
		}
		case NEXT_BUTTON:
		{
			becam_gridview->SelectNext();
			break;
		}
		case PREVIOUS_BUTTON:
		{
			becam_gridview->SelectPrevious();
			break;
		}
		case OPN_STATUS:
		{	
			char	tmpBuffer[100];
			sprintf(tmpBuffer,"Downloading number %ld of the %ld selected files",(uint32)0,(uint32)0);
			ShowStatusDock(0, tmpBuffer);
			break;
		}
		case ABOUT:
			CreateAboutWindow();
			break;
		case CONFIG:
			CreateConfigWindow();
			break;
		case B_SELECT_ALL:
			becam_gridview->SelectAll();
			break;
		case GET_CONFIGURATION:
		{
             // Get the settings from the system core
			BMessage reply;
			BMessage message(GET_CONFIGURATION);
			BMessenger messenger(NULL,systemcore);
			messenger.SendMessage(&message,&reply);
			reply.FindPointer("settings",(void **)&pgsettings);
			BMenuItem *morePath;
            break;
		}
		case RELOAD_CONFIGURATION:
		{
			//	Add extra BDCP menu
			BMessage reply;
			int32 devtype;
			BMessage message(GET_DEVTYPE);
			BMessenger messenger(NULL,systemcore);
			messenger.SendMessage(&message,&reply);
			reply.FindInt32("devtype",&devtype);
			#ifdef DEBUG
				lfmainw = fopen(INTF_LOGFILE,"a");	
				fprintf(lfmainw,"MAINWINDOW - device type is: %ld\n",devtype);
				fclose(lfmainw);
			#endif
			if(devtype == TYPE_PAR)
			{
				becam_actionsMenu->SetEnabled(true);
			}
			else
			{
				becam_actionsMenu->SetEnabled(false);
			}
			break;
		}
		case HIDE_ACTIONDOCK:
		{
			float height;
			message->FindFloat("height",&height);
			becam_gridview->ResizeTo(becam_gridview->Frame().Width(),becam_gridview->Frame().Height() + height);
			break;
		}
		case SHOW_ACTIONDOCK:
		{
			float height;
			message->FindFloat("height",&height);
			becam_gridview->ResizeTo(becam_gridview->Frame().Width(),becam_gridview->Frame().Height() - height);
			break;
		}
		case SHOW_STATUSDOCK:
		{
			break;
		}
		case HIDE_STATUSDOCK:
		{
			HideStatusDock();
			break;
		}
		default:
			BWindow::MessageReceived(message);
		}
}

