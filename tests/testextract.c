/* testfs.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include <ncurses.h>
#include "../src/cJSON.h"
#include "../src/filesys.h"
#include "../src/guiextract.h"
#include "../src/extractor.h"
#include "../src/utils.h"

void extractorCb(FileList *fl, FileInfo *fi, Track *track, int screenIdx) {
    guiExtractUpdateAt(screenIdx, fi, track);
}

int main(int argc, char *argv[]) {

    // Since this is isolated let's setup ncurses here
    initscr();              // Init screen
    start_color();          // Enable colors
    noecho();               // Don't echo inputs
    cbreak();               // Disables line buffering and erase/kill character-processing 
    curs_set(0);            // Disable cursor
    keypad(stdscr, TRUE);   // Enable keyboard input
    refresh();

    // Get files in test path
    FileList fl = fsScanDir(argv[1], argv[2], 16);
    fsSortList(&fl);

    FileInfo *fi = &fl.files[0];
    printf("Using file: %s\n", fi->fullPath);

    cJSON *json = fsGetTracksJson(fi);
    if(json == NULL) {
        printf("json is null");
        fsFreeList(&fl);
        return 1;
    }

    cJSON *tracks = NULL;
    tracks = cJSON_GetObjectItemCaseSensitive(json, "tracks");
    if(tracks == NULL) {
        printf("Tracks is null");
        fsFreeList(&fl);
        cJSON_Delete(json);
        json = NULL;
        return 1;
    }

    int row = 0;
    cJSON *track = NULL;
    int trackCount = cJSON_GetArraySize(tracks);
    // Allocate tracks
    if(trackCount > 0) {
        if(fi->tracks != NULL) {
            free(fi->tracks);
            fi->tracks = NULL;
        }
        fi->tracks = malloc(trackCount * sizeof(Track));
    }
    
    cJSON_ArrayForEach(track, tracks) {
        if(track == NULL) {
            row += 1;
            continue;
        }
        
        cJSON *type = cJSON_GetObjectItemCaseSensitive(track, "type");
        // Ignore non sub tracks since we don't currently care about those
        if(strstr(type->valuestring, "subtitles") == NULL) continue;
        Track parsedTrack = trackParseJson(track);
        trackResolveNewName(fi->name, &parsedTrack);
        fsAddTrack(fi, &parsedTrack, row);
        row += 1;
    }

    cJSON_Delete(json);
    json = NULL;

    guiExtractInit(&fl);
    int screenIdx = 0;
    for(int i = 0 ; i < fl.files[0].trackCount ; i++) {
        if(fl.files[0].tracks[i].Extract) {
            extractorExtractTrack(&fl, &fl.files[0], &fl.files[0].tracks[i], fl.files[0].name, screenIdx, extractorCb);
            screenIdx++;
        }
    }
    guiExtractUpdate(&fl);

    while(1) {
        int ch = getch();
        if(ch == 'q') break;
    }

    guiExtractClean();
    endwin();
    fsFreeList(&fl);
    return 0;
}