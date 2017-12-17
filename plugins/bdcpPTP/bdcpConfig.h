/* bdcpConfig.h */

#include <Window.h>

class PTPCnfWindow : public BWindow {
public:
    PTPCnfWindow(BPoint middle);
    virtual bool QuitRequested();
    BBitmap* CameraPic;
};
