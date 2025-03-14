/* track.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include "track.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// TODO missing some codecs
const char *trackCodecNames[5] = {
    "srt",
    "ssa",
    "sub",
    "ass",
    "und"
};

const char *trackGetCodecName(trackCodec c) {
    if (c >= 0 && c < trackCodecCount) {
        return trackCodecNames[c];
    } 
    return "und";
}


bool checkName(const char *name, const char *what) {
    if(strcasestr(name, what) != NULL) return true;
    return false;
}

void appendFlag(char *flags, const char *append, size_t size) {
    size_t curLen = strlen(flags);
    size_t appLen = strlen(append);

    if (curLen + appLen < size) {
        strcat(flags, append);
    } else {
        fprintf(stderr, "Not enough space to append '%s'\n", append);
    }
}

/// @brief Checks if track should be auto checked based on language configs.
/// @param track Track to check
void trackAutoCheck(Track *track) {
    // Default config = en,eng
    if(strstr(track->Language, "eng")) track->Extract = true;
}

Track trackParseJson(cJSON *json) {
    Track track;
    track.IsSubTrack = true;
    track.Extract = false;
    track.ExtractProgress = 0;
    track.Forced = false;
    track.Default = false;
    track.ClosedCaptions = false;
    track.Commentary = false;
    track.Enabled = false;
    track.HearingImpaired = false;
    track.OriginalLanguage = false;
    track.TextSubtitles = false;

    strncpy(track.Type, "subtitles\0", 10);
    //cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "id");
    track.Idx = cJSON_GetObjectItemCaseSensitive(json, "id")->valueint;
    cJSON *codec = cJSON_GetObjectItemCaseSensitive(json, "codec");
    strncpy(track.CodecStr, codec->valuestring, sizeof(track.CodecStr) - 1);
    track.CodecStr[sizeof(track.CodecStr) - 1] = '\0';

    // Props
    cJSON *props = cJSON_GetObjectItemCaseSensitive(json, "properties");

    // Resolve codec
    cJSON *codec_id = cJSON_GetObjectItemCaseSensitive(props, "codec_id");
    if(strstr(codec_id->valuestring, "S_TEXT/UTF8") != NULL) track.Codec = CodecSrt;
    else if(strstr(codec_id->valuestring, "S_TEXT/SSA") != NULL) track.Codec = CodecSsa;
    else if(strstr(codec_id->valuestring, "S_VOBSUB") != NULL) track.Codec = CodecVobSub;
    else if(strstr(codec_id->valuestring, "S_TEXT/ASS") != NULL) track.Codec = CodecAss;
    else track.Codec = CodecUnknown;

    // Flags
    track.Default = cJSON_GetObjectItemCaseSensitive(props, "default_track")->valueint;
    track.Enabled = cJSON_GetObjectItemCaseSensitive(props, "enabled_track")->valueint;
    track.Forced = cJSON_GetObjectItemCaseSensitive(props, "forced_track")->valueint;
    track.TextSubtitles = cJSON_GetObjectItemCaseSensitive(props, "text_subtitles")->valueint;

    cJSON *encoding = cJSON_GetObjectItemCaseSensitive(props, "encoding");
    strncpy(track.Encoding, encoding->valuestring, sizeof(track.Encoding) - 1);
    track.Encoding[sizeof(track.Encoding) - 1] = '\0';

    cJSON *language = cJSON_GetObjectItemCaseSensitive(props, "language");
    strncpy(track.Language, language->valuestring, sizeof(track.Language) - 1);
    track.Language[sizeof(track.Language) - 1] = '\0';

    track.Num = cJSON_GetObjectItemCaseSensitive(props, "number")->valueint;
    
    cJSON *track_name = cJSON_GetObjectItemCaseSensitive(props, "track_name");
        if(track_name != NULL) {
        strncpy(track.Name, track_name->valuestring, sizeof(track.Name) - 1);
        track.Name[sizeof(track.Name) - 1] = '\0';

        if(!track.Default) track.Default = checkName(track_name->valuestring, "default");
        if(!track.Forced) track.Forced = checkName(track_name->valuestring, "forced");
        if(!track.HearingImpaired) track.HearingImpaired = checkName(track_name->valuestring, "sdh");
    }

    // cJSON *uid = cJSON_GetObjectItemCaseSensitive(props, "uid"); we don't care about the uid

    // Flags string
    strcpy(track.Flags, "");
    if(track.Default) appendFlag(track.Flags, "default.", sizeof(track.Flags));
    if(track.OriginalLanguage) appendFlag(track.Flags, "original.", sizeof(track.Flags));
    if(track.Forced) appendFlag(track.Flags, "forced.", sizeof(track.Flags));
    if(track.HearingImpaired) appendFlag(track.Flags, "sdh.", sizeof(track.Flags));
    if(track.ClosedCaptions) appendFlag(track.Flags, "cc.", sizeof(track.Flags));
    if(track.Commentary) appendFlag(track.Flags, "commentary.", sizeof(track.Flags));

    strcpy(track.Extension, trackGetCodecName(track.Codec));
    track.Extension[sizeof(track.Extension) - 1] = '\0';
    trackAutoCheck(&track);
    return track;
}

void removeExt(char *filename) {
    char *dot = strrchr(filename, '.');
    if (dot != NULL) {
        *dot = '\0';
    }
}

void trackResolveNewName(const char *fileName, Track *track, char *out) {
    char *pos;
    char buffer[256];
    char tempOut[256];
    strcpy(tempOut, "fn.flags.lang.ext");
    strcpy(buffer, fileName);
    
    removeExt(buffer);

    // Replace fn in format with the filename
    while((pos = strstr(tempOut, "fn")) != NULL) {
        size_t len = strlen(buffer);
        memmove(pos + len, pos + 2, strlen(pos + 2) + 1);
        memcpy(pos, buffer, len);
    }

    // Replace flags in format with flags
    while((pos = strstr(tempOut, "flags.")) != NULL) {
        size_t len = strlen(track->Flags);
        memmove(pos + len, pos + 6, strlen(pos + 6) + 1);
        memcpy(pos, track->Flags, len);
    }

    // Replace lang in format with language tag
    while((pos = strstr(tempOut, "lang")) != NULL) {
        size_t len = strlen(track->Language);
        memmove(pos + len, pos + 4, strlen(pos + 4) + 1);
        memcpy(pos, track->Language, len);
    }

    // Replace extension with correct one. For now just srt
    while((pos = strstr(tempOut, "ext")) != NULL) {
        memmove(pos + 3, pos + 3, strlen(pos + 3) + 1);
        memcpy(pos, "srt", 3);
    }

    strcpy(out, tempOut);
}

void trackPrintDbg(const Track *track) {
        printf("Id(%d):%d Type:%s Codec:%s[%s] Ext:%s Default:%d Enabled:%d Encoding:%s Forced:%d SDH:%d Lang:%s TS:%d Name:%s Flags:%s\n",
            track->Idx,                         // Id
            track->Num,                         // Num
            track->Type,                        // Type
            trackGetCodecName(track->Codec),    // Codec name
            track->CodecStr,                    // Codec string
            track->Extension,                   // Extension
            track->Default,                     // Is default?
            track->Enabled,                     // Is Enabled?
            track->Encoding,                    // Encoding
            track->Forced,                      // Is forced?
            track->HearingImpaired,             // Is hearing impaired?
            track->Language,                    // Language tag
            track->TextSubtitles,               // Is text subtitles?
            track->Name,                        // Track name
            track->Flags                        // Flags
        );
}
