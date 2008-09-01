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
		lfmainw = fopen(LOGFILE,"a");	
		fprintf(lfmainw,"MAINWINDOW - Create window\n");
		fclose(lfmainw);
	#endif
	devicetype = devtype;
	systemcore = syscore;
	// initialisation variables
	becam_connected = false;
	aboutWindow = NULL;
	configWindow = NULL;
	statusWindow = NULL;
	//
	// Add the Menubar
	r = Bounds();
	#ifdef DEBUG
		lfmainw = fopen(LOGFILE,"a");	
		fprintf(lfmainw,"MAINWINDOW - Create menubar\n");
		fclose(lfmainw);
	#endif
	addMenuBar();
	// Add the MainView	
	r.top = becam_menubar->Frame().bottom + 1;	
	becam_view = new BeCam_MainView(r,"BeDiGiCamView");
	#ifdef DEBUG
		lfmainw = fopen(LOGFILE,"a");	
		fprintf(lfmainw,"MAINWINDOW - Set the controls\n");
		fclose(lfmainw);
	#endif
	//set controls
	rgb_color bg_color=ui_color(B_PANEL_BACKGROUND_COLOR);
	becam_view->SetViewColor(bg_color);

	r.top=5;
	r.left=5;
	r.right-=20;
	r.bottom=r.bottom-90-becam_menubar->Bounds().bottom;

	becam_gridview = new GridView(r,"gridview", B_FOLLOW_ALL, B_WILL_DRAW);
	
		
	becam_scrollview = new BScrollView(
									"becam_scrollview",
									becam_gridview,
									B_FOLLOW_ALL_SIDES,
									B_WILL_DRAW | B_FRAME_EVENTS,
									false,
									true,
									B_FANCY_BORDER
									);
    becam_view->AddChild(becam_scrollview);
    becam_gridview->TargetedByScrollView (becam_scrollview);
    #ifdef DEBUG
		lfmainw = fopen(LOGFILE,"a");	
		fprintf(lfmainw,"MAINWINDOW - Create popupmenu\n");
		fclose(lfmainw);
	#endif
    // 	Popup menu to choose your path
 	becam_downloadMenu = new BPopUpMenu("downloadMenu");
	// 	Add the popup menu to the config groupbox
	r.top = becam_gridview->Frame().bottom + 10;
	r.left = 5;
	r.right= r.left + 250;
	becam_downloadPopup = new BMenuField(r, "path", _T("Download Folder:"), 
											becam_downloadMenu,
											B_FOLLOW_BOTTOM,
											B_WILL_DRAW | B_NAVIGABLE);
	becam_downloadPopup->SetDivider(be_plain_font->StringWidth("Download Folder:") + 5);
	becam_downloadPopup->SetEnabled(false);
	becam_view->AddChild(becam_downloadPopup);
	//
	#ifdef DEBUG
		lfmainw = fopen(LOGFILE,"a");	
		fprintf(lfmainw,"MAINWINDOW - Add action buttons\n");
		fclose(lfmainw);
	#endif	
	addActionBar();	
	#ifdef DEBUG
		lfmainw = fopen(LOGFILE,"a");	
		fprintf(lfmainw,"MAINWINDOW - Action buttons added\n");
		fprintf(lfmainw,"MAINWINDOW - Add the statusbar\n");
		fclose(lfmainw);
	#endif
	addStatusBar();
	#ifdef DEBUG
		lfmainw = fopen(LOGFILE,"a");	
		fprintf(lfmainw,"MAINWINDOW - Statusbar added\n");
		fclose(lfmainw);
	#endif
	// add view to window
	AddChild(becam_view);
	// Create a FilePanel to select the download directory
	becam_selectdirpanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), NULL, B_DIRECTORY_NODE, false,new BMessage(OPEN_FILE_PANEL), NULL, false, true);
	//Set the focus to the listview
	becam_gridview->MakeFocus(true);
	#ifdef DEBUG
		lfmainw = fopen(LOGFILE,"a");	
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
	actionsMenu = new BMenu(_T("Actions"));
	actionsMenu->AddItem(new BMenuItem(_T("Connect") , new BMessage(CAM_CONNECT)));
	actionsMenu->AddItem(new BMenuItem(_T("Disconnect") , new BMessage(CAM_DISCON)));
	disconnect = actionsMenu->FindItem(_T("Disconnect"));
	disconnect->SetEnabled(false);
	becam_menubar->AddItem(actionsMenu);
	// Add the test menu
	#ifdef DEBUG	
		BMenu *debuggingMenu = new BMenu(_T("Debugging"));
		debuggingMenu->AddItem(new BMenuItem(_T("Open Statuswindow") , new BMessage(OPN_STATUS)));
		becam_menubar->AddItem(debuggingMenu);
	#endif	
	if(devicetype == TYPE_USB)
		actionsMenu->SetEnabled(false);
}

