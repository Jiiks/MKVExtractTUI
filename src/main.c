/* main.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include <unistd.h>
#include <ncurses.h>
#include "utils.h"
#include "guimain.h"

#include <dirent.h>
#include <errno.h>

int state = 0;

// Some command line args
int argHandler(int argc, char *argv[]) {
    for (int i = 1; i < argc; ++i) {
        if(strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            printf("%s%s\n", "MKV(Sub)Extract Terminal UI v", MKVE_VERSION);
            printf("%s\n", "Check for updates?(Y/n)");
            int c = getchar();
            if(c == '\n' || c == 'y' || c == 'Y') {
                char command[128];
                char output[256];
                snprintf(command, sizeof(command), "curl -s https://api.github.com/repos/Jiiks/MKVExtractTUI/releases/latest | grep \"tag_name\" | cut -d':' -f2 | cut -d'\"' -f2");
                FILE *fp;
                fp = popen(command, "r");
                if (fp == NULL) {
                    perror("Error opening pipe");
                    return 0;
                }
                if (fgets(output, sizeof(output), fp) != NULL) {
                    printf("Current Version: %s Latest version: %s", MKVE_VERSION, output);
                } else {
                    fprintf(stderr, "No output from command\n");
                }
                if (pclose(fp) == -1) {
                    perror("Error closing pipe");
                    return 0;
                }
            }
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
            printf("  -f, --fastupdate %*c Fast update gui.\n", 7, ' ');

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

int init() {
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

    if(cfgLoad() != 0) return 1;
    return 0;
}

void extractFinished() {
    state = 2;
    while (getch() != ERR);
    usleep(10000);
}

int mainLoop() {
    int ch;
    while (1) {
        ch = getch();
        if (ch == ERR) {
            usleep(10000);
            continue;
        }
        if (ch == 27) { // ESC
            break;
            guiMainAbortExtract(extractFinished);
            state = 2;
            continue;
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
                guiMainExtract(extractFinished);
                break;
            case KEY_BACKSPACE:
                guiMainBackSpace();
                break;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if(argc > 1 && argHandler(argc, argv) == 0) return 0;
    if(init() != 0) return 1;

    // Get current directory
    if(!utilsGetCwd(g_cfg.cwd, sizeof(g_cfg.cwd))) return 1;
    // Custom input directory
    if(argc > 2)
        if(!cfgParseArgs(argc, argv)) return 1;
    //cfgPrintDbg();

    bool useWd = g_cfg.wd[0] != '\0';
    // Check that directory or file exists
    bool singleFile = strstr((useWd ? g_cfg.wd : g_cfg.cwd), ".mkv") != NULL;
    DIR *dir = NULL;
    int retries = 0;
    int lastErrno = 0; 
    while(retries < 5) {
        dir = opendir(useWd ? g_cfg.wd : g_cfg.cwd);
        if(dir == NULL && !singleFile) {
            lastErrno = errno;
        } else if (ENOENT == errno) {
            lastErrno = errno;
        } else {
            closedir(dir);
            dir = NULL;
            break;
        }
        mssleep(1000);
        retries++;
    }

    if(dir != NULL) {
        closedir(dir);
        dir = NULL;
        fprintf(stderr, "Failed to open directory: %s - %s \n", useWd ? g_cfg.wd : g_cfg.cwd, strerror(lastErrno));
        return 1;
    }

    char title[48];
    snprintf(title, sizeof(title), "%s v%s", MKVE_WINDOW_TITLE, MKVE_VERSION);

    // Scan and sort files in workind dir or current working dir
    // Since generally a season is 10+ episodes initialize filelist with capacity of 16 to avoid unnecessary realloc.
    FileList fl = fsScanDir(useWd ?  g_cfg.wd : g_cfg.cwd, ".mkv", 16, singleFile);

    if(fl.size <= 0) {
        printf("No suitable files in: %s\n",useWd ? g_cfg.wd : g_cfg.cwd);
    }

    if(!singleFile)
        fsSortList(&fl);

    guiMainInit(title, &fl);
    guiMainUpdate();

    int res = mainLoop();

    fsCleanup();
    guiMainClean();
    fsFreeList(&fl);

    return res;
}
