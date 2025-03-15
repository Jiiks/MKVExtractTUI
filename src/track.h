/* track.h
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#ifndef TRACK_H
#define TRACK_H

#include "cJSON.h"
#include <stdbool.h>

typedef enum {
    CodecSrt,
    CodecSsa,
    CodecVobSub,
    CodecAss,
    CodecUnknown,
    trackCodecCount
} trackCodec;

/// @brief Get the string represenation of a codec
/// @param c Track codec
/// @return String represenation of a codec
const char *trackGetCodecName(trackCodec c);

/// @brief Representation of a track
typedef struct {
    int Idx;                // Track index. This is the index used with mkvextract not the track #
    int Num;                // Track number
    char Name[64];          // Track name. 64 should be enough
    char NewName[256];
    char Type[10];          // Track type
    char Language[4];       // 2 or 3 char language tag
    char Extension[4];      // Extension
    char CodecStr[20];      // Codec
    char Encoding[8];       // Encoding
    char Flags[64];         // Flags
    trackCodec Codec;       // Codec
    int ExtractProgress;    // Extraction progress %
    bool IsSubTrack;        // Is a subtitle track.
    bool Extract;           // Should extract?
    bool Forced;            // Is the track forced
    bool Default;           // Is the track default
    bool ClosedCaptions;    // Is closed captions
    bool Commentary;        // Is commentary
    bool Enabled;           // Is enabled
    bool HearingImpaired;   // Is the track hearing impaired
    bool OriginalLanguage;  // Is the track original language
    bool TextSubtitles;     // Text subtitles
} Track;

void trackResolveNewName(const char *fileName, Track *track);

/// @brief Parses track json
/// @param json Track json string
/// @return Parse json as track
Track trackParseJson(cJSON *json);
/// @brief Debug print track 
/// @param track Track
void trackPrintDbg(const Track *track);

#endif // TRACK_H
