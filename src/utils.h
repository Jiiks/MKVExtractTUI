/* utils.h
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>

/// @brief Sleep for x milliseconds
/// @param ms milliseconds to sleep for
void mssleep(int ms) {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = ms * 1000000L;

    nanosleep(&ts, NULL);
}

/// @brief Get current working directory.
/// @param cwd 
/// @param size 
/// @return true if success. false if fail.
bool utilsGetCwd(char *cwd, size_t size) {
    if(getcwd(cwd, size) != NULL) {
        #ifdef DEBUG
        printf("CWD:%s\n", cwd);
        #endif
        return true;
    }
    perror("getcwd() error");
    return false;
}

#endif