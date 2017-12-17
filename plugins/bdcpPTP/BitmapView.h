/*
        Copyright 1999, Be Incorporated.   All Rights Reserved.
        This file may be used under the terms of the Be Sample Code License.
 */

#ifndef _BitmapView_h
#define _BitmapView_h

#include <View.h>
#include <Locker.h>

class BitmapView : public BView {
public:
    BitmapView(BBitmap* bitmap, BRect frame, const char* name,
            uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
            uint32 flags = B_WILL_DRAW);
    BitmapView(type_code resourceType, uint32 resourceId, const char* name,
            uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
            uint32 flags = B_WILL_DRAW);
    virtual void AttachedToWindow();
    virtual void SetViewBitmap(BBitmap* bitmap);
    virtual void ResizeToPreferred();
    virtual BSize PreferredSize();
private:
    BSize preferredSize;
    BBitmap* bitmap;

};

#endif /* _BitmapView_h */
