/* extractor.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include "extractor.h"

#include <time.h>

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>

int extractorExtractTrack(FileList *fl, FileInfo *fi, Track *track, const char* fileName, int screenIdx, ExtractorProgressUpdate cb) {
    char command[4096];
    snprintf(command, sizeof(command), "mkvextract '%s' tracks %d:'%s/_test_%s'", fi->fullPath, track->Idx, fi->path, track->NewName);
    
    char buf[1024];
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        printf("popen error %s\n", "err");
        perror("popen");
        return 1;
    }

    // Get the file descriptor from FILE *
    int d = fileno(fp); 
     // Set the file descriptor to non-blocking
    fcntl(d, F_SETFL, O_NONBLOCK);
    
    while (1) {
        // Read data into buffer
        ssize_t r = read(d, buf, sizeof(buf)); 
        if (r == -1) {
            if (errno == EAGAIN) { 
                // No data yet
                usleep(100000); 
            } else {
                perror("read");
                pclose(fp);
                return 1;
            }
        } else if (r > 0) {
            buf[r] = '\0';

            char *pstr = strstr(buf, "Progress: ");
            if (pstr != NULL) {
                pstr += strlen("Progress: ");
                char *percentSign = strchr(pstr, '%');
                if (percentSign != NULL) {
                    *percentSign = '\0';
                    int x = atoi(pstr);
                    track->ExtractProgress = x;
                    if(cb) cb(fl, fi, track, screenIdx);
                }
            }
        } else {
            break;
        }
    }

    pclose(fp);
    track->ExtractProgress = 100;
    if(cb) cb(fl, fi, track, screenIdx);
    return 0;
}