#ifndef NX_SHELL_GUI_H
#define NX_SHELL_GUI_H

#include <vector>
#include <string>
#include <SDL.h>
#include <switch.h>

#include "textures.h"
#include <vector>
#include "uri.h"

enum MENU_STATES {
    MENU_STATE_FILEBROWSER,
    MENU_STATE_OPTIONS,
    MENU_STATE_DELETE,
    MENU_STATE_PROPERTIES,
    MENU_STATE_SETTINGS,
    MENU_STATE_IMAGEVIEWER,
    MENU_STATE_ARCHIVEEXTRACT,
    MENU_STATE_TEXTREADER
};

typedef struct {
    MENU_STATES state = MENU_STATE_FILEBROWSER;
    int selected = 0;
    std::vector<FsDirectoryEntry> entries;
    std::vector<bool> checked;
    std::vector<bool> checked_copy;
    Uri checked_cwd;
    int checked_count = 0;
    s64 used_storage = 0;
    s64 total_storage = 0;
    std::vector<Tex> textures;
    long unsigned int frame_count = 0;
} MenuItem;

typedef struct {
	std::vector<u8> buffer;
} TextReader;

extern SDL_Window *window;
extern MenuItem item;
extern TextReader text_reader;

namespace GUI {
    inline void ResetCheckbox(void) {
		item.checked.clear();
		item.checked_copy.clear();
		item.checked.resize(item.entries.size());
		item.checked.assign(item.checked.size(), false);
		item.checked_cwd.clear();
		item.checked_count = 0;
	};

    int RenderLoop(void);
}

#endif
