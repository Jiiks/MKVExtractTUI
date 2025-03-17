/* track.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include "track.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"

// TODO missing some codecs
const char *trackCodecNames[6] = {
    "srt",
    "ssa",
    "sub",
    "ass",
    "sup",
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
    char l[5];
    strncpy(l, track->Language, 3);
    l[3] = ',';
    l[4] = '\0';
    if(strstr(g_cfg.lang, l) != NULL) track->Extract = true;
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

    // Props
    cJSON *props = cJSON_GetObjectItemCaseSensitive(json, "properties");

    // Index
    track.Idx = -1; // Set to -1 in case id fails
    const cJSON *cjId = cJSON_GetObjectItemCaseSensitive(json, "id");
    if(cjId != NULL) track.Idx = cjId->valueint;
    
    // Codec
    const cJSON *cjCodec = cJSON_GetObjectItemCaseSensitive(json, "codec");
    if(cjCodec != NULL) {
        strncpy(track.CodecStr, cjCodec->valuestring, sizeof(track.CodecStr) - 1);
        track.CodecStr[sizeof(track.CodecStr) - 1] = '\0';

        // Resolve codec
        const cJSON *cjCodecId = cJSON_GetObjectItemCaseSensitive(props, "codec_id");
        if(cjCodecId != NULL) {
            const char *cjCodecIdVs = cjCodecId->valuestring;
            if(strstr(cjCodecIdVs, "S_TEXT/UTF8") != NULL) track.Codec = CodecSrt;
            else if(strstr(cjCodecIdVs, "S_TEXT/SSA") != NULL) track.Codec = CodecSsa;
            else if(strstr(cjCodecIdVs, "S_VOBSUB") != NULL) track.Codec = CodecVobSub;
            else if(strstr(cjCodecIdVs, "S_TEXT/ASS") != NULL) track.Codec = CodecAss;
            else if(strstr(cjCodecIdVs, "S_HDMV/PGS") != NULL) track.Codec = CodecPgs;
            else track.Codec = CodecUnknown;
        } else {
            track.Codec = CodecUnknown;
        }
    }

    // Flags
    const cJSON *cjDefault = cJSON_GetObjectItemCaseSensitive(props, "default_track");
    const cJSON *cjEnabled = cJSON_GetObjectItemCaseSensitive(props, "enabled_track");
    const cJSON *cjForced = cJSON_GetObjectItemCaseSensitive(props, "forced_track");
    const cJSON *cjTextSubtitles = cJSON_GetObjectItemCaseSensitive(props, "text_subtitles");

    if(cjDefault != NULL) track.Default = cjDefault->valueint;
    if(cjEnabled != NULL) track.Enabled = cjEnabled->valueint;
    if(cjForced != NULL) track.Forced = cjForced->valueint;
    if(cjTextSubtitles != NULL) track.TextSubtitles = cjTextSubtitles->valueint;

    // Encoding
    const cJSON *cjEncoding = cJSON_GetObjectItemCaseSensitive(props, "encoding");
    if(cjEncoding != NULL) {
        strncpy(track.Encoding, cjEncoding->valuestring, sizeof(track.Encoding) - 1);
        track.Encoding[sizeof(track.Encoding) - 1] = '\0';
    } else {
        strncpy(track.Encoding, "unknown\0", 8);
    }

    printf("%zu\n", sizeof(track.Language));

    // Language
    const cJSON *cjLanguage = cJSON_GetObjectItemCaseSensitive(props, "language");
    if(cjLanguage != NULL) {
        strncpy(track.Language, cjLanguage->valuestring, sizeof(track.Language) - 1);
        track.Language[sizeof(track.Language) - 1] = '\0';
    } else {
        strncpy(track.Language, "und\0", 4);
    }

    // Num
    track.Num = -1;
    const cJSON *cjNum = cJSON_GetObjectItemCaseSensitive(props, "number");
    if(cjNum != NULL) {
        track.Num = cjNum->valueint;
    }

    // Name
    const cJSON *cjTrackName = cJSON_GetObjectItemCaseSensitive(props, "track_name");
    if(cjTrackName != NULL) {
        strncpy(track.Name, cjTrackName->valuestring, sizeof(track.Name) - 1);
        track.Name[sizeof(track.Name) - 1] = '\0';
        if(!track.Default) track.Default = checkName(track.Name, "default");
        if(!track.Forced) track.Forced = checkName(track.Name, "forced");
        if(!track.HearingImpaired) track.HearingImpaired = checkName(track.Name, "sdh");
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
    track.Flags[sizeof(track.Flags) - 1] = '\0';

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

void trackResolveNewName(const char *fileName, Track *track) {
    char *pos;
    char buffer[256];

    strcpy(buffer, fileName);
    removeExt(buffer);

    // Start with the format directly in track->NewName
    strcpy(track->NewName, g_cfg.format);

    // Replace "fn" in format with the filename
    while ((pos = strstr(track->NewName, "fn")) != NULL) {
        size_t len = strlen(buffer);
        memmove(pos + len, pos + 2, strlen(pos + 2) + 1);
        memcpy(pos, buffer, len);
    }

    // Replace "flags" in format with flags
    while ((pos = strstr(track->NewName, "flags.")) != NULL) {
        size_t len = strlen(track->Flags);
        memmove(pos + len, pos + 6, strlen(pos + 6) + 1);
        memcpy(pos, track->Flags, len);
    }

    // Replace "lang" in format with language tag
    while ((pos = strstr(track->NewName, "lang")) != NULL) {
        size_t len = strlen(track->Language);
        memmove(pos + len, pos + 4, strlen(pos + 4) + 1);
        memcpy(pos, track->Language, len);
    }

    // Replace "ext" in format with "srt"
    while ((pos = strstr(track->NewName, "ext")) != NULL) {
        memmove(pos + 3, pos + 3, strlen(pos + 3) + 1);
        memcpy(pos, track->Extension, 3);
    }
}

void trackPrintDbg(const Track *track) {
        printf("Id(%d):%d Type:%s Codec:%s[%s] Ext:%s Default:%d Enabled:%d Encoding:%s Forced:%d SDH:%d Lang:%s TS:%d Name:%s Flags:%s AC:%d\n",
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
            track->Flags,                       // Flags
            track->Extract
        );
}
