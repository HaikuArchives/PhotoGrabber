/*
 * Copyright (C) 2017 Kai Niessen <kai.niessen@online.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* 
 * File:   IconView.cpp
 * Author: Kai Niessen <kai.niessen@online.de>
 * 
 * Created on April 22, 2017, 6:26 PM
 */

#include "IconView.h"

IconView::IconView(const char* name)
  : BView(name, B_WILL_DRAW),
        fIcon(NULL)
{
}

IconView::~IconView() {
}

void 
IconView::SetIcon(BBitmap* icon) {
    if (fIcon == icon) return;
    fIcon = icon;
    if (fIcon) {
        BSize size(fIcon->Bounds().Size());
        ResizeTo(size);
        SetExplicitMinSize(size);
        SetExplicitMaxSize(size);
        SetExplicitSize(size);
    }
    Invalidate();
}

BBitmap*
IconView::Icon() {
    return fIcon;
}

void 
IconView::Draw(BRect updateRect) {
    SetDrawingMode(B_OP_COPY);
    SetHighColor(ViewColor());
    FillRect(Bounds());
    SetDrawingMode(B_OP_BLEND);
    DrawBitmap(fIcon, fIcon->Bounds(), Bounds());
}