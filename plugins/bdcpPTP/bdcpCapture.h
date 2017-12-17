/* bdcpCapture.h */

#ifndef _BDCPCAPTURE_
#define _BDCPCAPTURE_
#include <Window.h>
#include <View.h>
#include <Slider.h>
#include <Locker.h>
#include <CheckBox.h>
#include <Button.h>
#include <StringView.h>
#include "BitmapView.h"
#include "PTPFunctions.h"

#define msgShutter      'bnSH'
#define msgZoom         'slZM'
#define msgSaveDir      'bnSF'
#define msgLeave        'cbLv'
#define msgSaveDirSel   'sDSl'

#define PULSE_RATE      60000
class PTPCamera;

class PTPCaptView : public BView {
public:
    PTPCaptView(BRect frame, PTPCamera* Camera);
    ~PTPCaptView();
    virtual void            AttachedToWindow();
    virtual void            MessageReceived(BMessage *msg);
    virtual void            Pulse();
    void                    Shoot();
    PTPCamera*              Cam;
    
private:
    PTPController<uint16>*  Zoom;
    BBitmap*                ViewFinder;
    BitmapView*             ViewFinderView;
    BButton*                bnShutter;
    BSlider*                slZoom;
    BButton*                bnSaveDir;
    BStringView*            lbStatus;
    BStringView*            lbSaveDir;
    int                     captureCount;
    BPath                   saveDir;
};

class PTPCaptWindow : public BWindow {
public:
							PTPCaptWindow(PTPCamera* Camera); 
    virtual void			WindowActivated(bool focus);
	virtual status_t		InitCheck() { return fStatus; }
    virtual bool			QuitRequested();
    void					Go();
private:
	PTPCamera*				fCamera;
    PTPCaptView*			CaptView;
    sem_id					waitEndCapture;
	status_t				fStatus;
};

#endif
