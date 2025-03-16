/* config.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#define CONFIG_FOLDER "MkvExtractTUI"
#define CONFIG_FILE "config.cfg"

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Config g_cfg;

// Reset config to default values
bool reset() {
    strcpy(g_cfg.autoCheck, "en,eng,");
    strcpy(g_cfg.format, "fn.flags.lang.ext");
    g_cfg.noGui = false;
    g_cfg.quiet = false;
    g_cfg.autoCheckAll = false;
    return true;
}

// Resolve paths for config file and config root.
bool resolvePaths() {
    // Try to get XDG_CONFIG_HOME
    const char *xdg_config_home = getenv("XDG_CONFIG_HOME");
    if(xdg_config_home) {
        snprintf(g_cfg.rootPath, 4096, "%s/%s", xdg_config_home, CONFIG_FOLDER);
        snprintf(g_cfg.configPath, 4096, "%s/%s/%s", xdg_config_home, CONFIG_FOLDER, CONFIG_FILE);
        #ifdef DEBUG
        printf("Found config dir in XDG_CONFIG_HOME\n");
        printf("Root Path:%s\n", g_cfg.rootPath);
        printf("File Path:%s\n", g_cfg.configPath);
        #endif
        return true;
    }

    // If not then get HOME
    const char *home = getenv("HOME");
    if(home) {
        snprintf(g_cfg.rootPath, 4096, "%s/.config/%s", home, CONFIG_FOLDER);
        snprintf(g_cfg.configPath, 4096, "%s/.config/%s/%s", home, CONFIG_FOLDER, CONFIG_FILE);
        #ifdef DEBUG
        printf("Found config dir in HOME\n");
        printf("Root Path:%s\n", g_cfg.rootPath);
        printf("File Path:%s\n", g_cfg.configPath);
        #endif
        return true;
    }

    // Config not found anywhere?
    fprintf(stderr, "\033[31mFailed to locate config in either XDG_CONFIG_HOME or HOME\033[0m\n");
    return false;
}

bool cfgInit() {
    if(!reset()) return false;
    if(!resolvePaths()) return false;
    return true;
}

bool cfgParseArgs(int argc, char *argv[]) {
    for(int i = 1 ; i < argc ; i++) {
        if(strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--nogui") == 0) {
            g_cfg.noGui = true;
            continue;
        }
        if(strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--quiet") == 0) {
            g_cfg.quiet = true;
            continue;
        }
        if(strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--all") == 0) {
            g_cfg.autoCheckAll = true;
            continue;
        }

        // Multi commands
        if(argc <= i + 1) continue;
        if(strcmp(argv[i], "-i")  == 0 || strcmp(argv[i], "--input") == 0) {
            strcpy(g_cfg.wd, argv[i+1]);
        }
        if(strcmp(argv[i], "-p")  == 0 || strcmp(argv[i], "--pattern") == 0) {
            if(strlen(argv[i+1]) > sizeof(g_cfg.format) - 2) {
                printf("Pattern is too long!%s\n", argv[i+1]);
                return false;
            }
            strncpy(g_cfg.format, argv[i+1], sizeof(g_cfg.format) - 1);
            g_cfg.format[sizeof(g_cfg.format) - 1] = '\0';
        }
        if(strcmp(argv[i], "-l")  == 0 || strcmp(argv[i], "--lang") == 0) {
            if(strlen(argv[i+1]) > sizeof(g_cfg.lang) - 2) {
                printf("Lang is too long!%s\n", argv[i+1]);
                return false;
            }
            strncpy(g_cfg.lang, argv[i+1], sizeof(g_cfg.lang) - 2);
            g_cfg.lang[strlen(argv[i+1])] = ','; // add another , for comparison
            g_cfg.lang[sizeof(g_cfg.lang) - 1] = '\0';
        }
    }
    return true;
}

void cfgPrintDbg() {
    char l[4];
    strncpy(l, "eng", 3);
    l[3] = ',';
    if(strstr("en,eng,rus", l) != NULL) printf("Found!\n");
    printf("%s\n", l);
    printf("NoGui:%d, Quiet:%d, AutoAll:%d\nFormat:%s\nLang:%s\nRoot:%s\nCfgPath:%s\nCWD:%s\nWD:%s", 
        g_cfg.noGui,
        g_cfg.quiet,
        g_cfg.autoCheckAll,
        g_cfg.format,
        g_cfg.lang,
        g_cfg.rootPath,
        g_cfg.configPath,
        g_cfg.cwd,
        g_cfg.wd
    );
}
