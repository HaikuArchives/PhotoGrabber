/*
	Copyright 1999, Be Incorporated.   All Rights Reserved.
	This file may be used under the terms of the Be Sample Code License.
*/

#include <Application.h>
#include <Bitmap.h>
#include <stdio.h>
#include <Roster.h>
#include <TranslationUtils.h>
#include <Resources.h>
#include <File.h>
#include <DataIO.h>

#include "bdcpPTP.h"
#include "BitmapView.h"

BitmapView::BitmapView(BBitmap* bitmap, BRect frame, const char* name,
	uint32 resizingMode, uint32 flags)
: BView(frame, name, resizingMode, flags) 
{ 
    this->bitmap = NULL;
    preferredSize.Set(0,0);
    SetDrawingMode(B_OP_ALPHA);
    SetViewBitmap(bitmap);
    ResizeToPreferred();
}

BitmapView::BitmapView(type_code resourceType, uint32 resourceId, const char* name,
	uint32 resizingMode, uint32 flags)
: BView(BRect(0,0,64,64), name, resizingMode, flags) 
{
    this->bitmap = NULL;
    SetDrawingMode(B_OP_ALPHA);

    // Find Background picture
    entry_ref imageRef;
    be_roster->FindApp(APP_SIGNATURE, &imageRef);
    BFile imageFile(&imageRef, B_READ_ONLY);
    BResources resources;
    resources.SetTo(&imageFile);
    if (resources.InitCheck() == B_OK) {
        size_t picSize;
        const void* picData = resources.LoadResource(resourceType, resourceId, &picSize);
        BMemoryIO picStream(picData, picSize);
        BBitmap* bitmap = BTranslationUtils::GetBitmap(&picStream);
        if (bitmap->IsValid()) {
            SetViewBitmap(bitmap);
        }
        delete bitmap;
        ResizeToPreferred();
    }
}

void BitmapView::ResizeToPreferred() {
    if (preferredSize.width > 0) ResizeTo(preferredSize.width, preferredSize.height);
}

BSize BitmapView::PreferredSize() {
    return preferredSize;
}

void BitmapView::AttachedToWindow() {
    if (bitmap) {
        SetViewBitmap(bitmap);
        delete bitmap;
        bitmap = NULL;
    }
}

void BitmapView::SetViewBitmap(BBitmap* bitmap) {
    if (LockLooper()) {
        if (bitmap && bitmap->IsValid()) {
            BView::SetViewBitmap(bitmap);
            preferredSize = bitmap->Bounds().Size();
        } else {
            ClearViewBitmap();
            preferredSize.Set(0,0);
        }
        UnlockLooper();
        this->bitmap = NULL;
    } else {
        this->bitmap = new BBitmap(bitmap);
        preferredSize = bitmap->Bounds().Size();
    }
}
