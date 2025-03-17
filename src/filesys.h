/* filesys.h
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#ifndef FILESYS_H
#define FILESYS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>
#include "track.h"

#ifndef DT_REG
#define DT_REG 8
#endif

typedef struct {
    char *name;
    char *path;
    char *fullPath;
    Track *tracks;
    int trackCount;
    int lt;
    int selectedIndex;
} FileInfo;

/// @brief A list of files. Make sure to call fsFreeList after.
typedef struct {
    char *path;
    FileInfo *files;
    size_t size;
    size_t capacity;
} FileList;

/// @brief Scan a directory for files.
/// @param path path to scan.
/// @param filter only return files with specified extension.
/// @return FileList of files in directory.
FileList fsScanDir(const char *path, const char *filter, size_t initialSize, bool singleFile);

void fsSetupTracks(FileInfo *fi, const int trackCount);
void fsGetTracks(FileInfo *fileInfo);

cJSON *fsGetTracksJson(FileInfo *fileInfo);

/// @brief Add file to filelist
/// @param list List to add file into
/// @param name File name
/// @param path File path
void fsAddFile(FileList *list, const char *name, const char *path);

/// @brief Free a filelist, also frees all files in the list.
/// @param list List to free
void fsFreeList(FileList *list);

/// @brief Sort a filelist
/// @param list  List to sort
void fsSortList(FileList *list);

/// @brief Add track to fileinfo
/// @param fi FileInfo to add track to
/// @param track Track to add
/// @param idx Track index
void fsAddTrack(FileInfo *fi, Track *track, const int idx);

void fsCleanup();

#endif // FILESYS_H
