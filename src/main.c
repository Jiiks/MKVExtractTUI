/* main.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include <stdio.h>
#include <string.h>
#include "config.h"
#include <unistd.h>
#include <ncurses.h>
#include "utils.h"
#include "guimain.h"

int state = 0;

// Some command line args
int argHandler(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        if(strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            printf("%s%s\n", "MKV(Sub)Extract Terminal UI v", MKVE_VERSION);
            return 0;
        }
        if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-?") == 0) {
            printf(" %s [options] <input>\n\nOptions:\n", argv[0]);
            printf("  -i, --input %*c Specify input file or directory.\n", 12, ' ');
            printf("  -n, --nogui %*c Run guiless and extract all tracks that are tagged to auto check.\n", 12, ' ');
            printf("  -p, --pattern %*c Specify custom renaming pattern. Default \"fn.flags.lang.ext\"\n", 10, ' ');
            printf("  -l, --lang %*c Auto check all tracks matching language tags. Default: \"en,eng\"\n", 13, ' ');
            printf("  -a, --all %*c Auto check all tracks.\n", 14, ' ');
            printf("  -q, --quiet %*c Suppress output.\n", 12, ' ');

            printf("\n  --repo %*c Print repository url.\n", 17, ' ');
            printf("  --issues %*c Print issues url.\n", 15, ' ');
            printf("  --readme %*c Print readme url.\n", 15, ' ');
            
            printf("\n  -h, --help %*c Show this help.\n", 13, ' ');
            printf("  -v, --version %*c Show version information.\n\n", 10, ' ');
            //printf("%s\n", MKVE_REPO);
            return 0;
        }
        if(strcmp(argv[i], "--issues") == 0) {
            printf("%s\n", MKVE_ISSUES);
            return 0;
        }
        if(strcmp(argv[i], "--repo") == 0) {
            printf("%s\n", MKVE_REPO);
            return 0;
        }
        if(strcmp(argv[i], "--readme") == 0) {
            printf("%s\n", MKVE_README);
            return 0;
        }
    }
    return 1;
}

void extractFinished() {
    state = 2;
    while (getch() != ERR); // Clear the buffer
    usleep(10000);
}

int main(int argc, char *argv[]) {
    if(argc > 1 && argHandler(argc, argv) == 0) return 0;
    
    if(system("which mkvinfo > /dev/null 2>&1")) {
        printf("mkvinfo not found/installed\n");
        return 1;
    }
    if(system("which mkvmerge > /dev/null 2>&1")) {
        printf("mkvmerge not found/installed\n");
        return 1;
    }
 
    if(!cfgInit()) {
        printf("config init failed!\n");
        return 1;
    }

    // Get current directory
    if(!utilsGetCwd(g_cfg.cwd, sizeof(g_cfg.cwd))) return 0;
    // Custom input directory
    if(argc > 2) {
        if(strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--input")) {
            strcpy(g_cfg.wd, argv[2]);
        }
    }

    char title[48];
    snprintf(title, sizeof(title), "%s v%s", MKVE_WINDOW_TITLE, MKVE_VERSION);

    // Scan and sort files in workind dir or current working dir
    // Since generally a season is 10+ episodes initialize filelist with capacity of 16 to avoid unnecessary realloc.
    FileList fl = fsScanDir(g_cfg.wd[0] != '\0' ? g_cfg.wd : g_cfg.cwd, ".mkv", 16);
    if(fl.size <= 0) {
        printf("No suitable files in: %s\n", g_cfg.wd[0] != '\0' ? g_cfg.wd : g_cfg.cwd);
       // return 0;
    }

    fsSortList(&fl);
    //int fileCount = fl.size;

    guiMainInit(title, &fl);
    guiMainUpdate();

    int ch;
    while (1) {
        ch = getch();
        if (ch == ERR) {
            usleep(10000);
            continue;
        }
        if (ch == 27) { // ESC key
            break;
            guiMainAbortExtract(extractFinished);
            state = 2;
            continue; // Immediately continue the loop after abort
        }

        if(state == 2) {
            if(ch == KEY_BACKSPACE) {
                guiMainBackSpace();
                state = 0;
            }
            usleep(10000);
            continue;
        }

        if (ch == 'q') break;

        switch (ch) {
            case KEY_PPAGE:
                guiSidebarSelect(-1);
                break;
            case KEY_NPAGE:
                guiSidebarSelect(1);
                break;
            case KEY_HOME:
                guiSidebarSelect(0);
                break;
            case KEY_UP:
                guiMainSelect(-1);
                break;
            case KEY_DOWN:
                guiMainSelect(1);
                break;
            case ' ':
                guiMainCheck(0);
                break;
            case 'a':
                guiMainCheck(1);
                break;
            case 'd':
                guiMainCheck(-1);
                break;
            case KEY_ENTER:
            case 10:
                state = 1;
                guiMainExtract(extractFinished); // Start extraction
                break;
            case KEY_BACKSPACE:
                guiMainBackSpace();
                break;
        }
    }

    fsCleanup();
    guiMainClean();
    fsFreeList(&fl);

    return 0;
}