//
// MainWindow:: Create the Action Bar with the action controls
void BeCam_MainWindow::addActionBar ()
{
	BRect r;
	r.left = 5;
	r.right = r.left + 108;
	r.top = becam_gridview->Frame().bottom + 35;
	r.bottom = r.top + 30;
	// Create download button
	becam_download = new BButton(r, "download", _T("Download"),new BMessage(DOWN_BUTTON), B_FOLLOW_LEFT|B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW);
	becam_download->SetEnabled(false);
	becam_view->AddChild(becam_download);
	r.left = r.right + 20;
	r.right = r.left + 108;
	// Create delete button
	becam_delete = new BButton(r, "delete", _T("Delete"),new BMessage(DEL_BUTTON), B_FOLLOW_LEFT|B_FOLLOW_BOTTOM, B_NAVIGABLE|B_WILL_DRAW);
	becam_view->AddChild(becam_delete);
	becam_delete->SetEnabled(false);
	
}

//
// MainWindow:: Create the statusbar
void BeCam_MainWindow::addStatusBar ()
{
	// statusbar toevoegen
	BRect r = Bounds();
	r.top = becam_download->Frame().bottom + 5; 
	becam_winstatusbar = new BTextControl(
									r, 
									"winstat",
									NULL,
									"Status", 
									new BMessage(-1),
									B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM | B_FOLLOW_LEFT,
									B_WILL_DRAW | B_NAVIGABLE
									);
	becam_winstatusbar->SetEnabled(false);
	becam_view->AddChild(becam_winstatusbar);
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
	//
	if(becam_gridview->CurrentSelection() >= 0)
	{
		entry_ref refentry;
		int count,index=0,selectedindex=0;
		uint32 totalpics=0;
		char	tmpBuffer[100];
		BeCam_Item *LocaleItem;
		BMessage *cam_message;
		becam_winstatusbar->SetText(_T("Downloading items..."));
		becam_download->SetEnabled(false);
		becam_delete->SetEnabled(false);
		becam_extraMenu->SetEnabled(false);
		becam_downloadPopup->SetEnabled(false);
		//
		count = becam_gridview->CountItems();
		// create the status bar
		for(index=0;index < count;index++)
		{
			if(becam_gridview->IsItemSelected(index))
				totalpics++;	
		}
		sprintf(tmpBuffer,"Downloading number %ld of the %ld selected files",(uint32)0,totalpics);
		CreateStatusWindow(totalpics, tmpBuffer);
		UpdateStatusWindow(0, tmpBuffer);
		//
		for(index=0;index < count;index++)
		{
			if(becam_gridview->IsItemSelected(index))
			{
				selectedindex++;
				LocaleItem = (BeCam_Item *)becam_gridview->ItemAt(index);
				// Send a message to the camera interface 
				// to get the selected item
				cam_message = new BMessage(DOWN_ITEM);
				cam_message->AddInt32("itemhandle",(int32)LocaleItem->GetHandle());
				if(copyToDir != NULL)
					cam_message->AddRef("copyToDir", copyToDir);
				else
				{
					BEntry *entry = new BEntry(becam_downloadMenu->FindMarked()->Label());
					entry->GetRef(&refentry);
					cam_message->AddRef("copyToDir", &refentry);
				}
				// Wait untill the item has been downloaded
				BMessenger messenger(NULL,systemcore);
				BMessage reply;
				messenger.SendMessage(cam_message,&reply);
				if(reply.what == DOWN_ITEM_OK)
				{
					sprintf(tmpBuffer,"Downloading number %ld of the %ld selected files",(uint32)selectedindex,totalpics);
					UpdateStatusWindow(1,tmpBuffer);
				}
				delete(cam_message);
			}
		}
		CloseStatusWindow();
		becam_download->SetEnabled(true);
		becam_delete->SetEnabled(true);
		becam_extraMenu->SetEnabled(true);
		becam_downloadPopup->SetEnabled(true);
		becam_winstatusbar->SetText(_T("All items have been downloaded."));
	}
	else
	{
		BAlert *myAlert = new BAlert(_T("Download"), _T("Please select some files."),_T("Ok"), NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);
		myAlert->SetShortcut(0, B_ENTER);
		myAlert->Go();
	}
}
//
// MainWindow:: Remove the selected items
void BeCam_MainWindow::removeSelectedItems()
{
	//
	#ifdef DEBUG
		lfmainw = fopen(LOGFILE,"a");	
		fprintf(lfmainw,"MAINWINDOW - Begin remove Items\n");
		fclose(lfmainw);
	#endif
	if(becam_gridview->CurrentSelection() >= 0)
	{
		int count,index=0,selectedindex=0;
		uint32 totalpics=0;
		char	tmpBuffer[100];
		BMessage *cam_message;
		//
		becam_winstatusbar->SetText(_T("Removing items..."));
		becam_download->SetEnabled(false);
		becam_delete->SetEnabled(false);
		becam_extraMenu->SetEnabled(false);
		becam_downloadPopup->SetEnabled(false);
		// Ask the user if he/she is sure to remove the files.
		BAlert *myAlert = new BAlert(_T("Remove files"), _T("Are you sure you want to erase the selected files?"),_T("No"), _T("Yes"),NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);
		myAlert->SetShortcut(0, B_ESCAPE);
		uint32 button_index = myAlert->Go();
		if(button_index == 1)
		{
			count = becam_gridview->CountItems();
			#ifdef DEBUG
				lfmainw = fopen(LOGFILE,"a");	
				fprintf(lfmainw,"MAINWINDOW - %ld items should be removed\n",totalpics);
				fclose(lfmainw);
			#endif
			index = 0;
			BeCam_Item *selectedItem;
			while((index = becam_gridview->CurrentSelection(index)) >= 0)
			{
				selectedItem = (BeCam_Item *)becam_gridview->ItemAt(index);
				// Send a message to the camera 
				// to get the selected item
				cam_message = new BMessage(REM_ITEM);
				cam_message->AddInt32("itemhandle", (int32)selectedItem->GetHandle());
				// Wait untill the item has been downloaded
				BMessenger messenger(NULL,systemcore);
				BMessage reply;
				messenger.SendMessage(cam_message,&reply);
				if(reply.what == REM_ITEM_OK)
				{
					removeItem(selectedItem);
					delete(selectedItem);
					selectedItem = NULL;
					index = 0;
					delete(cam_message);
					continue;
				}
				delete(cam_message);
				index++;
			}
			becam_download->SetEnabled(true);
			becam_delete->SetEnabled(true);
			becam_extraMenu->SetEnabled(true);
			becam_downloadPopup->SetEnabled(true);	
			becam_winstatusbar->SetText(_T("All selected items have been removed."));
		}
	}
	else
	{	
		BAlert *myAlert = new BAlert(_T("Remove"), _T("Please select some files."),_T("Ok"), NULL,NULL,B_WIDTH_AS_USUAL,B_WARNING_ALERT);	
		myAlert->SetShortcut(0, B_ENTER);
		myAlert->Go();
	}
	#ifdef DEBUG
		lfmainw = fopen(LOGFILE,"a");	
		fprintf(lfmainw,"MAINWINDOW - End remove Items\n");
		fclose(lfmainw);
	#endif
			
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
		
		file = fopen(LOGFILE,"a");
		fprintf(file,errorMessage);
		fclose(file);
	}
	return(ErrorMes);
}

