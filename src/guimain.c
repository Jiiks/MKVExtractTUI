/* guimain.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include "guimain.h"
#include <signal.h>
#include <ncurses.h>

#define SIDEBAR_RATIO 4
#define SIDEBAR_MIN_WIDTH 30
#define BOTTOM_PAD 4
#define FILES_HEADER "Files pgup/pgdn/home"
#define TRACKS_HEADER "Tracks up/down - space"
#define FOOTER_TEXT "(q)uit | select (a)ll | (d)eselect all | +/- auto select lang"
const char *extractText = " ENTER TO EXTRACT ";

typedef struct {
    WINDOW *sidebar;        // Sidebar to display files in dir.
    WINDOW *sidebarPad;     // Pad to enable sidebar scrolling
    WINDOW *main;           // Main container
    WINDOW *mainPad;        // Pad to enable main scrolling
    WINDOW *footer;         // Footer container
    FileList *fileList;     // Files in dir
    const char *title;      // Window title

    int row;
    int col;

    int sidebarIdx;         // Current selected index in sidebar
    int sidebarWidth;       // Sidebar width
    int sidebarPadPos;      // Sidebar scroll index
    int sidebarMaxY;        // Sidebar height
    int sidebarMaxX;        // Sidebar width

    int mainIdx;            // Current selected index in main container
    int mainAreaWidth;      // Main area width
    int mainPadPos;         // Main container scroll index
    int mainMaxY;           // Main container height
    int mainMaxX;           // Main container width

    int footerEnd;          // End of footer for printing at the end
} guiMainCtx;

guiMainCtx ctx;

int guiGetSidebarIndex() { return ctx.sidebarIdx; }
int guiGetMainIndex() { return ctx.mainIdx ; }

// Resolve screen dimensions
void resolveDimensions(int *row, int *col, int *sidebarWidth, int *mainAreaWidth) {
    getmaxyx(stdscr, *row, *col);

    *sidebarWidth = *col / SIDEBAR_RATIO;
    if (*sidebarWidth < SIDEBAR_MIN_WIDTH) {
        *sidebarWidth = SIDEBAR_MIN_WIDTH;
    }
    *mainAreaWidth = *col - *sidebarWidth;

    ctx.sidebarMaxX = *sidebarWidth;
    ctx.sidebarMaxY = *row - BOTTOM_PAD;
    ctx.mainMaxX = *mainAreaWidth;
    ctx.mainMaxY = *row - BOTTOM_PAD;
}

void handleResize(int sig) {
    endwin();
    refresh();
    clear();

    int row, col, sidebarWidth, mainAreaWidth;
    resolveDimensions(&row, &col, &sidebarWidth, &mainAreaWidth);
    ctx.sidebarWidth = sidebarWidth;
    ctx.mainAreaWidth = mainAreaWidth;
    ctx.row = row;
    ctx.col = col;

    // Resize the windows
    wresize(ctx.sidebar, row - BOTTOM_PAD, sidebarWidth);
    wresize(ctx.main, row - BOTTOM_PAD, mainAreaWidth);
    // Move main to correct position
    mvwin(ctx.main, 1, sidebarWidth);
    // Move footer to correct position
    mvwin(ctx.footer, row - 3, 0);

    // Add some size if no files so we can print some error message.
    wresize(ctx.sidebarPad, ctx.fileList->size == 0 ? 20 : ctx.fileList->size, sidebarWidth);
    FileInfo *file = &ctx.fileList->files[ctx.sidebarIdx];
    wresize(ctx.mainPad, file->trackCount == 0 ? 20 : file->trackCount, mainAreaWidth);

    wclear(ctx.sidebarPad);
    wclear(ctx.sidebar);
    wclear(ctx.mainPad);
    wclear(ctx.main);
    wclear(ctx.footer);

    // Update title and refresh
    mvprintw(0, 1, "%s", ctx.title);
    refresh();
    
    ctx.footerEnd = col - strlen(extractText);
    guiMainUpdate();
}

// ncurses initializer
void initNcurses() {
    initscr();              // Init screen
    start_color();          // Enable colors
    noecho();               // Don't echo inputs
    cbreak();               // Disables line buffering and erase/kill character-processing 
    curs_set(0);            // Disable cursor
    keypad(stdscr, TRUE);   // Enable keyboard input
}

void guiMainInit(const char *title, FileList *fileList) {
    ctx.fileList = fileList;
    ctx.title = title;
    ctx.sidebarPad = 0;
    ctx.sidebarIdx = 0;
    ctx.mainPad = 0;
    ctx.mainIdx = 0;
    initNcurses();
    mvprintw(0, 1, "%s", title);
    refresh();

    int row, col, sidebarWidth, mainAreaWidth;
    resolveDimensions(&row, &col, &sidebarWidth, &mainAreaWidth);
    ctx.sidebarWidth = sidebarWidth;
    ctx.mainAreaWidth = mainAreaWidth;
    ctx.row = row;
    ctx.col = col;

    ctx.sidebar = newwin(row - BOTTOM_PAD, sidebarWidth, 1, 0);
    // Add some size if no files so we can print some error message.
    ctx.sidebarPad = newpad(fileList->size == 0 ? 20 : fileList->size, sidebarWidth);

    ctx.main = newwin(row - BOTTOM_PAD, mainAreaWidth, 1, sidebarWidth);

    ctx.mainPad = newpad(40, mainAreaWidth);

    ctx.footer = newwin(3, col, row - 3, 0);
    ctx.footerEnd = col - strlen(extractText);

    guiMainUpdate();
    signal(SIGWINCH, handleResize);
}

void guiMainUpdate() {
    box(ctx.sidebar, 0, 0);
    box(ctx.main, 0, 0);

    mvwprintw(ctx.sidebar, 0, 1, " %s ", FILES_HEADER);
    
    mvwprintw(ctx.main, 0, 1, " %s ", TRACKS_HEADER);

    mvwprintw(ctx.footer, 1, 1, "%s", FOOTER_TEXT);
    wattron(ctx.footer, A_BOLD | A_STANDOUT);
    mvwprintw(ctx.footer, 1, ctx.footerEnd, " ENTER TO EXTRACT ");
    wattroff(ctx.footer, A_BOLD | A_STANDOUT);

    wrefresh(ctx.sidebar);
    wrefresh(ctx.main);
    wrefresh(ctx.footer);
    guiMainUpdateSideBar();
    guiMainUpdateMain();
}

void guiTrimText(const char *input, char *output, int max_length) {
    if (strlen(input) <= max_length) {
        strcpy(output, input);
    } else {
        strncpy(output, input, max_length - 3);
        output[max_length - 3] = '\0';
        strcat(output, "...");
    }
}

void guiMainUpdateSideBar() {
    int maxIndex = ctx.sidebarMaxY - 2;
    char text[ctx.sidebarMaxX - 2]; 
    if(ctx.fileList->size <= 0) {
        mvwprintw(ctx.sidebarPad, 0, 1, "No files found in: \n %s", ctx.fileList->path);
    } else {
        for(int i = 0 ; i < ctx.fileList->size ; i++) {
            if(ctx.sidebarIdx == i) {
                wattron(ctx.sidebarPad, A_BOLD | A_STANDOUT);
            }
            snprintf(text, sizeof(text), "%-*s", ctx.sidebarMaxX, ctx.fileList->files[i].name);
            mvwprintw(ctx.sidebarPad, i, 1, "%s", text);
            wattroff(ctx.sidebarPad, A_BOLD | A_STANDOUT);
        }
    }

    prefresh(ctx.sidebarPad, ctx.sidebarPadPos, 0, 2, 1, maxIndex + 1, ctx.sidebarMaxX - 2);
    guiMainUpdateMain();
}

void displayFlags(const char *input, char *output) {
    size_t len = strlen(input);
    size_t last_dot_index = 0;

    for (size_t i = 0; i < len; i++) {
        if (input[i] == '.') {
            last_dot_index = i;
        }
    }

    for (size_t i = 0; i < len; i++) {
        if (input[i] == '.' && i != last_dot_index) {
            output[i] = '|';
        } else if (input[i] == '.' && i == last_dot_index) {
            output[i] = '\0';
        } else {
            output[i] = input[i];
        }
    }
    output[len] = '\0';
}

void guiMainUpdateMain() {
    FileInfo *fi = &ctx.fileList->files[ctx.sidebarIdx];
    wclear(ctx.mainPad);
    // Track count is 0 so load all tracks
    if(fi->trackCount == 0) {
        mvwprintw(ctx.mainPad, 0, 0, "Loading tracks...");
        prefresh(ctx.mainPad, ctx.mainPadPos, 0, 2, ctx.sidebarWidth + 2, ctx.row - BOTTOM_PAD - 1, ctx.sidebarWidth + ctx.mainAreaWidth - 1);
 
        cJSON *json = fsGetTracksJson(fi);
        cJSON *tracks = cJSON_GetObjectItemCaseSensitive(json, "tracks");
        cJSON *track = NULL;
        int row = 0;
        wclear(ctx.mainPad);

        int trackCount = cJSON_GetArraySize(tracks);
        // Allocate tracks
        if(trackCount > 0) {
            if(fi->tracks != NULL) {
                free(fi->tracks);
                fi->tracks = NULL;
            }
            fi->tracks = malloc(trackCount * sizeof(Track));
        }

        cJSON_ArrayForEach(track, tracks) {
            if(track == NULL) {
                mvwprintw(ctx.mainPad, ctx.sidebarWidth, 2, "Track is null: %d", row);
                row += 1;
                continue;
            }
            
            cJSON *type = cJSON_GetObjectItemCaseSensitive(track, "type");
            // Ignore non sub tracks since we don't currently care about those
            if(strstr(type->valuestring, "subtitles") == NULL) continue;
            Track parsedTrack = trackParseJson(track);
            fsAddTrack(fi, &parsedTrack, row);
            row += 1;
        }
        cJSON_Delete(json);
    }
    if(fi->trackCount == 0) {
        wclear(ctx.mainPad);
        mvwprintw(ctx.mainPad, 0, 0, "No tracks");
    } else {
        wclear(ctx.mainPad);
        for(int i = 0 ; i < fi->trackCount ; i++) {
            Track track = fi->tracks[i];
            char newName[256];
            trackResolveNewName(fi->name, &track, newName);
            char df[256];
            displayFlags(track.Flags, df);
     
            if(i == fi->selectedIndex) wattron(ctx.mainPad, A_BOLD | A_STANDOUT);
            mvwprintw(ctx.mainPad, i, 0, "[%c] #%d [%s] (%s) %*s => %s",
                track.Extract ? 'x' : ' ',
                track.Idx,
                track.Language,
                df,
                fi->lt - (int)strlen(df), " ",
                newName
            );
            wattroff(ctx.mainPad, A_BOLD | A_STANDOUT);
        }
    }
    prefresh(ctx.mainPad, ctx.mainPadPos, 0, 2, ctx.sidebarWidth + 2, ctx.row - BOTTOM_PAD - 1, ctx.sidebarWidth + ctx.mainAreaWidth - 5);
}

void guiSidebarSelect(int dir) {
    int count = ctx.fileList->size;
    if(count <= 0) {
        guiMainUpdate();
        return;
    }

    if(dir == 0) {
        ctx.sidebarIdx = 0;
        ctx.sidebarPadPos = 0;
        guiMainUpdateSideBar();
        return;
    }

    if (dir == -1) {
        ctx.sidebarIdx = (ctx.sidebarIdx - 1 + count) % count;
    } else {
        ctx.sidebarIdx = (ctx.sidebarIdx + 1) % count;
    }

    int maxIndex = ctx.sidebarMaxY - 2;
    if (dir == 1 && ctx.sidebarIdx - ctx.sidebarPadPos >= maxIndex) {
        ctx.sidebarPadPos++;
    }
    if (dir == -1 && ctx.sidebarIdx + 1 <= ctx.sidebarPadPos) {
        ctx.sidebarPadPos--;
    }

    if (ctx.sidebarIdx == 0) ctx.sidebarPadPos = 0;
    if (ctx.sidebarIdx + 1 >= count) ctx.sidebarPadPos = count - 1;

    if (ctx.sidebarPadPos < 0) ctx.sidebarPadPos = 0;  
    if (ctx.sidebarPadPos > count - ctx.sidebarMaxY + 2) ctx.sidebarPadPos = count - ctx.sidebarMaxY + 2;

    guiMainUpdateSideBar();
    //guiMainUpdateMain();
}

void guiMainSelect(int dir) {
    FileInfo *fi = &ctx.fileList->files[ctx.sidebarIdx];

    int count = fi->trackCount;
    if(count <= 0) {
        guiMainUpdate();
        return;
    }

    if (dir == -1) {
        fi->selectedIndex = (fi->selectedIndex - 1 + count) % count;
    } else {
        fi->selectedIndex = (fi->selectedIndex + 1) % count;
    }

    int maxIndex = ctx.mainMaxY - 2;
    if (dir == 1 && fi->selectedIndex - ctx.mainPadPos >= maxIndex) {
        ctx.mainPadPos++;
    }
    if (dir == -1 && fi->selectedIndex + 1 <= ctx.mainPadPos) {
        ctx.mainPadPos--;
    }

    if (fi->selectedIndex == 0) ctx.mainPadPos = 0;
    if (fi->selectedIndex + 1 >= count) ctx.mainPadPos = count - 1;

    if (ctx.mainPadPos < 0) ctx.mainPadPos = 0;  
    if (ctx.mainPadPos > count - ctx.mainMaxY + 2) ctx.mainPadPos = count - ctx.mainMaxY + 2;

    guiMainUpdateMain();
}

void guiMainClean() {
    if(ctx.sidebarPad != NULL) {
        delwin(ctx.sidebarPad);
        ctx.sidebarPad = NULL;
    }
    
    if(ctx.sidebar != NULL) {
        delwin(ctx.sidebar);
        ctx.sidebar = NULL;
    }

    if(ctx.mainPad != NULL) {
        delwin(ctx.mainPad);
        ctx.mainPad = NULL;
    }

    if(ctx.main != NULL) {
        delwin(ctx.main);
        ctx.main = NULL;
    }

    if(ctx.footer != NULL) {
        delwin(ctx.footer);
        ctx.footer = NULL;
    }

    endwin();
}
