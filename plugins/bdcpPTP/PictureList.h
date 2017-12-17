// CanonPictures.h //

#ifndef CANONPICTURES
#define CANONPICTURES
#include <ObjectList.h>
#include <Bitmap.h>

enum picType {picUnknown, picJPEG, picOther, picMovie, picRaw};

#define PICTURE_NAME_MAX_SIZE	32
#define PICTURE_PATH_MAX_SIZE	128


class PictureDesc {
public:
								PictureDesc();
								~PictureDesc();
    char*						Name() { return name; };
    char*						Path() { return path; }
    uint32						Handle;
    tm							Date;
    uint32						Size;
    uint32						Orientation;
    bool						Flash;
    uint32						XRes;
    uint32						YRes;
    picType						Type;
    BBitmap*					thumb;
    
private:
    char						path[PICTURE_PATH_MAX_SIZE];
    char						name[PICTURE_NAME_MAX_SIZE];
};	

class PictureList : BObjectList<PictureDesc> {
public:
								PictureList();
								~PictureList();
    virtual bool				Add(PictureDesc *item);
    PictureDesc*				Item(int index) { return ItemAt(index); };
    PictureDesc*				CurrentItem();
    status_t					Remove(int index);
    status_t					Remove();
    status_t					Clear();
    int							Current();
    status_t					SetCurrent(int index);
    int							Count();
private:
    int							fCurrent;
};

#endif