//		BeCam_MainWin::QuitRequested
//

bool BeCam_MainWindow::QuitRequested()
{
	if(becam_selectdirpanel)
		delete(becam_selectdirpanel);
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
void BeCam_MainWindow::CreateStatusWindow(int32 totalbytes, char *message)
{
	float xPos=0,yPos=0;
	
	if(!statusWindow)
	{
		CalculatePos(&xPos,&yPos,STATUS_WINDOW);
		statusWindow = new BeCam_StatusWindow(xPos,yPos,totalbytes, message,this);
	}
	statusWindow->reset(totalbytes);
	statusWindow->Show();
}
//
//		BeCam_MainWindow::Update the Status Window
void BeCam_MainWindow::UpdateStatusWindow(uint32 delta, char *message)
{
	if(statusWindow)
		statusWindow->updateStatus(delta, message);
}
//
//	BeCam_MainWindow:: Close the Status Window
void BeCam_MainWindow::CloseStatusWindow(void)
{
	//BMessage *enablemessage;
	if(statusWindow)
		statusWindow->Hide();
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
		case STATUS_WINDOW:
			*xPos = ((mainRect.left + length/2) -(WINDOW_WIDTH_STATUS/2));
			*yPos = ((mainRect.top + height/2) - (WINDOW_HEIGHT_STATUS/2));
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
	char statusmessage[250];
			
	switch(message->what)
		{
		
		case QUIT:
			systemcore->PostMessage(CAM_DISCON);
			systemcore->PostMessage(B_QUIT_REQUESTED);
			break;
		case CAM_CONNECTED:
			product = message->FindString("product");
			strcpy(statusmessage,_T("Connected to: "));
			becam_download->SetEnabled(true);
			becam_delete->SetEnabled(true);
			strcat(statusmessage,product);
			becam_winstatusbar->SetText(statusmessage);
			becam_connected = true;
			becam_extraMenu->SetEnabled(true);
			becam_downloadPopup->SetEnabled(true);
			break;
		case CAM_DISCONNECTED:
			product = message->FindString("product");
			strcpy(statusmessage,product);
			strcat(statusmessage,_T(" disconnected"));
			becam_download->SetEnabled(false);
			becam_delete->SetEnabled(false);
			becam_winstatusbar->SetText(statusmessage);
			becam_connected = false;	
			becam_downloadPopup->SetEnabled(false);
			becam_extraMenu->SetEnabled(false);
			clearItems();
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
				connect = actionsMenu->FindItem(_T("Connect"));	
				connect->SetEnabled(false);
				disconnect = actionsMenu->FindItem(_T("Disconnect"));	
				disconnect->SetEnabled(true);
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
			connect = actionsMenu->FindItem(_T("Connect"));
			connect->SetEnabled(true);	
			disconnect = actionsMenu->FindItem(_T("Disconnect"));
			disconnect->SetEnabled(false);
			break;
		}
		case DOWN_BUTTON:
			downloadSelectedItems();
			break;
		case SELECT_PATHMENU:
			becam_selectdirpanel->SetButtonLabel(B_DEFAULT_BUTTON,_T("Select"));	
			becam_selectdirpanel->Window()->SetWorkspaces(B_CURRENT_WORKSPACE);
			becam_selectdirpanel->Show();
			break;
		case DEL_BUTTON:
		case REM_ITEMS:
			removeSelectedItems();
			break;
		case OPN_STATUS:
		{	
			char	tmpBuffer[100];
			sprintf(tmpBuffer,"Downloading number %ld of the %ld selected files",(uint32)0,(uint32)0);
			CreateStatusWindow(0, tmpBuffer);
			break;
		}
		case OPEN_FILE_PANEL:
		{
			entry_ref dir;
			message->FindRef("refs", &dir);
			BEntry entry(&dir);
			BPath path(&entry);
			if(path.Path() != NULL)
			{
				defaultPath->SetLabel(path.Path());
				defaultPath->SetMarked(true);
				strncpy(pgsettings->defaultDownloadPath,path.Path(),B_FILE_NAME_LENGTH);
				BMessage *appmessage = new BMessage(SAVE_CONFIGURATION);
				systemcore->PostMessage(appmessage);
				//appmessage = new BMessage(RELOAD_CONFIGURATION);
				//systemcore->PostMessage(appmessage);
				delete(appmessage);
			}
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
		case B_COPY_TARGET:
		{
			entry_ref copyToDirDrag;
			message->FindRef("directory", &copyToDirDrag);
			downloadSelectedItems(&copyToDirDrag);	
			break;
		}
		case GET_CONFIGURATION:
		{
             // Get the settings from the system core
			BMessage reply;
			BMessage message(GET_CONFIGURATION);
			BMessenger messenger(NULL,systemcore);
			messenger.SendMessage(&message,&reply);
			reply.FindPointer("settings",(void **)&pgsettings);
			BMenuItem *morePath;
			defaultPath = new BMenuItem(pgsettings->defaultDownloadPath, NULL);
			defaultPath->SetMarked(true);
			becam_downloadMenu->AddItem(defaultPath);
			morePath = new BMenuItem(_T("Select new folder..."), new BMessage(SELECT_PATHMENU));	
			becam_downloadMenu->AddItem(morePath);
			BRect r = becam_downloadPopup->Frame();
			becam_view->RemoveChild(becam_downloadPopup);
			becam_downloadPopup = new BMenuField(r, "path", _T("Download Folder:"), 
											becam_downloadMenu,
											B_FOLLOW_BOTTOM,
											B_WILL_DRAW | B_NAVIGABLE);
			becam_downloadPopup->SetDivider(be_plain_font->StringWidth("Download Folder:") + 5);
			becam_downloadPopup->SetEnabled(false);
			becam_view->AddChild(becam_downloadPopup);
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
			lfmainw = fopen(LOGFILE,"a");	
			fprintf(lfmainw,"MAINWINDOW - device type is: %ld\n",devtype);
			fclose(lfmainw);
			#endif
			if(devtype == TYPE_PAR)
			{
				actionsMenu->SetEnabled(true);
			}
			else
			{
				actionsMenu->SetEnabled(false);
			}
			break;
		}
		default:
			BWindow::MessageReceived(message);
		}
}

