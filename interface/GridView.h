/*
****************************************************************
* Copyright (c) 2004-2010,	Ramshankar, Jan-Rixt Van Hoye	   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// File definition
#ifndef GRIDVIEW_H
#define GRIDVIEW_H
//
// Includes
#include <View.h>
#include <ScrollView.h>
#include <Region.h>
#include <Control.h>
#include <ObjectList.h>
#include <vector>
#include <set>
//
// Local Includes
#include "PicItem.h"
//
// Defines
#define ITEM_NEXT	'INXT'
#define ITEM_BACK	'IBAK'
#define ITEM_SORT_BY_TITLE	0
#define ITEM_SORT_BY_DATE	1
//
// Classes
class GridView;
class BMessenger;
//
//	Struct 
struct list_tracking_data {
	GridView	*view;
	BPoint		start;
};

#define kDRAG_SLOP              4
#define DEFAULT_SELECTION_RADIUS 4
#define DEFAULT_ITEM_SPACING    20
#define DEFAULT_ITEM_WIDTH      160
#define DEFAULT_ITEM_HEIGHT     120

using namespace std;

typedef BObjectList<PicItem>    	PicList;
typedef signed long int         	listIndex;

class GridView : public BControl, public PicList {
public:
                                    GridView(const char* name, 
                                    BMessage* InvokeMsg = NULL, 
                                    BMessage* SelectMsg = NULL);
    virtual                         ~GridView();

    virtual void                    FrameResized (float newWidth, float newHeight);
    virtual void                    Draw(BRect rect);
    virtual void                    KeyDown(const char* bytes, int32 numBytes);
    virtual void                    MouseDown(BPoint pt);
    virtual void                    AttachedToWindow ();

    void                            AddList(BObjectList<PicItem>* list);
    bool                            AddItem(PicItem* item);
    bool                            RemoveItem(PicItem* item);
    PicItem*                        RemoveItemAt(listIndex index);
    void                            MakeEmpty();
    BRect                           ItemRect(listIndex index);
    void                            Select(listIndex index, bool extend = false);
    void                            SelectRange(listIndex fromIndex, listIndex toIndex);
    void                            SelectAll();
    void                            Deselect(listIndex index);
    void                            DeselectAll();
    void                            ScrollToSelection();
    long int                        SelectedIndex(int32 index = 0);
    PicItem*                        SelectedItem(int32 index = 0);
    BObjectList<PicItem>*           Selected();
    bool                            ItemSelected(listIndex index);
    bool                            ItemSelected(PicItem* item);
    int32                           CountSelected();	

    void                            SetSelectionCurveRadius(uint8 radius);
    uint8                           SelectionCurveRadius() const;

    void                            SetMinItemSpacing(BSize spacing);
    BSize                           MinItemSpacing();

    int32                           CountColumns() const;
    int32                           CountRows() const;

    void                            SendKeyStrokesTo(BLooper* looper,BHandler* handler = NULL);
    void                            SortItemsBy(int sortType);

    bool                            Layout();
private:
    void                            DrawItem(PicItem* item, BRect frame = BRect(0,0,-1,-1));
    void                            InvalidateItem(listIndex index);
    float                           GetTop(int32 row);
    float                           GetRowHeight(int32 row);
    bool                            HandleKeyMovement(const char* bytes, int32 numBytes);
    listIndex                       IndexAt(BPoint point);
    static status_t                 TrackItem(list_tracking_data *data);
    void                            ActionCopy(BMessage * request);
    static int                      CompareTitles(const PicItem* first, const PicItem* second);
    static int                      CompareDates(const PicItem* first, const PicItem* second);

    BSize                           fItemSize;	// height of item
    BSize                           fMinItemSpacing; // min horizontal margin
    BSize                           fItemSpacing;	// horizontal margin
    float                           fDetailHeight,
                                    fDetailSpacing;
        
    int32                           fColumnCount, 
                                    fRowCount,
                                    fRowsPerPage;
    vector<float>                   fRowTops;
    int8                            fSelectionRadius;
    BObjectList<PicItem>            fSelected;

    BScrollView*                    fScrollView;
    BMessage*                       fSelectMsg;
    BMessage*                       fInvokeMsg;
    BMessenger                      fTargetMessenger;
    BLooper*                        fKeyTargetLooper;
    BHandler*                       fKeyTargetHandler;
};

#endif
