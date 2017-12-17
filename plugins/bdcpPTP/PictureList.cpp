// PictureList.cpp //

#include "PictureList.h"

PictureDesc::PictureDesc() {
    path[0]         = 0;
    name[0]         = 0;
    Date.tm_year    = 0;
    Date.tm_mon     = 0;
    Date.tm_mday    = 0;
    Date.tm_hour    = 0;
    Date.tm_min     = 0;
    Date.tm_sec     = 0;
    Size            = 0;
    Orientation     = 1;
    Flash           = false;
    XRes            = 0;
    YRes            = 0;
    Type            = picUnknown;
    thumb           = NULL;
}

PictureDesc::~PictureDesc() {
    if (thumb)
        delete thumb;
}

PictureList::PictureList() 
    : 
    BObjectList<PictureDesc>()	
{
    fCurrent = -1;
}

PictureList::~PictureList() {
    MakeEmpty(true);
}

bool PictureList::Add(PictureDesc *item) {
    bool result =  BObjectList<PictureDesc>::AddItem(item);
    if (result && fCurrent == -1)
        fCurrent = 0;
    return result;
}

status_t PictureList::Remove(int index) {
    PictureDesc* item = BObjectList<PictureDesc>::RemoveItemAt(index);
    if (item) {
        delete item;
        if (fCurrent > index || fCurrent == CountItems())
            fCurrent--;
        return B_OK;
    } else return B_BAD_INDEX;
}

status_t PictureList::Remove() {
    return Remove(fCurrent); 
}

status_t PictureList::Clear() {
    MakeEmpty(true);
    return B_OK;
}
	
int PictureList::Current() {
    return fCurrent;
}

PictureDesc* PictureList::CurrentItem() {
    return ItemAt(fCurrent);
}

status_t PictureList::SetCurrent(int index)
{
    if (0 <= index && index < CountItems()) {
        fCurrent = index;
        return B_OK;
    } else return B_BAD_INDEX;
}

int PictureList::Count() {
    return CountItems();
}
