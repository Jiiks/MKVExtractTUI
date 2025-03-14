/* testfs.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include <string.h>
#include <stdio.h>
#include "../src/filesys.h"

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

    // "pre"compile regexes
    //if(fsCompileRegexes() == 0) {
    //    fsCleanup();
    //    fsFreeList(&fl);
    //    return 0;
    //}

    for(int i = 0 ; i < fileCount ; i++) {
        FileInfo *fi = &fl.files[i];
        fsGetTracks(fi);
        printf("File: %s | Tracks: %d\n", fi->name, fi->trackCount);
        for(int ii = 0 ; ii < fl.files[i].trackCount ; ii++) {
            Track *track = &fi->tracks[ii];
            if(ii != 0) printf(" | ");
            printf("#%d [%s] %s(%s) [%d,%d,%d,%d]", track->Idx, track->Language, fsGetCodecName(track->Codec), track->CodecStr, 
            track->Forced, track->Default, track->HearingImpaired, track->OriginalLanguage);
        }
        printf("\n");
    }
    fsCleanup();
    fsFreeList(&fl);

    perror("testfs");

    return 0;
}
