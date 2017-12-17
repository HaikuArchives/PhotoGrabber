/*  bdcpCapture.cpp */

#include <TextView.h>
#include <TranslationKit.h>
#include <Button.h>
#include <Window.h>
#include <FindDirectory.h>
#include <Cursor.h>
#include <Entry.h>
#include <FilePanel.h>
#include "bdcpPTP.h"
#include "bdcpCapture.h"

PTPCaptView::PTPCaptView(BRect frame, PTPCamera* Camera) : 
    BView(frame, 
        "CaptView", 
        B_FOLLOW_ALL_SIDES, 
        B_WILL_DRAW | B_NAVIGABLE | B_PULSE_NEEDED) 
{
    Cam = Camera;
    captureCount = 0;
    Zoom = NULL;
    find_directory(B_DESKTOP_DIRECTORY, &saveDir);
    
    SetViewColor(100, 100, 80);
    SetHighColor(255, 255, 255);
    
    BRect r = BRect(0, 0, 319, 239);
    ViewFinder = new BBitmap(r, B_RGB32);
    r.OffsetTo(5, 5);
    ViewFinderView = new BitmapView(ViewFinder, r, "viewViewFinder");
    AddChild(ViewFinderView);

    r = Bounds();
    r.top = r.bottom - 22;
    lbStatus = new BStringView(r, "status", NULL, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM);
    AddChild(lbStatus);
    lbStatus->SetViewColor(0,0,255);
    lbStatus->SetLowColor(255,255,255);
    lbStatus->SetHighColor(255, 255, 255);
    
    r.Set(330, 5, 480, 28);
    bnShutter = new BButton(r, "bnShutter", "Capture", new BMessage(msgShutter), B_FOLLOW_LEFT | B_FOLLOW_RIGHT | B_FOLLOW_TOP);
    AddChild(bnShutter);

    if (Camera->deviceInfo->VendorExtensionId == PTP_VENDOR_CANON) {
        r.OffsetBy(0, 30);
        r.bottom +=10; 
        slZoom = new BSlider(r, "slZoom", "Zoom", new BMessage(msgZoom), 7, 80, B_HORIZONTAL, B_BLOCK_THUMB, B_FOLLOW_LEFT | B_FOLLOW_TOP | B_FOLLOW_RIGHT, B_NAVIGABLE | B_WILL_DRAW | B_FRAME_EVENTS);
        AddChild(slZoom);
        r.top +=10;
    } else slZoom = NULL;
    
    r.OffsetBy(0, 32);
    r.bottom = r.top + 20;
    r.right -= 30;
    lbSaveDir = new BStringView(r, "lbSaveDir", saveDir.Path(), B_FOLLOW_LEFT | B_FOLLOW_TOP | B_FOLLOW_RIGHT);
    AddChild(lbSaveDir);
    
    r.right += 30;
    r.left = r.right - 25;
    bnSaveDir = new BButton(r, "bnSaveDir", "...", new BMessage(msgSaveDir), B_FOLLOW_TOP | B_FOLLOW_RIGHT);
    AddChild(bnSaveDir);
}

PTPCaptView::~PTPCaptView() {
    if (Zoom) delete Zoom;
}

void PTPCaptView::AttachedToWindow() {
    bnShutter->SetTarget(this);
    if (slZoom) {
        slZoom->SetTarget(this);
        Zoom = (PTPController<uint16>*)Cam->GetController(PTP_DPC_CANON_Zoom);
        if (Zoom) {
            slZoom->SetLimits(Zoom->Min(), Zoom->Max());
            slZoom->SetKeyIncrementValue(Zoom->Step());
            slZoom->SetValue(Zoom->Value());
        }
    }

    bnSaveDir->SetTarget(this);
}

