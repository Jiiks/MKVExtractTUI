/* extractor.c
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#include "extractor.h"

void extractorExtractTrack(Track *track, int screenIdx, ExtractorProgressUpdate cb) {
    track->ExtractProgress += 1;
    if(cb) cb(track, screenIdx);
}