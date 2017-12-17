#include <stdarg.h>
#include <stdio.h>

#ifdef DEBUG

#include <Window.h>
#include <View.h>
#include <Layout.h>
#include <LayoutItem.h>

void LogDebug(const char* fmt, ...) {
    va_list arg_list;
    va_start(arg_list, fmt);
    vprintf(fmt, arg_list);
    va_end(arg_list);
    putchar('\n');
}
#define SHOW_LAYOUT                 'shwL'

void FrameLayout(BLayout* layout, BView* parentView) {
    if (!layout) return;
    for (int32 i = 0; i < layout->CountItems(); i++) {
        BLayoutItem* li = layout->ItemAt(i);
        BView* lv = parentView;
        if (!lv) lv = li->View();
        if (lv) {
            rgb_color highColor = lv->HighColor();
            lv->SetHighColor(make_color(0, 255, 0, 255));
            lv->StrokeRect(li->Frame());
            lv->SetHighColor(highColor);
        }
    }
}

void FrameView(BView* v, bool recursive = true) {
    if (v) {
        rgb_color highColor = v->HighColor();
        v->SetHighColor(make_color(255, 0, 0, 255));
        v->StrokeRect(v->Bounds());
        v->SetHighColor(highColor);
        if (recursive) 
            for (int32 i = 0; i < v->CountChildren(); i++) {
                FrameView(v->ChildAt(i));
                FrameLayout(v->GetLayout(), v);
            }
    }
}

#else
void LogDebug(const char* fmt, ...) {
}
#endif

void LogError(const char* fmt, ...) {
    va_list arg_list;
    va_start(arg_list, fmt);
    vprintf(fmt, arg_list);
    va_end(arg_list);
    putchar('\n');
}
