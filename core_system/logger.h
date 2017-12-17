void LogDebug(const char* fmt, ...);
void LogError(const char* fmt, ...);

#ifdef DEBUG
#define SHOW_LAYOUT                 'dShL'
#define SHOW_SD_HIDDEN              'dStH'
#define SHOW_SD_INIT                'dStI'
#define SHOW_SD_CONNECTED           'dStC'
#define SHOW_SD_DOWNLOAD            'dStD'

void FrameView(BView* v, bool recursive = true);
void FrameLayout(BLayout* layout, BView* parentView = NULL);
#endif
