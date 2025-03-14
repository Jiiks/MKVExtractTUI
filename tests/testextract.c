/* testfs.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include <ncurses.h>
#include "../src/cJSON.h"
#include "../src/filesys.h"
#include "../src/guiextract.h"

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
    
    cJSON_ArrayForEach(track, tracks) {
        if(track == NULL) {
            row += 1;
            continue;
        }
        
        cJSON *type = cJSON_GetObjectItemCaseSensitive(track, "type");
        // Ignore non sub tracks since we don't currently care about those
        if(strstr(type->valuestring, "subtitles") == NULL) continue;
        Track *parsedTrack = trackParseJson(track);
        fsAddTrack(fi, parsedTrack);
        row += 1;
    }

    cJSON_Delete(json);
    json = NULL;

    guiExtractInit(&fl);

    while(1) {
        int ch = getch();
        if(ch == 'q') break;
    }

    guiExtractClean();
    endwin();
    fsFreeList(&fl);
    return 0;
}