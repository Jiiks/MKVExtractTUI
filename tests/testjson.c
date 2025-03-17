/* testjson.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include "../src/cJSON.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../src/config.h"
#include "../src/filesys.h"

int main(int argc, char *argv[]) {
    cfgInit();
    printf("Testing filesys path: %s\n", argv[1]);
    FileList fl = fsScanDir(argv[1], argv[2], 16, false);

    printf("Using file: %s\n", fl.files[0].fullPath);

    cJSON *json = fsGetTracksJson(&fl.files[0]);

    if(json == NULL) {
        printf("json is null");
        fsFreeList(&fl);
        return 1;
    }

    cJSON *tracks = NULL;
    tracks = cJSON_GetObjectItemCaseSensitive(json, "tracks");
    printf("Tracks is null? %s\n", tracks == NULL ? "yes" : "no");

    cJSON *track = NULL;
    cJSON_ArrayForEach(track, tracks) {
        if(track == NULL) {
            printf("Track is somehow null\n");
            continue;
        }
        cJSON *type = cJSON_GetObjectItemCaseSensitive(track, "type");
        // Ignore non sub tracks since we don't currently care about those
        if(strstr(type->valuestring, "subtitles") == NULL) continue;
        Track parsedTrack = trackParseJson(track);
        trackPrintDbg(&parsedTrack);
    }

    cJSON_Delete(json);
    json = NULL;
    fsFreeList(&fl);
    perror("testjson");
    return 0;
}
