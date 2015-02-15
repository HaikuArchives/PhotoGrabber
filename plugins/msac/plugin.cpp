#include <cstdio>
#include <iostream.h>

/*
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    http://www.gnu.org/copyleft/gpl.html
*/

#include "debug.h"

#include "preferences.h"
#include "configplugin.h"

#include "plugin.h"

int get_BDCP_API_Revision(void)
{
	return(2);
}

void getPluginVersion(version_info &ver) {
	ver.major 		= 1;
	ver.middle 		= 0;
	ver.minor 		= 0;
	ver.variety 	= 0;
	ver.internal	= 0;
	sprintf(ver.short_info, "Copyright 2000 Chris Adams");
	sprintf(ver.long_info, "BDCP Sony Memory Stick Reader Plugin.");
}

void getSupportedCameras(vector<string> & listofcams) {
	listofcams.push_back("Sony Serial Memory Stick Reader (MSAC-SR1)");
}

status_t openCamera(void) {
	LoadSettingsFromFile();
	stickReader = new MSAC_SR1(serialportsetting, speed);
	return(stickReader->init());
}

status_t closeCamera(void) {
	delete(stickReader);
	return(B_NO_ERROR);
}

status_t getNumberofPics(int &number) {
	number = stickReader->getNumberOfPics();
	return(B_NO_ERROR);
}

status_t setCurrentPicture(int picturenum) {
	return(stickReader->setCurrentPicture(picturenum));
}

status_t downloadPicture(BPath savedir) {
	return(stickReader->downloadPicture(savedir));
}

extern BLooper* msgtarget;

status_t setMessageTarget(BLooper* mtarget) {
	msgtarget = mtarget;
	return(B_NO_ERROR);
}

status_t deletePicture() {
	return(stickReader->deletePicture());
}

status_t takePicture() {
	return(B_ERROR);
}

BWindow* configurePlugin(BPoint middle) {
	return(new configplugin(BRect(middle.x - 74, middle.y - 122, middle.x + 74, middle.y + 122 )));
}

status_t getImageName(char* &name) {
	return(stickReader->getImageName(name));
}

status_t getImageSize(int &size) {
	return(stickReader->getImageSize(size));
}

status_t getImageDate(char* &date) {
	return(stickReader->getImageDate(date));
}

status_t getThumbnail(BBitmap* & bitmap) {
	return(stickReader->getThumbnail(bitmap));
}

status_t getLevel3FunctionNames(map<char*, char*> &menunames) {
	menunames.clear();
	return(B_NO_ERROR);
}