/* config.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#define CONFIG_FOLDER "MkvExtractTUI"
#define CONFIG_FILE "config.json"

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "cJSON.h"
#include <sys/stat.h>

bool statFileExists(char *filename) {
    struct stat buffer;   
    return (stat (filename, &buffer) == 0);
}

Config g_cfg;

// Reset config to default values
bool reset() {
    snprintf(g_cfg.autoCheck, sizeof(g_cfg.autoCheck), "en,eng");
    snprintf(g_cfg.lang, sizeof(g_cfg.lang), "en,eng,");
    snprintf(g_cfg.format, sizeof(g_cfg.format), "fn.flags.lang.ext");
    g_cfg.noGui = false;
    g_cfg.quiet = false;
    g_cfg.autoCheckAll = false;
    g_cfg.fastUpdate = false;
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

int cfgLoad() {
    if(access(g_cfg.rootPath, F_OK) == -1) {
        printf("Config directory does not exist; creating %s\n", g_cfg.rootPath);
        if(mkdir(g_cfg.rootPath, 0755) == -1) {
            perror("Error creating directory");
            return 1;
        }
    }
    
    if(!statFileExists(g_cfg.configPath)) {
        printf("Config file does not exist in %s\n", g_cfg.configPath);
        return cfgSave();
    }

    FILE *file = fopen(g_cfg.configPath, "r");

    char buffer[1024];
    int len = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);

    cJSON *cjConf = cJSON_Parse(buffer);
    if(cjConf == NULL) {
        const char *err = cJSON_GetErrorPtr();
        if(err != NULL) {
            printf("Json Error: %s\n", err);
        }
        cJSON_Delete(cjConf);
        return 1;
    }

    const cJSON *cjAutoCheck = cJSON_GetObjectItemCaseSensitive(cjConf, "auto_check");
    if(cjAutoCheck != NULL) {
        int len = strlen(cjAutoCheck->valuestring);
        if(len > sizeof(g_cfg.lang) - 2) {
            printf("Lang is too long!%s\n", cjAutoCheck->valuestring);
            cJSON_Delete(cjConf);
            return 1;
        }
        strncpy(g_cfg.lang, cjAutoCheck->valuestring, sizeof(g_cfg.lang) - 2);
        strncpy(g_cfg.autoCheck, cjAutoCheck->valuestring, sizeof(g_cfg.autoCheck) - 2);
        g_cfg.lang[len] = ','; // add another , for comparison
        g_cfg.lang[sizeof(g_cfg.lang) - 1] = '\0';
        g_cfg.autoCheck[sizeof(g_cfg.autoCheck) - 1] = '\0';
    }

    const cJSON *cjPattern = cJSON_GetObjectItemCaseSensitive(cjConf, "pattern");
    if(cjPattern != NULL) {
        int len = strlen(cjPattern->valuestring);
        if(len > sizeof(g_cfg.format) - 2) {
            printf("Pattern is too long!%s\n", cjPattern->valuestring);
            cJSON_Delete(cjConf);
            return 1;
        }
        strncpy(g_cfg.format, cjPattern->valuestring, sizeof(g_cfg.format) - 1);
        g_cfg.format[sizeof(g_cfg.format) - 1] = '\0';
    }


    const cJSON *cjNoGui = cJSON_GetObjectItemCaseSensitive(cjConf, "no_gui");
    if(cjNoGui != NULL) g_cfg.noGui = cjNoGui->valueint;
    const cJSON *cjQuiet = cJSON_GetObjectItemCaseSensitive(cjConf, "quiet");
    if(cjQuiet != NULL) g_cfg.quiet = cjQuiet->valueint;
    const cJSON *cjAutoCheckAll = cJSON_GetObjectItemCaseSensitive(cjConf, "auto_check_all");
    if(cjAutoCheckAll != NULL) g_cfg.autoCheckAll = cjAutoCheckAll->valueint;
    const cJSON *cjFastUpdate = cJSON_GetObjectItemCaseSensitive(cjConf, "fast_update");
    if(cjFastUpdate != NULL) g_cfg.fastUpdate = cjFastUpdate->valueint;

    cJSON_Delete(cjConf);
    return 0;
}

int cfgSave() {
    FILE *file = fopen(g_cfg.configPath, "w");
    if(file == NULL) {
        perror("Error opening config file for writing.");
        return 1;
    }
    //setvbuf(file, NULL, _IONBF, 0);
    fprintf(file, "{");
    fprintf(file, "\n    \"auto_check\": \"%s\",", g_cfg.autoCheck);
    fprintf(file, "\n    \"pattern\": \"%s\",", g_cfg.format);
    fprintf(file, "\n    \"no_gui\": %s,", (g_cfg.noGui ? "true" : "false"));
    fprintf(file, "\n    \"quiet\": %s,", (g_cfg.quiet ? "true" : "false"));
    fprintf(file, "\n    \"auto_check_all\": %s,", (g_cfg.autoCheckAll ? "true" : "false"));
    fprintf(file, "\n    \"fast_update\": %s", (g_cfg.fastUpdate ? "true" : "false"));
    fprintf(file, "\n}\n");

    if(file == NULL) {
        printf("File is null\n");
        return 1;
    } else {
        fflush(file);
        fsync(fileno(file));
        fclose(file);
        return 0;
    }
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
        if(strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--fastupdate") == 0) {
            g_cfg.fastUpdate = true;
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
            strncpy(g_cfg.autoCheck, argv[i+1], sizeof(g_cfg.autoCheck) - 2);
            g_cfg.lang[strlen(argv[i+1])] = ','; // add another , for comparison
            g_cfg.lang[sizeof(g_cfg.lang) - 1] = '\0';
            g_cfg.autoCheck[sizeof(g_cfg.autoCheck) - 1] = '\0';
        }
    }
    return true;
}

void cfgPrintDbg() {
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

