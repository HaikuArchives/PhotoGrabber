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
 * File:   IconView.h
 * Author: Kai Niessen <kai.niessen@online.de>
 *
 * Created on April 22, 2017, 6:26 PM
 */

#ifndef ICONVIEW_H
#define ICONVIEW_H
#include <View.h>
#include <Bitmap.h>

class IconView : public BView {
public:
	IconView(const char* name);
	virtual ~IconView();
    void                            SetIcon(BBitmap* icon);
    BBitmap*                        Icon();
    virtual void                    Draw(BRect updateRect);
private:
    BBitmap*                        fIcon;
};

#endif /* ICONVIEW_H */

