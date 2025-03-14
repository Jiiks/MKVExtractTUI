/* testfs.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include <string.h>
#include <stdio.h>
#include "../src/filesys.h"
#include "../src/cJSON.h"

int main(int argc, char *argv[]) {
    printf("Testing filesys path: %s\n", argv[1]);
    FileList fl = fsScanDir(argv[1], argv[2], 16);

    printf("Found %d files matching filter %s\n", (int)fl.size, argv[2]);

    fsSortList(&fl);

    for(int i = 0 ; i < fl.size ; i++) {
        FileInfo *fi = &fl.files[i];
        printf("Name: %s, Path: %s, FullPath: %s\n", fi->name, fi->path, fi->fullPath);
    }

    int fileCount = fl.size;
    printf("%s\n", "Testing track detection");

    for(int i = 0 ; i < fileCount ; i++) {
        FileInfo *fi = &fl.files[i];
        cJSON *json = fsGetTracksJson(fi);
        cJSON *tracks = cJSON_GetObjectItemCaseSensitive(json, "tracks");
        printf("%s Tracks: %d\n", fi->name, cJSON_GetArraySize(tracks));
        cJSON_Delete(json);
    }


    fsCleanup();
    fsFreeList(&fl);

    perror("testfs");

    return 0;
}