void PTPCaptView::MessageReceived(BMessage* msg) {
    switch (msg->what) {
        case msgShutter:
            Shoot();
            break;
        case msgZoom: {
            Zoom->SetValue(slZoom->Value());
            break;
        }
        case msgSaveDir: {
            entry_ref ref;
            BEntry entry(saveDir.Path());
            entry.GetRef(&ref);
            BFilePanel* dlg = new BFilePanel(B_OPEN_PANEL, new BMessenger(this), &ref, B_DIRECTORY_NODE, false, new BMessage(msgSaveDirSel));
            dlg->Show();
            dlg = NULL;
            break;
        }
        case msgSaveDirSel: {
            entry_ref ref;
            status_t status;
            if ((status = msg->FindRef("refs", &ref)) == B_OK) {
                saveDir.SetTo(&ref);
                lbSaveDir->SetText(saveDir.Path());
            }
            break;
        }
        default:
            BView::MessageReceived(msg);
    }
}

void PTPCaptView::Pulse() {
    if (Cam->isViewFinding) {
        BBitmap *bmp;
        Window()->LockLooper();
        Window()->SetPulseRate(0);
        Cam->GetViewFinderBitmap(&bmp);
        ViewFinderView->SetViewBitmap(bmp);
        Window()->SetPulseRate(PULSE_RATE);
        Window()->UnlockLooper();
        delete bmp;
    }
}

void PTPCaptView::Shoot() {
    lbStatus->SetText("Taking Picture...");
    lbStatus->Invalidate();

    BPath path;
    BString fName;
    status_t status = B_FILE_EXISTS;
    while (status == B_FILE_EXISTS) {
        path = saveDir;
        fName.SetToFormat("Capture%03d.jpg", captureCount);
        path.Append(fName);
        BEntry entry(path.Path());
        if (entry.Exists()) 
            captureCount++;
        else
            status = B_OK;
    }
    
    Window()->LockLooper();
    DEBUG("Take Picture: Pausing Viewfinder...\n");
    Window()->SetPulseRate(0);
    snooze(4 * PULSE_RATE);
    Cam->SetViewFinding(false);
    DEBUG("Take Picture: Executing Take Picture...\n");
    snooze(4 * PULSE_RATE);
    if (Cam->TakePicture(&path) >= B_OK) {
        BString msg;
        msg.SetToFormat("Take Picture: Saved image as %s", path.Path());
        lbStatus->SetText(msg);
        captureCount++;
    } else {
        lbStatus->SetText("Unable to take shot.");
    }
    DEBUG("Take Picture: Waiting a second...\n");
    snooze(10000);
    DEBUG("Take Picture: Restarting Viewfinder...\n");
    Cam->SetViewFinding(true);
    DEBUG("Take Picture: Restarting Pulse...\n");
    Window()->SetPulseRate(PULSE_RATE);
    Window()->UnlockLooper();
}

PTPCaptWindow::PTPCaptWindow(PTPCamera* Camera)
  : BWindow(BRect(100, 100, 585, 360), "Capture", B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE),
	fCamera(Camera),
	fStatus(B_OK)	
{
    fStatus = fCamera->SetShooting(true);
	if (fStatus < B_OK) 
		return;

    fStatus = fCamera->SetViewFinding(true);
	if (fStatus < B_OK) {
		fCamera->SetShooting(false);
		return;
	}
	
    CaptView = new PTPCaptView(Bounds(), Camera);
    AddChild(CaptView);
    waitEndCapture = create_sem(0, "waitEndCapture");
}

void PTPCaptWindow::WindowActivated(bool focus) {
    if (focus) {
        SetPulseRate(PULSE_RATE);
    } else {
        SetPulseRate(0);
    }
}

bool PTPCaptWindow::QuitRequested() {
    SetPulseRate(0);
    snooze(PULSE_RATE);
    if (fCamera->isViewFinding)
        fCamera->SetViewFinding(false);
    if (fCamera->isShooting)
        fCamera->SetShooting(false);
        
    delete_sem(waitEndCapture);
    return true;
}

void PTPCaptWindow::Go() {
    Show();
    while (acquire_sem(waitEndCapture) == B_INTERRUPTED)
        snooze(PULSE_RATE);
}

