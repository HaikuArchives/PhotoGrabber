/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
// File definition
#ifndef ITEM_H
#define ITEM_H

//		Includes
#include <interface/ListView.h>
#include <interface/Region.h>
#include <stdlib.h>
//
//		Local Includes
#include "intf_global.h"
#include "core_global.h"
#include "ItemData.h"

//		define classes
//
class PicItem {
public:
                                    PicItem(ItemData *data);
                                    PicItem();
                                    ~PicItem();
    const char*                     Name();
    const char*                     Date();
    uint32                          Handle();
    uint32                          Size();
    BBitmap*                        ThumbBitmap();
    char*                           ItemPath();
    uint32                          Status();
    uint32                          Orientation();
    float                           Height() const;
    float                           Width() const;
    float                           ThumbHeight() const;
    float                           ThumbWidth() const;
    void                            RotateThumb(uint32 angle);
		
private:
    ItemData*                       itemdata;
};

#endif


