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

// Some command line args
int argHandler(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        if(strcmp(argv[i], "-version") == 0 || strcmp(argv[i], "-v") == 0) {
            printf("%s%s\n", "MKV(Sub)Extract Terminal UI v", MKVE_VERSION);
            return 0;
        }
        if(strcmp(argv[i], "-help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("%s\n", MKVE_REPO);
            return 0;
        }
        if(strcmp(argv[i], "-issues") == 0) {
            printf("%s\n", MKVE_ISSUES);
            return 0;
        }
        if(strcmp(argv[i], "-readme") == 0) {
            printf("%s\n", MKVE_README);
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if(argc > 1 && argHandler(argc, argv) == 0) return 0; 
    if(!cfgInit()) return 0;

    // Get current directory
    if(!utilsGetCwd(g_cfg.cwd, sizeof(g_cfg.cwd))) return 0;
    // Custom input directory
    if(argc > 2) {
        if(strcmp(argv[1], "-i") == 0) {
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
    int fileCount = fl.size;

    guiMainInit(title, &fl);
    guiMainUpdate();

    int ch;
    while(1) {
        ch = getch();
        if(ch == 'q') break;
        if(ch == KEY_PPAGE) {
            guiSidebarSelect(-1);
        }
        if(ch == KEY_NPAGE) {
            guiSidebarSelect(1);
        }
        if(ch == KEY_HOME) {
            guiSidebarSelect(0);
        }
        if(ch == KEY_UP) {
            guiMainSelect(-1);
        }
        if(ch == KEY_DOWN) {
            guiMainSelect(1);
        }
    }
    fsCleanup();
    guiMainClean();
    fsFreeList(&fl);

    return 0;
}