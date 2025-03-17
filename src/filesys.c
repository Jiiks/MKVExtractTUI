/* filesys.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include "filesys.h"
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include "config.h"

FileList fsScanDir(const char *path, const char *filter, size_t initialSize, bool singleFile) {
    if(singleFile) initialSize = 1;
    FileList fileList = { NULL, NULL, 0, initialSize };
    fileList.path = strdup(path);

    if(singleFile) {
        fileList.files = malloc(fileList.capacity * sizeof(FileInfo));
        fileList.singleFile = true;
        if(access(path, F_OK) != 0) {
            return fileList;
        }

        char *dirc, *basec, *bname, *dname;
        dirc = strdup(path);
        basec = strdup(path);
        dname = dirname(dirc);
        bname = basename(basec);
       
        fsAddFile(&fileList, bname, dname);

        return fileList;
    }

    fileList.singleFile = false;

    struct dirent *entry;
    DIR *dir = opendir(path);

    if(dir == NULL) {
        perror("scandir fail");
        return fileList;
    } else if (ENOENT == errno) {
        perror("not a directory");
        return fileList;
    }

    fileList.files = malloc(fileList.capacity * sizeof(FileInfo));

    while((entry = readdir(dir))) {
        if(entry->d_type != DT_REG) continue;
        const char *ext = strrchr(entry->d_name, '.');
        if(ext && strcmp(ext, filter) == 0) fsAddFile(&fileList, entry->d_name, path);
    }

    closedir(dir);
    return fileList;
}

void fsSetupTracks(FileInfo *fi, const int trackCount) {
    fi->trackCount = trackCount;
    fi->tracks = malloc(trackCount * sizeof(Track));
}

void fsAddTrack(FileInfo *fi, Track *track, const int idx) {
    fi->tracks[fi->trackCount] = *track;
    fi->trackCount++;
    int fl = strlen(track->Flags);
    if(fl > fi->lt) fi->lt = fl; // Set lt for gui reasons
}

void fsGetTracks(FileInfo *fileInfo) {}

cJSON *fsGetTracksJson(FileInfo *fileInfo) {
    char command[PATH_MAX];
    snprintf(command, sizeof(command), "mkvmerge -J '%s'", fileInfo->fullPath);

    char buffer[1024];
    size_t output_size = 1024;
    size_t current_length = 0;

    char *output = malloc(output_size);
    if (output == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    output[0] = '\0';

    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen");
        free(output);
        return NULL;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t buffer_length = strlen(buffer);

        if (current_length + buffer_length >= output_size) {
            output_size *= 2;
            char *new_output = realloc(output, output_size);
            if (new_output == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                free(output);
                pclose(fp);
                return NULL;
            }
            output = new_output;
        }

        strncat(output + current_length, buffer, buffer_length);
        current_length += buffer_length;
    }
    pclose(fp);

    cJSON *ret = cJSON_Parse(output);
    free(output);
    return ret;
}

void fsAddFile(FileList *list, const char *name, const char *path) {
    if(list->size == list->capacity) {
        list->capacity *= 2;
        FileInfo *tmp = realloc(list->files, list->capacity * sizeof(FileInfo));
        if(tmp) {
            list->files = tmp;
        } else {
            perror("Realloc fail");
            return;
        }
    }

    FileInfo *last = &list->files[list->size];
    last->trackCount = 0;
    last->lt = 0;
    last->selectedIndex = 0;
    if(last->tracks != NULL) {
        free(last->tracks);
        last->tracks = NULL;
    }
    last->name = strdup(name);
    last->path = strdup(path);

    int fpl = strlen(path) + strlen(name) + 2;
    last->fullPath = malloc(fpl * sizeof(char));
    sprintf(last->fullPath, "%s/%s", path, name);

    list->size++;
}

void fsFreeList(FileList *list) {
    if(list == NULL) return;
    if(list->files != NULL) {
        for(size_t i = 0 ; i < list->size ; i++) {
            FileInfo *file = &list->files[i];
            free(file->name);
            file->name = NULL;
            free(file->path);
            file->path = NULL;
            free(file->fullPath);
            file->fullPath = NULL;
            free(file->tracks);
            file->tracks = NULL;
            file->trackCount = 0;
        }
    
        free(list->files);
        list->files = NULL;
    }

    free(list->path);
    list->path = NULL;
    list->size = 0;
    list->capacity = 0;
}

int fsCompareFiByName(const void *va, const void *vb) {
    FileInfo *a = (FileInfo *)va;
    FileInfo *b = (FileInfo *)vb;
    return strcmp(a->name, b->name);
}

void fsSortList(FileList *list) {
    qsort(list->files, list->size, sizeof(FileInfo), fsCompareFiByName);
}

void fsCleanup() {

}
