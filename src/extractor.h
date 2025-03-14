/* extractor.h
 *https://github.com/Jiiks/MKVExtractTUI
 *Distributed under the MIT License [ https://opensource.org/licenses/MIT ]
 *Copyright (c) 2025-present Jiiks https://github.com/Jiiks
*/

#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include "filesys.h"

typedef void (*ExtractorProgressUpdate)(Track *track, int screenIdx);

void extractorExtractTrack(Track *track, int screenIdx, ExtractorProgressUpdate cb);

#endif // EXTRACTOR_H