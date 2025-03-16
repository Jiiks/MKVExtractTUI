/* config.h
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#ifndef CONFIG_H
#define CONFIG_H

#define MKVE_VERSION "0.1.2a"
#define MKVE_WINDOW_TITLE "MKV(Sub)Extract Terminal UI"
#define MKVE_REPO "https://github.com/jiiks/MKVExtractTUI"
#define MKVE_ISSUES "https://github.com/jiiks/MKVExtractTUI/issues"
#define MKVE_README "https://github.com/jiiks/MKVExtractTUI/blob/main/README.md"


#include <stdbool.h>
#include <limits.h>
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif // PATH_MAX

// Config file struct
typedef struct {
    char autoCheck[100];        // Languages to auto check. Default: "en,eng"
    bool noGui;                 // Run guiless? Default: false
    char format[100];           // File renaming format. Default: fn.flags.lang.ext eg. Movie.sdh.en.srt
    char rootPath[PATH_MAX];    // Root path for config. Should be (~/.config|/home/user/.config)/MkvExtractTUI
    char configPath[PATH_MAX];  // Config file path. rootpath/config.cfg
    char cwd[PATH_MAX];         // Current working directory.
    char wd[PATH_MAX];          // Working directory. Supplied through command line args.
} Config;

// Global config
extern Config g_cfg;

/// @brief 
/// Config initializer. Resets config to default values and resolves paths.
/// @return true if success. false if fail.
bool cfgInit();

#endif // CONFIG_H
