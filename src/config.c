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
    strcpy(g_cfg.autoCheck, "en,eng");
    strcpy(g_cfg.format, "fn.flags.lang.ext");
    g_cfg.noGui = false;
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
